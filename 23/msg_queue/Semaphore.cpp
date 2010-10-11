#include "Semaphore.h"

void Semaphore::error_check(int retv) {
    char buf[256];
    ::strerror_r(errno, buf, sizeof(buf));
    if (retv == -1) {
        throw SemaphoreException(buf);
    }
}

Semaphore::Semaphore(const Semaphore & other) {
}

Semaphore& Semaphore::operator=(const Semaphore & other) {
}

Semaphore::Semaphore(int value, bool pshared) {
    error_check(sem_init(&_sid, pshared == false ? 0 : 1, value));
}

Semaphore::~Semaphore() {
    sem_destroy(&_sid);
}

void Semaphore::post() {
    error_check(sem_post(&_sid));
}

void Semaphore::wait() {
    error_check(sem_wait(&_sid));
}

Semaphore& Semaphore::operator++() {
    this->post();
}

Semaphore& Semaphore::operator++(int) {
    this->post();
}

Semaphore& Semaphore::operator--() {
    this->wait();
}

Semaphore& Semaphore::operator--(int) {
    this->wait();
}