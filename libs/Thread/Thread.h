#pragma once
#include <exception>
#include <cstdlib>
#include <cerrno>
#include <string>
#include <cstring>
#include <assert.h>

class Thread {
private:
    pthread_t _tid;
   
    void *(*_func)(void *);
    void *_arg;
    bool _run;
    void error_check(int retv);


public:

    Thread(void *(*f)(void *), void *arg = NULL) : _func(f), _arg(arg), _run(false) {
    }


    Thread & operator=(const Thread& other) {
        if(this == &other){
            return *this;
        }
        _run = other._run;
        _func = other._func;
        _arg = other._arg;
        return *this;
    }

    Thread(const Thread& other) {
        assert(_run == false);
        _run = other._run;
        _func = other._func;
        _arg = other._arg;
    }

    ~Thread() {
    }

    void run();
    void detach();
    void join(void **ptr = NULL);

    static void exit(void *ptr = NULL) {
        ::pthread_exit(ptr);
    }
};

class ThreadException : std::exception {
    std::string _err;
public:

    ThreadException(const char *err_msg) : _err(err_msg) {
    }

    ~ThreadException() throw () {

    }

    const char *what() const throw () {
        return _err.c_str();
    }
};
