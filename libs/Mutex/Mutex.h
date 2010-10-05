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

    enum mutex_type {
        PTHREAD_FAST_MUTEX = ::PTHREAD_MUTEX_FAST_NP,
        PTHREAD_RECURSIVE_MUTEX = ::PTHREAD_MUTEX_RECURSIVE_NP,
        PTHREAD_ERRORCHECK_MUTEX = ::PTHREAD_MUTEX_ERRORCHECK_NP
    };

    void error_check(int retv);

    Mutex & operator=(const Mutex& other) {
    }

public:
    static const enum mutex_type FAST_MUTEX = PTHREAD_FAST_MUTEX;
    static const enum mutex_type RECURSIVE_MUTEX = PTHREAD_RECURSIVE_MUTEX;
    static const enum mutex_type ERRORCHECK_MUTEX = PTHREAD_ERRORCHECK_MUTEX;

    Mutex(enum mutex_type type = FAST_MUTEX);
	Mutex(const Mutex& other);
    ~Mutex();

    void lock();
    void unlock();
    bool trylock();

};
