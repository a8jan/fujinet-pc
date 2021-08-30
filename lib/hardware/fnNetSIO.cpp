#include <string.h>
#include <cstdarg>

#include <stdio.h>
#include <string.h>
#include "config.h"
#ifdef HAVE_BSD_STRING_H
#include <bsd/string.h>
#endif
#include <sys/time.h>
#include <unistd.h> // write(), read(), close()
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <fcntl.h> // Contains file controls like O_RDWR
#include <sys/ioctl.h> // TIOCM_DSR etc.

#include "fnNetSIO.h"
#include "../../include/debug.h"
// #include "../../include/pinmap.h"

// Number of RTOS ticks to wait for data in TX buffer to complete sending
#define MAX_FLUSH_WAIT_TICKS 200
#define MAX_READ_WAIT_TICKS 200
#define MAX_WRITE_BYTE_TICKS 100
#define MAX_WRITE_BUFFER_TICKS 1000

NetSIOManager fnUartSIO;

// Constructor
NetSIOManager::NetSIOManager() : 
    _initialized(false),
    _fd(-1),
    _ip(IPADDR_NONE),
    _port(6508),
    _command_asserted(false),
    _byte_pending(false)
{};

void NetSIOManager::end()
{
    if (_fd >= 0)
    {
        close(_fd);
        _fd  = -1;
    }
    _initialized = false;
}

void NetSIOManager::set_port(const char *device, int command_pin, int proceed_pin)
{
    if (device != nullptr)
        strlcpy(_host, device, sizeof(_host));
    else
        _host[0] = 0;
    _command_pin = command_pin;
    _proceed_pin = proceed_pin;
    switch (_command_pin)
    {
// TODO move from fnConfig.h to fnUART.h
// enum serial_command_pin
// {
//     SERIAL_COMMAND_NONE = 0,
//     SERIAL_COMMAND_DSR,
//     SERIAL_COMMAND_CTS,
//     SERIAL_COMMAND_RI,
//     SERIAL_COMMAND_INVALID
// };
    case 1: // SERIAL_COMMAND_DSR
        _command_tiocm = TIOCM_DSR;
        break;
    case 2: // SERIAL_COMMAND_CTS
        _command_tiocm = TIOCM_CTS;
        break;
    case 3: // SERIAL_COMMAND_RI
        _command_tiocm = TIOCM_RI;
        break;
    default:
        _command_tiocm = 0;
    }

// TODO move from fnConfig.h to fnUART.h
// enum serial_proceed_pin
// {
//     SERIAL_PROCEED_NONE = 0,
//     SERIAL_PROCEED_DTR,
//     SERIAL_PROCEED_RTS,
//     SERIAL_PROCEED_INVALID
// };
    switch (_proceed_pin)
    {
    case 1: // SERIAL_PROCEED_DTR
        _proceed_tiocm = TIOCM_DTR;
        break;
    case 2: // SERIAL_PROCEED_RTS
        _proceed_tiocm = TIOCM_RTS;
        break;
    }
}

const char* NetSIOManager::get_port(int &command_pin, int &proceed_pin)
{
    command_pin = _command_pin;
    proceed_pin = _proceed_pin;
    return _host;
}

void NetSIOManager::begin(int baud)
{
    // if(_uart_q)
    // {
    //     end();
    // }

    _errcount = 0;
    _suspend_time = 0;

    _command_asserted = false;
    _byte_pending = false;


    if(*_host != 0)
    {
        Debug_printf("Setting up NetSIO (%s)\n", _host);
        // TODO allow <host>:<port>
        _fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    }

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
		perror("Failed to connect NetSIO socket");
        suspend();
		return;
    }

    fcntl(_fd, F_SETFL, O_NONBLOCK);

    // ping
    // send(_fd, "\xC2", 1, 0);
    // recv(_fd, ...)

    // connect device
    send(_fd, "\xC1", 1, 0);

    struct timeval tv;
    gettimeofday(&tv, NULL);
    _alive_time = tv.tv_sec;
    _expire_time = _alive_time + 5;

    Debug_printf("### NetSIO initialized ###\n");
    // Set initialized.
    _initialized=true;
    _baud = 19200;
}


void NetSIOManager::suspend(int sec)
{
    Debug_println("Suspending serial port");
    struct timeval tv;
    gettimeofday(&tv, NULL);
    _suspend_time = tv.tv_sec + sec;
    end();
}

/* Discards anything in the input buffer
*/
void NetSIOManager::flush_input()
{
    if (_initialized)
        // tcflush(_fd, TCIFLUSH);
        _byte_pending = false;
}

/* Clears input buffer and flushes out transmit buffer waiting at most
   waiting MAX_FLUSH_WAIT_TICKS until all sends are completed
*/
void NetSIOManager::flush()
{
    if (_initialized)
        // tcdrain(_fd);
        _byte_pending = false;
}

/* read byte from socket and update internal variables */
int NetSIOManager::handle_netsio()
{
    uint8_t rxbuf[3];
    struct timeval tv;
    int result;

    result = recv(_fd, rxbuf, sizeof(rxbuf), 0);
    if (result > 0)
    {
        switch (rxbuf[0])
        {
            case 0x81: // data byte
                _byte_pending = true;
                _byte = rxbuf[1];
                break;
            case 0x82: // command off
                _command_asserted = false;
                break;
            case 0x83: // command on
                _command_asserted = true;
                break;
            case 0x84: // motor off
            case 0x85: // motor on
                break;
            case 0x8A: // speed change notification
                if (rxbuf[1] || rxbuf[2]) 
                {
                    unsigned int cpb = rxbuf[1] | (rxbuf[2] << 8);
                    set_baudrate(1789773 / cpb);
                }
                break;
            case 0xC5: // connection alive response
            {
                gettimeofday(&tv, NULL);
                _expire_time = tv.tv_sec + 5;
            }
            case 0xC3: // ping response
            default:
                break;
        }

    }
    gettimeofday(&tv, NULL);
    if (_alive_time + 2 <= tv.tv_sec)
    {
        _alive_time = tv.tv_sec;
        uint8_t alive = 0xC4;
        send(_fd, &alive, 1, 0);
    }
    return result;
}

/* Returns number of bytes available in receive buffer or -1 on error
*/
int NetSIOManager::available()
{
    if (!_byte_pending)
        handle_netsio();
    return _byte_pending ? 1 : 0;
}

/* NOT IMPLEMENTED
*/
int NetSIOManager::peek()
{
    return 0;
}

/* Changes baud rate
*/
void NetSIOManager::set_baudrate(uint32_t baud)
{
    Debug_printf("set_baudrate: %d\n", baud);

    if (!_initialized)
        return;

    unsigned int cpb = 1789773 / baud;
    uint8_t txbuf[3];
    txbuf[0] = 0x8A;
    txbuf[1] = cpb & 0xff;
    txbuf[2] = cpb >> 8;
    int result = send(_fd, txbuf, sizeof(txbuf), 0);
    _baud = baud;
}

bool NetSIOManager::is_command(void)
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
            begin(19200); // TODO current speed
        }
        if (! _initialized)
            return false;
    }

    // process NetSIO event, if any
    handle_netsio();

    gettimeofday(&tv, NULL);
    if (_expire_time <= tv.tv_sec)
    {
        Debug_println("NetSIO connection lost");
        // try to re-open serial port
        begin(19200); // TODO current speed
    }
    // if (ioctl(_fd, TIOCMGET, &status) < 0)
    // {
    //     // handle serial port errors
    //     _errcount++;
    //     if(_errcount == 1)
    //         perror("Cannot retrieve serial port status");
    //     else if (_errcount > 1000)
    //         suspend();
    //     return false;
    // }
    // _errcount = 0;

    return _command_asserted;
}

void NetSIOManager::set_proceed_line(bool level, bool force)
{
    int result;
    int mbit = _proceed_tiocm;
    static bool last_level = true;

    if (!_initialized)
        return;

    if (!force && last_level == level)
        return;
    
    last_level = level;
    Debug_print(level ? "+" : "-");
    if (!waitWritable(500))
    {
        Debug_println("### NetSIO set_proceed_line() TIMEOUT ###");
    }
    uint8_t cmd = level ? 0x87 : 0x86;
    send(_fd, &cmd, 1, 0);
}

timeval timeval_from_ms(const uint32_t millis)
{
  timeval tv;
  tv.tv_sec = millis / 1000;
  tv.tv_usec = (millis - (tv.tv_sec * 1000)) * 1000;
  return tv;
}

bool NetSIOManager::waitReadable(uint32_t timeout_ms)
{
    // TODO

    // // Setup a select call to block for serial data or a timeout
    // fd_set readfds;
    // FD_ZERO(&readfds);
    // FD_SET(_fd, &readfds);
    // timeval timeout_tv(timeval_from_ms(timeout_ms));

    // int result = select(_fd + 1, &readfds, nullptr, nullptr, &timeout_tv);

    // if (result < 0) 
    // {
    //     if (errno != EINTR) 
    //     {
    //         perror("waitReadable - select error");
    //     }
    //     return false;
    // }
    // // Timeout occurred
    // if (result == 0)
    // {
    //     return false;
    // }
    // // This shouldn't happen, if result > 0 our fd has to be in the list!
    // if (!FD_ISSET (_fd, &readfds)) 
    // {
    //     Debug_println("waitReadable - unexpected select result");
    // }

    while (!_byte_pending)
    {
        // TODO, implement timeout via select
        handle_netsio();
    }

    // Data available to read.
    return true;
}

/* Returns a single byte from the incoming stream
*/
int NetSIOManager::read(void)
{
    uint8_t byte;
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
size_t NetSIOManager::readBytes(uint8_t *buffer, size_t length, bool command_mode)
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
                Debug_printf("### NetSIO readBytes() ERROR %d %s ###\n", errno, strerror(errno));
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
            Debug_println("### UART readBytes() CMD pin deasserted while reading command ###");
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

bool NetSIOManager::waitWritable(uint32_t timeout_ms)
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
ssize_t NetSIOManager::write(uint8_t c)
{
    uint8_t txbuf[2];
    txbuf[0] = 0x81; // byte command
    txbuf[1] = c;    // value

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

ssize_t NetSIOManager::write(const uint8_t *buffer, size_t size)
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
            txbuf[0] = 0x91; // push buffer
            result = send(_fd, txbuf, 1, 0);
        }
        else {
            // fill SIO buffer
            to_send = ((size-txbytes) > sizeof(txbuf)-1) ? sizeof(txbuf)-1 : (size-txbytes);
            txbuf[0] = 0x92; // fill buffer
            memcpy(txbuf+1, buffer+txbytes, to_send);
            result = send(_fd, txbuf, to_send+1, 0);
            if (result > 0) {
                txbytes += result-1;
                push = true;
            }
        }
        if (result < 1) {
            Debug_printf("### UART write() ERROR %d ###\n", errno);
            break;
        }
    }
    return txbytes;
}

ssize_t NetSIOManager::write(const char *str)
{
    // int z = uart_write_bytes(_uart_num, str, strlen(str));
    // return z;
    return write((const uint8_t *)str, strlen(str));
}

/*
size_t NetSIOManager::printf(const char * fmt...)
{
    char * result = nullptr;
    va_list vargs;

    if (!_initialized)
        return -1;

    va_start(vargs, fmt);

    int z = vasprintf(&result, fmt, vargs);

    if(z > 0)
        uart_write_bytes(_uart_num, result, z);

    va_end(vargs);

    if(result != nullptr)
        free(result);

    //uart_wait_tx_done(_uart_num, MAX_WRITE_BUFFER_TICKS);

    return z >=0 ? z : 0;
}
*/

size_t NetSIOManager::_print_number(unsigned long n, uint8_t base)
{
    char buf[8 * sizeof(long) + 1]; // Assumes 8-bit chars plus zero byte.
    char *str = &buf[sizeof(buf) - 1];

    if (!_initialized)
        return 0;

    *str = '\0';

    // prevent crash if called with base == 1
    if(base < 2)
        base = 10;

    do {
        unsigned long m = n;
        n /= base;
        char c = m - base * n;
        *--str = c < 10 ? c + '0' : c + 'A' - 10;
    } while(n);

    return write(str);
}

size_t NetSIOManager::print(const char *str)
{
//     int z = strlen(str);

    if (!_initialized)
        return 0;

//     return uart_write_bytes(_uart_num, str, z);;
    return write(str);
}

size_t NetSIOManager::print(std::string str)
{
    if (!_initialized)
        return 0;

    return print(str.c_str());
}

size_t NetSIOManager::print(int n, int base)
{
    if (!_initialized)
        return 0;

    return print((long) n, base);
}

size_t NetSIOManager::print(unsigned int n, int base)
{
    if (!_initialized)
        return 0;

    return print((unsigned long) n, base);
}

size_t NetSIOManager::print(long n, int base)
{
    if (!_initialized)
        return 0;

    if(base == 0) {
        return write(n);
    } else if(base == 10) {
        if(n < 0) {
            // int t = print('-');
            int t = print("-");
            n = -n;
            return _print_number(n, 10) + t;
        }
        return _print_number(n, 10);
    } else {
        return _print_number(n, base);
    }
}

size_t NetSIOManager::print(unsigned long n, int base)
{
    if (!_initialized)
        return 0;

    if(base == 0) {
        return write(n);
    } else {
        return _print_number(n, base);
    }
}

/*
size_t NetSIOManager::println(const char *str)
{
    if (!_initialized)
        return -1;

    size_t n = print(str);
    n += println();
    return n;
}

size_t NetSIOManager::println(std::string str)
{
    if (!_initialized)
        return -1;

    size_t n = print(str);
    n += println();
    return n;
}

size_t NetSIOManager::println(int num, int base)
{
    if (!_initialized)
        return -1;

    size_t n = print(num, base);
    n += println();
    return n;
}
*/


/*

size_t Print::print(const __FlashStringHelper *ifsh)
{
    return print(reinterpret_cast<const char *>(ifsh));
}


size_t Print::print(char c)
{
    return write(c);
}

size_t Print::print(unsigned char b, int base)
{
    return print((unsigned long) b, base);
}


size_t Print::print(double n, int digits)
{
    return printFloat(n, digits);
}

size_t Print::println(const __FlashStringHelper *ifsh)
{
    size_t n = print(ifsh);
    n += println();
    return n;
}

size_t Print::print(const Printable& x)
{
    return x.printTo(*this);
}

size_t Print::print(struct tm * timeinfo, const char * format)
{
    const char * f = format;
    if(!f){
        f = "%c";
    }
    char buf[64];
    size_t written = strftime(buf, 64, f, timeinfo);
    if(written == 0){
        return written;
    }
    return print(buf);
}

size_t Print::println(char c)
{
    size_t n = print(c);
    n += println();
    return n;
}

size_t Print::println(unsigned char b, int base)
{
    size_t n = print(b, base);
    n += println();
    return n;
}

size_t Print::println(unsigned int num, int base)
{
    size_t n = print(num, base);
    n += println();
    return n;
}

size_t Print::println(long num, int base)
{
    size_t n = print(num, base);
    n += println();
    return n;
}

size_t Print::println(unsigned long num, int base)
{
    size_t n = print(num, base);
    n += println();
    return n;
}

size_t Print::println(double num, int digits)
{
    size_t n = print(num, digits);
    n += println();
    return n;
}

size_t Print::println(const Printable& x)
{
    size_t n = print(x);
    n += println();
    return n;
}

size_t Print::println(struct tm * timeinfo, const char * format)
{
    size_t n = print(timeinfo, format);
    n += println();
    return n;
}
*/
