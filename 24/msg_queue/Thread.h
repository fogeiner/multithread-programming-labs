#pragma once
#include <exception>
#include <cstdlib>
#include <cerrno>
#include <string>
#include <cstring>

class Thread {
private:
    pthread_t _tid;
    bool _needs_resource_release;
    bool _run;
    void *(*_func)(void *);
    void *_arg;
    void error_check(int retv);
public:

    Thread(void *(*f)(void *), void *arg) : _func(f), _arg(arg), _run(false), _needs_resource_release(false) {
    }
    Thread(const Thread& orig);
    ~Thread();
    void run();
    void detach();
    void join(void **ptr);
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