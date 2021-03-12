/* Basically a simplified copy of the ESP Arduino library in HardwareSerial.h/HardwareSerial.cpp
*/
#ifndef FNUART_H
#define FNUART_H

#include <string>
// #include <driver/uart.h>

class UARTManager
{
private:
    // uart_port_t _uart_num;
    char _device[64]; // device name or path
    int _command_pin;
    int _proceed_pin;
    int _command_tiocm;
    int _proceed_tiocm;
    int _fd;
    // QueueHandle_t _uart_q;
    bool _initialized; // is UART ready?

    // serial port error counter
    int _errcount;
    unsigned long _suspend_time;

    size_t _print_number(unsigned long n, uint8_t base);

public:
    // UARTManager(int uart_num);
    UARTManager(void);

    void begin(int baud);
    void end();
    void suspend(int sec=5);
    void set_baudrate(uint32_t baud);
    bool initialized() { return _initialized; }

    void set_port(const char *device, int command_pin, int proceed_pin);
    const char* get_port(int &command_pin, int &proceed_pin);
    bool is_command();
    void set_proceed_line(bool level=true, bool force=false);

    int available();
    int peek();
    void flush();
    void flush_input();

    bool waitReadable(uint32_t timeout_ms);

    int read();
    size_t readBytes(uint8_t *buffer, size_t length, bool command_mode=false);
    size_t readBytes(char *buffer, size_t length) { return readBytes((uint8_t *)buffer, length); };

    size_t write(uint8_t);
    size_t write(const uint8_t *buffer, size_t size);
    size_t write(const char *s);

    size_t write(unsigned long n) { return write((uint8_t)n); };
    size_t write(long n) { return write((uint8_t)n); };
    size_t write(unsigned int n) { return write((uint8_t)n); };
    size_t write(int n) { return write((uint8_t)n); };

    // size_t printf(const char *format, ...);

    // //size_t println(const char *format, ...);
    // size_t println(const char *str);
    // size_t println() { return print("\n"); };
    // size_t println(std::string str);
    // size_t println(int num, int base = 10);

    // //size_t print(const char *format, ...);
    size_t print(const char *str);
    size_t print(std::string str);
    size_t print(int n, int base = 10);
    size_t print(unsigned int n, int base = 10);
    size_t print(long n, int base = 10);
    size_t print(unsigned long n, int base = 10);
};

// extern UARTManager fnUartDebug;
extern UARTManager fnUartSIO;

#endif //FNUART_H
