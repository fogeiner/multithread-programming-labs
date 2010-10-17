#pragma once
#include <pthread.h>
#include <cerrno>
#include <string>
#include <exception>
#include <cstring>

class MutexException : public std::exception {
private:
    std::string _err;

public:

    MutexException(const char *msg) : _err(msg) {
    }

    ~MutexException() throw () {
    }

    const char *what() const throw () {
        return _err.c_str();
    }
};

class Mutex {
    friend class CondVar;
private:
    int locks;
    pthread_mutex_t _mid;
    pthread_mutexattr_t _mattrid;

    void error_check(int retv);

    Mutex & operator=(const Mutex& other) {
    }

public:
    enum mutex_type {
        NORMAL_MUTEX = PTHREAD_MUTEX_NORMAL,
        RECURSIVE_MUTEX = PTHREAD_MUTEX_RECURSIVE,
        ERRORCHECK_MUTEX = PTHREAD_MUTEX_ERRORCHECK,
		DEFAULT_MUTEX = PTHREAD_MUTEX_DEFAULT
    };

    Mutex(enum mutex_type type = NORMAL_MUTEX);
	Mutex(const Mutex& other);
    ~Mutex();

    void lock();
    void unlock();
    bool trylock();

};
