#include <stdio.h>
#include <string.h>
#include "config.h"
#ifdef HAVE_BSD_STRING_H
#include <bsd/string.h>
#endif
#include <sys/time.h>
#include <unistd.h> // write(), read(), close()
#include <errno.h> // Error integer and strerror() function
#include <fcntl.h> // Contains file controls like O_RDWR

#include "fnSystem.h"
#include "../../include/debug.h"

#include "netsio.h"
#include "netsio_proto.h"

/* alive response timeout in seconds
 *  device sends in regular intervals (2 s) alive messages (NETSIO_ALIVE_REQUEST) to NetSIO HUB
 *  if the device will not receive alive response (NETSIO_ALIVE_RESPONSE) within ALIVE_TIMEOUT period
 *  the connection to the HUB is considered as expired/broken and new connection attempt will
 *  be made
 */
#define ALIVE_TIMEOUT   5

// Constructor
NetSioPort::NetSioPort() :
    _initialized(false),
    _fd(-1),
    _host{0},
    _ip(IPADDR_NONE),
    _port(NETSIO_PORT),
    _command_asserted(false),
    _byte_pending(false),
    _baud(SIOPORT_DEFAULT_BAUD)
{}

NetSioPort::~NetSioPort()
{
    end();
}

void NetSioPort::begin(int baud)
{
    if (_initialized) 
    {
        end();
    }

    _errcount = 0;
    _suspend_time = 0;

    _command_asserted = false;
    _byte_pending = false;


    Debug_printf("Setting up NetSIO (%s:%d)\n", _host, _port);
    _fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

    if (_fd < 0)
    {
		perror("Failed to create NetSIO socket");
        suspend();
		return;
	}
    
    _ip = get_ip4_addr_by_name(_host);
    if (_ip == IPADDR_NONE)
    {
        Debug_println("Failed to resolve NetSIO host name");
        suspend();
		return;
    }

    // Set remote IP address (no real connection is created for UDP socket)
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_addr.s_addr = _ip;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_port);
    if (connect(_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        // should happen (UDP)
		perror("Failed to connect NetSIO socket");
        suspend();
		return;
    }

    fcntl(_fd, F_SETFL, O_NONBLOCK);

    // ping
    // send(_fd, "\xC2", 1, 0);
    // recv(_fd, ...)

    // connect device
    uint8_t connect = NETSIO_DEVICE_CONNECT;
    send(_fd, &connect, 1, 0);

    struct timeval tv;
    gettimeofday(&tv, NULL);
    _alive_time = tv.tv_sec;
    _expire_time = _alive_time + ALIVE_TIMEOUT;

    Debug_printf("### NetSIO initialized ###\n");
    // Set initialized.
    _initialized = true;
    set_baudrate(baud);
}

void NetSioPort::end()
{
    if (_fd >= 0)
    {
        uint8_t disconnect = NETSIO_DEVICE_DISCONNECT;
        send(_fd, &disconnect, 1, 0);
        close(_fd);
        _fd  = -1;
        Debug_printf("### NetSIO stopped ###\n");
    }
    _initialized = false;
}

void NetSioPort::suspend(int sec)
{
    Debug_println("Suspending serial port");
    struct timeval tv;
    gettimeofday(&tv, NULL);
    _suspend_time = tv.tv_sec + sec;
    end();
}

/* Discards anything in the input buffer
*/
void NetSioPort::flush_input()
{
    if (_initialized)
        _byte_pending = false;
}

/* Clears input buffer and flushes out transmit buffer waiting at most
   waiting MAX_FLUSH_WAIT_TICKS until all sends are completed
*/
void NetSioPort::flush()
{
    if (_initialized)
        _byte_pending = false;
}

/* read byte from socket and update internal variables */
int NetSioPort::handle_netsio()
{
    uint8_t rxbuf[3];
    struct timeval tv;
    int result;

    result = recv(_fd, rxbuf, sizeof(rxbuf), 0);
    if (result > 0)
    {
        switch (rxbuf[0])
        {
            case NETSIO_DATA_BYTE:
                _byte_pending = true;
                _byte = rxbuf[1];
                break;
            case NETSIO_COMMAND_OFF:
                _command_asserted = false;
                break;
            case NETSIO_COMMAND_ON:
                _command_asserted = true;
                break;
            case NETSIO_MOTOR_OFF:
            case NETSIO_MOTOR_ON:
                break;
            case NETSIO_SPEED_CHANGE:
                // speed change notification
                if (rxbuf[1] || rxbuf[2]) 
                {
                    unsigned int cpb = rxbuf[1] | (rxbuf[2] << 8);
                    set_baudrate(1789773 / cpb);
                }
                break;
            case NETSIO_ALIVE_RESPONSE:
                // connection alive
                {
                    gettimeofday(&tv, NULL);
                    _expire_time = tv.tv_sec + 5;
                }
            case NETSIO_PING_RESPONSE:
            case NETSIO_WARM_RESET:
                break;
            case NETSIO_COLD_RESET:
                // emulator cold reset, do fujinet restart
                fnSystem.reboot();
                break;
            default:
                break;
        }
    }

    gettimeofday(&tv, NULL);
    if (_alive_time + 2 <= tv.tv_sec)
    {
        _alive_time = tv.tv_sec;
        uint8_t alive = NETSIO_ALIVE_REQUEST;
        send(_fd, &alive, 1, 0);
    }
    return result;
}

/* Returns number of bytes available in receive buffer or -1 on error
*/
int NetSioPort::available()
{
    if (!_byte_pending)
        handle_netsio();
    return _byte_pending ? 1 : 0;
}

/* Changes baud rate
*/
void NetSioPort::set_baudrate(uint32_t baud)
{
    Debug_printf("set_baudrate: %d\n", baud);

    if (!_initialized)
        return;

    unsigned int cpb = 1789773 / baud;
    uint8_t txbuf[3];
    txbuf[0] = NETSIO_SPEED_CHANGE;
    txbuf[1] = cpb & 0xff;
    txbuf[2] = cpb >> 8;
    int result = send(_fd, txbuf, sizeof(txbuf), 0);
    _baud = baud;
}

uint32_t NetSioPort::get_baudrate()
{
    return _baud;
}

bool NetSioPort::is_command(void)
{
    int status;
    struct timeval tv;

    if (! _initialized)
    {
        // is serial port suspended ?
        if (_suspend_time != 0)
        {
            gettimeofday(&tv, NULL);
            if (_suspend_time > tv.tv_sec)
                return false;
            // try to re-open serial port
            begin(_baud); // TODO current speed
        }
        if (! _initialized)
            return false;
    }

    // process NetSIO message, if any
    handle_netsio();

    gettimeofday(&tv, NULL);
    if (_expire_time <= tv.tv_sec)
    {
        Debug_println("NetSIO connection lost");
        // try to re-open serial port
        end();
        // suspend();
        begin(_baud); // TODO current speed
    }

    return _command_asserted;
}

void NetSioPort::set_proceed_line(bool level)
{
    static int last_level = -1; // 0,1 or -1 for unknown
    int new_level = level ? 0 : 1;
    int result;

    if (!_initialized)
        return;
    if (last_level == new_level)
        return;

    Debug_print(level ? "+" : "-");
    last_level = new_level;

    if (!waitWritable(500))
    {
        Debug_println("### NetSIO set_proceed_line() TIMEOUT ###");
    }
    uint8_t cmd = level ? NETSIO_PROCEED_ON : NETSIO_PROCEED_OFF;
    send(_fd, &cmd, 1, 0);
}

timeval NetSioPort::timeval_from_ms(const uint32_t millis)
{
  timeval tv;
  tv.tv_sec = millis / 1000;
  tv.tv_usec = (millis - (tv.tv_sec * 1000)) * 1000;
  return tv;
}

bool NetSioPort::waitReadable(uint32_t timeout_ms)
{
    timeval timeout_tv;
    fd_set readfds;
    int result;

    while (!_byte_pending)
    {
        // Setup a select call to block for socket data or a timeout
        timeout_tv = timeval_from_ms(timeout_ms);
        FD_ZERO(&readfds);
        FD_SET(_fd, &readfds);
        int result = select(_fd + 1, &readfds, nullptr, nullptr, &timeout_tv);

        if (result < 0)
        {
            if (errno == EINTR) {
                // TODO adjust timeout_tv
                continue;
            }
            perror("waitReadable - select error");
            return false;
        }
        // Timeout occurred
        if (result == 0)
        {
            return false;
        }
        // This shouldn't happen, if result > 0 our fd has to be in the list!
        if (!FD_ISSET (_fd, &readfds)) 
        {
            Debug_println("waitReadable - unexpected select result");
        }

        handle_netsio();
    }

    // Data available to read.
    return true;
}

/* Returns a single byte from the incoming stream
*/
int NetSioPort::read(void)
{
    if (!waitReadable(500))
    {
        Debug_println("### NetSIO read() TIMEOUT ###");
        return -1;
    }
    _byte_pending = false;
    return _byte;
}

/* Since the underlying Stream calls this Read() multiple times to get more than one
*  character for ReadBytes(), we override with a single call to uart_read_bytes
*/
size_t NetSioPort::read(uint8_t *buffer, size_t length, bool command_mode)
{
    if (!_initialized)
        return 0;

    int result;
    int rxbytes;
    for (rxbytes=0; rxbytes<length;)
    {
        result = read();
        // Debug_printf("read: %d\n", result);
        if (result < 0)
        {
            // if (errno == EAGAIN)
            // {
            //     result = 0;
            // }
            // else
            {
                Debug_printf("### NetSIO read() ERROR %d %s ###\n", errno, strerror(errno));
                break;
            }
        }
        else
        {
            buffer[rxbytes++] = (uint8_t)result;
        }

        if (rxbytes == length)
        {
            // done
            break;
        }

        // wait for more data
        if (command_mode && !is_command())
        {
            Debug_println("### NetSIO read()) CMD pin deasserted while reading command ###");
            return 1 + length; // indicate to SIO caller
        }
        // if (!waitReadable(500)) // 500 ms timeout
        // {
        //     Debug_println("### UART readBytes() TIMEOUT ###");
        //     break;
        // }
    }
    return rxbytes;
}

bool NetSioPort::waitWritable(uint32_t timeout_ms)
{
    timeval timeout_tv;
    fd_set writefds;
    int result;
    for(;;)
    {
        timeout_tv = timeval_from_ms(timeout_ms);
        FD_ZERO(&writefds);
        FD_SET(_fd, &writefds);
        result = select(_fd + 1, nullptr, &writefds, nullptr, &timeout_tv);
        if (result < 0) 
        {
            if (errno == EINTR) {
                // TODO adjust timeout_tv
                continue;
            }
            perror("waitWritable - select error");
            return false;
        }
        // Timeout occurred
        if (result == 0)
        {
            return false;
        }
        // This shouldn't happen, if result > 0 our fd has to be in the list!
        if (!FD_ISSET(_fd, &writefds)) 
        {
            Debug_println("waitWritable - unexpected select result");
            return false;
        }
        break;
    }
    return true;
}

/* write single byte via NetSIO */
ssize_t NetSioPort::write(uint8_t c)
{
    uint8_t txbuf[2];
    txbuf[0] = NETSIO_DATA_BYTE; // byte command
    txbuf[1] = c;                // value

    if (!_initialized)
        return 0;

    if (!waitWritable(500))
    {
        Debug_println("### NetSIO write() TIMEOUT ###");
        return 0;
    }
    ssize_t result = send(_fd, txbuf, 2, 0);
    return (result > 0) ? result-1 : 0; // amount of data bytes sent
}

ssize_t NetSioPort::write(const uint8_t *buffer, size_t size)
{
    int result;
    int to_send;
    int txbytes = 0;
    uint8_t txbuf[513];
    bool push = false;

    if (!_initialized)
        return 0;

    while (txbytes < size || push)
    {
        if (!waitWritable(500)) {
            Debug_println("### NetSIO write() TIMEOUT ###");
            break;
        }
        if (push) {
            // push SIO buffer to POKEY
            push = false;
            txbuf[0] = NETSIO_TRANSMITT_BUFFER; // push buffer
            result = send(_fd, txbuf, 1, 0);
        }
        else {
            // fill SIO buffer
            to_send = ((size-txbytes) > sizeof(txbuf)-1) ? sizeof(txbuf)-1 : (size-txbytes);
            txbuf[0] = NETSIO_FILL_BUFFER; // fill buffer
            memcpy(txbuf+1, buffer+txbytes, to_send);
            result = send(_fd, txbuf, to_send+1, 0);
            if (result > 0) {
                txbytes += result-1;
                push = true;
            }
        }
        if (result < 1) {
            Debug_printf("### NetSIO write() ERROR %d ###\n", errno);
            break;
        }
    }
    return txbytes;
}

// specific to NetSioPort
void NetSioPort::set_host(const char *host, int port)
{
    if (host != nullptr)
        strlcpy(_host, host, sizeof(_host));
    else
        _host[0] = 0;

    _port = port;
}

const char* NetSioPort::get_host(int &port)
{
    port = _port;
    return _host;
}
