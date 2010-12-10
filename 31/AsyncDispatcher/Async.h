#pragma once

class Async {
public:
    virtual bool readable() const = 0;
    virtual bool writable() const = 0;
    virtual void handle_read() = 0;
    virtual void handle_write() = 0;
    virtual void handle_close() = 0;
    virtual void handle_accept() = 0;
    virtual void handle_connect() = 0;

    virtual ~Async() {
    }
};

