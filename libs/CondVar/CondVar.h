#pragma once
#include "../Mutex/Mutex.h"
#include <pthread.h>
#include <exception>
#include <cerrno>
#include <cstring>
#include <string>

class CondVarException : public std::exception {
private:
    std::string _err;
public:

    CondVarException(const char *msg) : _err(msg) {
    }

    ~CondVarException() throw () {
    }

    const char *what() const throw () {
        return _err.c_str();
    }
};

class CondVar {
private:
    pthread_mutex_t _mid;
    pthread_cond_t _cvid;
    pthread_condattr_t _caid;

    void error_check(int retv);

    CondVar & operator=(const CondVar& other) {
    }

    CondVar(const CondVar& other) {
    }
    // TODO: replace mutex with Mutex
public:
    CondVar();
    ~CondVar();

    void signal();
    void broadcast();
    void wait();
    void timedwait(const struct timespec *abstime);
};