#include <string.h>
#include <cstdarg>
// #include <esp_system.h>
// #include <driver/uart.h>

// C library headers
#include <stdio.h>
#include <string.h>

// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
// #include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()
#include <termio.h> // TIOCM_DSR etc.
#include <linux/serial.h>

#include "fnUART.h"
#include "../../include/debug.h"

// #define UART_DEBUG UART_NUM_0
// #define UART_SIO   UART_NUM_2
#define SIO_PROBE_DEV1  "/dev/ttyUSB0"
#define SIO_PROBE_DEV2  "/dev/ttyS0"

// Number of RTOS ticks to wait for data in TX buffer to complete sending
#define MAX_FLUSH_WAIT_TICKS 200
#define MAX_READ_WAIT_TICKS 200
#define MAX_WRITE_BYTE_TICKS 100
#define MAX_WRITE_BUFFER_TICKS 1000

#define UART0_RX 3
#define UART0_TX 1
#define UART1_RX 9
#define UART1_TX 10
#define UART2_RX 33
#define UART2_TX 21

// UARTManager fnUartDebug(UART_DEBUG);
UARTManager fnUartSIO;

// Constructor
// UARTManager::UARTManager(uart_port_t uart_num) : _uart_num(uart_num), _uart_q(NULL) {}
UARTManager::UARTManager() : _device(nullptr) {}

void UARTManager::end()
{
    // uart_driver_delete(_uart_num);
    // if(_uart_q)
    //     free(_uart_q);
    // _uart_q = NULL;

    close(_fd);
    _fd  = -1;
}

void UARTManager::begin(int baud)
{
    // if(_uart_q)
    // {
    //     end();
    // }

    // uart_config_t uart_config = 
    // {
    //     .baud_rate = baud,
    //     .data_bits = UART_DATA_8_BITS,
    //     .parity = UART_PARITY_DISABLE,
    //     .stop_bits = UART_STOP_BITS_1,
    //     .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    //     .rx_flow_ctrl_thresh = 122, // No idea what this is for, but shouldn't matter if flow ctrl is disabled?
    //     .use_ref_tick = false // ?
    // };
    // uart_param_config(_uart_num, &uart_config);

    // int tx, rx;
    // if(_uart_num == 0)
    // {
    //     rx = UART0_RX;
    //     tx = UART0_TX;
    // }
    // else if(_uart_num == 1)
    // {
    //     rx = UART1_RX;
    //     tx = UART1_TX;
    // }
    // else if (_uart_num == 2)
    // {
    //     rx = UART2_RX;
    //     tx = UART2_TX;
    // } else {
    //     return;
    // }

    // uart_set_pin(_uart_num, tx, rx, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    // // Arduino default buffer size is 256
    // int uart_buffer_size = 256;
    // int uart_queue_size = 10;
    // int intr_alloc_flags = 0;

    // // Install UART driver using an event queue here
    // //uart_driver_install(_uart_num, uart_buffer_size, uart_buffer_size, uart_queue_size, &_uart_q, intr_alloc_flags);
    // uart_driver_install(_uart_num, uart_buffer_size, 0, uart_queue_size, NULL, intr_alloc_flags);

    // Open the serial port
    if (_device == nullptr)
    {
        // Probe some serial ports
        Debug_println("Trying " SIO_PROBE_DEV1);
        if ((_fd = open(SIO_PROBE_DEV1, O_RDWR)) >= 0)
        {
            _device = SIO_PROBE_DEV1;
        }
        else
        {
            Debug_println("Trying " SIO_PROBE_DEV2);
            if ((_fd = open(SIO_PROBE_DEV2, O_RDWR)) >= 0)
            {
                _device = SIO_PROBE_DEV2;
            }
        }

        // successfull probe
        if (_device != nullptr)
            Debug_printf("Setting up serial port (%s)\n", _device);
    }
    else
    {
        Debug_printf("Setting up serial port (%s)\n", _device);
        _fd = open(_device, O_RDWR);
    }

    if (_fd < 0)
    {
		perror("Failed to open serial device");
		// return;
        exit(1);
	}
    

    // Create new termios struct
    struct termios tios;

    // Read in existing settings
    if(tcgetattr(_fd, &tios) != 0)
    {
        perror("Failed to get termios structure");
		// return;
        exit(1);
    }

    tios.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
    tios.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
    tios.c_cflag &= ~CSIZE; // Clear all bits that set the data size 
    tios.c_cflag |= CS8; // 8 bits per byte (most common)
    tios.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
    tios.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)
  
    tios.c_lflag &= ~ICANON;
    tios.c_lflag &= ~ECHO; // Disable echo
    tios.c_lflag &= ~ECHOE; // Disable erasure
    tios.c_lflag &= ~ECHONL; // Disable new-line echo
    tios.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
    tios.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    tios.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes
  
    tios.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    tios.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
    // tios.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
    // tios.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)
  
    tios.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    tios.c_cc[VMIN] = 0;
  
    // Set in/out baud rate to be 19200
    // if (cfsetspeed(&tios, B19200) != 0)
    if (cfsetspeed(&tios, 19200) != 0)
    // if (cfsetispeed(&tios, baud) != 0 || cfsetospeed(&tios, baud) != 0)
    {
        perror("Failed to set I/O baud rate");
        return;
    }
  
    // Apply settings
    if (tcsetattr(_fd, TCSANOW, &tios) != 0)
    {
        perror("Failed to set serial attributes");
		return;
    }

    Debug_printf("### UART initialized ###\n");
    // Set initialized.
    _initialized=true;
}

/* Discards anything in the input buffer
*/
void UARTManager::flush_input()
{        
    // uart_flush_input(_uart_num);
    tcflush(_fd, TCIFLUSH);
}

/* Clears input buffer and flushes out transmit buffer waiting at most
   waiting MAX_FLUSH_WAIT_TICKS until all sends are completed
*/
void UARTManager::flush()
{
    // uart_wait_tx_done(_uart_num, MAX_FLUSH_WAIT_TICKS);
    tcdrain(_fd);
}

/* Returns number of bytes available in receive buffer or -1 on error
*/
int UARTManager::available()
{
    // size_t result;
    // if(ESP_FAIL == uart_get_buffered_data_len(_uart_num, &result))
    //     return -1;
    // return result;
    int result;
	if (ioctl(_fd, FIONREAD, &result) < 0)
        return -1;
    return result;
}

/* NOT IMPLEMENTED
*/
int UARTManager::peek()
{
    return 0;
}

/* Changes baud rate
*/
void UARTManager::set_baudrate(uint32_t baud)
{
    termios tios;
    struct serial_struct ss;

    Debug_printf("set_baudrate: %d\n", baud);

    tcgetattr(_fd, &tios);
    tios.c_cflag &= ~CSTOPB;
    cfmakeraw(&tios);

    tios.c_cc[VTIME] = 10;
    tios.c_cc[VMIN] = 0;

    int baud_id = 0;

    switch (baud)
    {
        case 300:
            baud_id = B300;
            break;
        case 600:
            baud_id = B600;
            break;
        case 1200:
            baud_id = B1200;
            break;
        case 1800:
            baud_id = B1800;
            break;
        case 2400:
            baud_id = B2400;
            break;
        case 4800:
            baud_id = B4800;
            break;
        case 9600:
            baud_id = B9600;
            break;
        case 19200:
            baud_id = B19200;
            break;
        case 38400:
            baud_id = B38400;
            break;
        case 57600:
            baud_id = B57600;
            break;
        case 115200:
            baud_id = B115200;
            break;
    }

    // set speed
    if (baud_id != 0)
    {
        // standard speeds
        if (baud_id == B38400)
        {
            // reset special handling of B38400 back to 38400
            ioctl(_fd, TIOCGSERIAL, &ss);
            ss.flags &= ~ASYNC_SPD_MASK;
            ioctl(_fd, TIOCSSERIAL, &ss);
        }
    }
    else
    {
        // configure B38400 to custom speed
        ioctl(_fd, TIOCGSERIAL, &ss);
        ss.flags = (ss.flags & ~ASYNC_SPD_MASK) | ASYNC_SPD_CUST;
        ss.custom_divisor = (ss.baud_base + (baud / 2)) / baud;
        int custom = ss.baud_base / ss.custom_divisor;

        if (custom < baud * 98 / 100 || custom > baud * 102 / 100)
            Debug_printf("Cannot set serial port speed to %d: Closest possible speed is %d\n", baud, custom);

        ioctl(_fd, TIOCSSERIAL, &ss);
        baud_id == B38400;
    }

    if ((cfsetispeed(&tios, baud_id) | cfsetospeed(&tios, baud_id)) != 0)
        perror("Failed to set baud rate");

    // Apply settings
    if (tcsetattr(_fd, TCSANOW, &tios) != 0)
        perror("Failed to set serial attributes");
}

bool UARTManager::is_command(void)
{
    int status;
    int mask = TIOCM_DSR;
    if (ioctl(_fd, TIOCMGET, &status) < 0) 
    {
        perror("Cannot retrieve serial port status");
        return false;
    }
    return ((status & mask) != 0);
}

/* Returns a single byte from the incoming stream
*/
int UARTManager::read(void)
{
    uint8_t byte;
    // int result = uart_read_bytes(_uart_num, &byte, 1, MAX_READ_WAIT_TICKS);
    int result = ::read(_fd, &byte, 1);
    if(result < 1)
    {
#ifdef DEBUG
        if(result == 0)
            Debug_println("### UART read() TIMEOUT ###");
        else
            Debug_printf("### UART read() ERROR %d ###\n", result);
#endif        
        return -1;
    } else
        return byte;
}

/* Since the underlying Stream calls this Read() multiple times to get more than one
*  character for ReadBytes(), we override with a single call to uart_read_bytes
*/
size_t UARTManager::readBytes(uint8_t *buffer, size_t length)
{
    int result;
    int rxbytes;
    for (rxbytes=0; rxbytes<length;)
    {
        result = ::read(_fd, &buffer[rxbytes], length-rxbytes);
        // Debug_printf("read: %d\n", result);
        if (result < 0 && errno != EAGAIN)
        {
            Debug_printf("### UART readBytes() ERROR %d %s ###\n", errno, strerror(errno));
            break;
        }
        if (result == 0)
        {
            Debug_println("### UART readBytes() TIMEOUT ###");
            break;
        }
        rxbytes += result;
    }
    
    return rxbytes;
}

size_t UARTManager::write(uint8_t c)
{
    // int z = uart_write_bytes(_uart_num, (const char *)&c, 1);
    // //uart_wait_tx_done(_uart_num, MAX_WRITE_BYTE_TICKS);
    // return z;
    return write(&c, 1);
}

size_t UARTManager::write(const uint8_t *buffer, size_t size)
{
    // int z = uart_write_bytes(_uart_num, (const char *)buffer, size);
    // //uart_wait_tx_done(_uart_num, MAX_WRITE_BUFFER_TICKS);
    // return z;
    int result;
    int txbytes;
    for (txbytes=0; txbytes<size;)
    {
        result = ::write(_fd, &buffer[txbytes], size-txbytes);
        // Debug_printf("write: %d\n", result);
        if (result < 0 && errno != EAGAIN)
        {
            Debug_printf("### UART write() ERROR %d ###\n", errno);
            break;
        }
        txbytes += result;
    }
    tcdrain(_fd);
    return txbytes;
}

size_t UARTManager::write(const char *str)
{
    // int z = uart_write_bytes(_uart_num, str, strlen(str));
    // return z;
    return write((const uint8_t *)str, strlen(str));
}

/*
size_t UARTManager::printf(const char * fmt...)
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

size_t UARTManager::_print_number(unsigned long n, uint8_t base)
{
    char buf[8 * sizeof(long) + 1]; // Assumes 8-bit chars plus zero byte.
    char *str = &buf[sizeof(buf) - 1];

    if (!_initialized)
        return -1;

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

size_t UARTManager::print(const char *str)
{
//     int z = strlen(str);

    if (!_initialized)
        return -1;

//     return uart_write_bytes(_uart_num, str, z);;
    return write(str);
}

size_t UARTManager::print(std::string str)
{
    if (!_initialized)
        return -1;

    return print(str.c_str());
}

size_t UARTManager::print(int n, int base)
{
    if (!_initialized)
        return -1;

    return print((long) n, base);
}

size_t UARTManager::print(unsigned int n, int base)
{
    if (!_initialized)
        return -1;

    return print((unsigned long) n, base);
}

size_t UARTManager::print(long n, int base)
{
    if (!_initialized)
        return -1;

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

size_t UARTManager::print(unsigned long n, int base)
{
    if (!_initialized)
        return -1;

    if(base == 0) {
        return write(n);
    } else {
        return _print_number(n, base);
    }
}

/*
size_t UARTManager::println(const char *str)
{
    if (!_initialized)
        return -1;

    size_t n = print(str);
    n += println();
    return n;
}

size_t UARTManager::println(std::string str)
{
    if (!_initialized)
        return -1;

    size_t n = print(str);
    n += println();
    return n;
}

size_t UARTManager::println(int num, int base)
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
