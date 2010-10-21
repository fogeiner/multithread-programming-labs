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
    pthread_cond_t _cvid;

    void error_check(int retv);

    CondVar & operator=(const CondVar& other) {
		assert(false);
		return *this;
    }

    CondVar(const CondVar& other) {
		assert(false);
    }
public:
    CondVar();
    ~CondVar();

    void signal();
    void broadcast();
    void wait(Mutex &m);
    void timedwait(Mutex &m, const struct timespec *abstime);
};
