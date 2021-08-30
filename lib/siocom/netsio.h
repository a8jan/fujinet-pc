#ifndef NETSIO_H
#define NETSIO_H

#include "sioport.h"
#include "fnDNS.h"

class NetSioPort : public SioPort
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

protected:
    void suspend(int sec=5);
    int handle_netsio();
    static timeval timeval_from_ms(const uint32_t millis);
    bool waitReadable(uint32_t timeout_ms);
    bool waitWritable(uint32_t timeout_ms);

public:
    NetSioPort();
    virtual ~NetSioPort();
    virtual void begin(int baud) override;
    virtual void end() override;

    virtual void set_baudrate(uint32_t baud) override;
    virtual uint32_t get_baudrate() override;

    virtual bool is_command() override;
    virtual void set_proceed_line(bool level) override;

    virtual int available() override;
    virtual void flush() override;
    virtual void flush_input() override;

    // read single byte
    virtual int read() override;
    // read bytes into buffer
    virtual size_t read(uint8_t *buffer, size_t length, bool command_mode=false) override;

    // write single byte
    virtual ssize_t write(uint8_t b) override;
    // write buffer
    virtual ssize_t write(const uint8_t *buffer, size_t size) override;

    // specific to NetSioPort
    void set_host(const char *host, int port);
    const char* get_host(int &port);
};

#endif // NETSIO_H
