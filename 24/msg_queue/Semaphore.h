#pragma once

#include <semaphore.h>
#include <cstring>
#include <string>
#include <cerrno>
#include <exception>
#include <cstdlib>

class Semaphore {
private:
    sem_t _sid;
    void error_check(int retv);

    Semaphore(const Semaphore &other);
    Semaphore & operator=(const Semaphore &other);
public:

    Semaphore(int value, bool pshared = false);
    ~Semaphore();

    void post();
    void wait();

    Semaphore & operator++();
    Semaphore & operator++(int);
    Semaphore & operator--();
    Semaphore & operator--(int);
};

class SemaphoreException : public std::exception {
private:

    std::string _err;
public:

    SemaphoreException(char *err_msg) : _err(err_msg) {
    }

    ~SemaphoreException() throw () {

    }

    const char *what() const throw () {
        return _err.c_str();
    }
};
