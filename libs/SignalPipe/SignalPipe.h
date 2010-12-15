#pragma once
#include <exception>
#include <string>

#include "../TCPSocket/Selectable.h"

class SignalPipeException : public std::exception {
    std::string _err;
public:

    SignalPipeException(int error) {
        char buf[256];
#if (_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600) && ! _GNU_SOURCE || defined ( sun ) || defined ( __sun )
        ::strerror_r(error, buf, sizeof (buf));
        _err.assign(buf);
#else
        char *msg_ptr;
        msg_ptr = ::strerror_r(error, buf, sizeof (buf));
        _err.assign(msg_ptr);
#endif

    }

    const char *what() const throw () {
        return this->_err.c_str();
    }

    ~SignalPipeException() throw () {
    }
};

class SignalPipe : public Selectable {
    int _fds[2];
public:

    SignalPipe() {
        if(pipe(_fds) == -1){
            throw SignalPipeException(errno);
        }
    }
    
    ~SignalPipe(){
        for(int i = 0; i < 2; ++i){
            close(_fds[i]);
        }
    }

    int fileno() const {
        return _fds[0];
    }

    void signal(){
        if(write(_fds[1], "", 1) == -1){
            throw SignalPipeException(errno);
        }
    }

    void reset_signal(){
        const int BUF_SIZE = 256;
        char buf[BUF_SIZE];
        if(read(_fds[0], buf, BUF_SIZE) == -1){
            throw SignalPipeException(errno);
        }
    }
};
