#include <cassert>
#include "Semaphore.h"

void Semaphore::error_check(int retv) {
    if (retv == -1) {
		char buf[256];
#if (_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600) && ! _GNU_SOURCE 
		char *msg_ptr;
		msg_ptr = ::strerror_r(errno, buf, sizeof(buf));
        throw SemaphoreException(errno, msg_ptr);
#else
		::strerror_r(errno, buf, sizeof(buf));
		throw SemaphoreException(errno, buf);
#endif
    }
}

Semaphore::Semaphore(const Semaphore & other) {
    assert(false);
}

Semaphore& Semaphore::operator=(const Semaphore & other) {
    assert(false);
    return *this;
}

Semaphore::Semaphore(int value, bool pshared) {
    error_check(sem_init(&_sid, pshared == false ? 0 : 1, value));
}

Semaphore::~Semaphore() {
    sem_destroy(&_sid);
}

int Semaphore::getvalue(){
    int val;
    error_check(sem_getvalue(&_sid, &val));
    return val;
}

void Semaphore::post() {
    error_check(sem_post(&_sid));
}

void Semaphore::wait() {
    error_check(sem_wait(&_sid));
}

void Semaphore::operator++() {
    this->post();
}

void Semaphore::operator++(int) {
    this->post();
}

void Semaphore::operator--() {
    this->wait();
}

void Semaphore::operator--(int) {
    this->wait();
}
