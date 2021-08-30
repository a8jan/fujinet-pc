#ifndef FNNETSIO_H
#define FNNETSIO_H

#include <string>
#include "fnDNS.h"

class NetSIOManager
{
private:
    char _host[64];
    in_addr_t _ip;
    uint16_t _port;

    uint32_t _baud;
    int _command_pin;
    int _proceed_pin;
    int _command_tiocm;
    int _proceed_tiocm;
    int _fd;
    bool _initialized; // is UART ready?
    bool _command_asserted;
    bool _byte_pending;
    uint8_t _byte;

    // serial port error counter
    int _errcount;
    unsigned long _suspend_time;
    unsigned long _alive_time;
    unsigned long _expire_time;

    size_t _print_number(unsigned long n, uint8_t base);

public:
    NetSIOManager(void);

    void begin(int baud);
    void end();
    void suspend(int sec=5);
    void set_baudrate(uint32_t baud);
    bool initialized() { return _initialized; }

    void set_port(const char *device, int command_pin, int proceed_pin);
    const char* get_port(int &command_pin, int &proceed_pin);
    bool is_command();
    void set_proceed_line(bool level=true, bool force=false);

    int handle_netsio();
    int available();
    int peek();
    void flush();
    void flush_input();

    bool waitReadable(uint32_t timeout_ms);
    bool waitWritable(uint32_t timeout_ms);

    int read();
    size_t readBytes(uint8_t *buffer, size_t length, bool command_mode=false);
    size_t readBytes(char *buffer, size_t length) { return readBytes((uint8_t *)buffer, length); };

    ssize_t write(uint8_t);
    ssize_t write(const uint8_t *buffer, size_t size);
    ssize_t write(const char *s);

    size_t write(unsigned long n) { return write((uint8_t)n); };
    size_t write(long n) { return write((uint8_t)n); };
    size_t write(unsigned int n) { return write((uint8_t)n); };
    size_t write(int n) { return write((uint8_t)n); };

    size_t print(const char *str);
    size_t print(std::string str);
    size_t print(int n, int base = 10);
    size_t print(unsigned int n, int base = 10);
    size_t print(long n, int base = 10);
    size_t print(unsigned long n, int base = 10);
};

extern NetSIOManager fnUartSIO;

#endif //FNNETSIO_H
