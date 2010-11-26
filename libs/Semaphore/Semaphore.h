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

    Semaphore(int value = 0, bool pshared = false);
    ~Semaphore();

    void post();
    void wait();
    int getvalue();
    Semaphore & operator++();
    Semaphore & operator++(int);
    Semaphore & operator--();
    Semaphore & operator--(int);
};

class SemaphoreException : public std::exception {
private:

    std::string _err;
	int _err_num;
public:

    SemaphoreException(int err_num, char *err_msg) : _err_num(err_num), _err(err_msg) {
    }

    ~SemaphoreException() throw () {

    }
	
	const int err_num() const {
		return _err_num;
	}

    const char *what() const throw () {
        return _err.c_str();
    }
};
