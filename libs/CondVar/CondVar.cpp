#include "CondVar.h"

void CondVar::error_check(int retv) {
    char buf[256];
    ::strerror_r(errno, buf, sizeof (buf));
    if (retv != 0) {
        throw CondVarException(buf);
    }
}

CondVar::CondVar() {
    error_check(pthread_condattr_init(&_caid));
    error_check(pthread_cond_init(&_cvid, &_caid));
    error_check(pthread_mutex_init(&_mid, NULL));
}

CondVar::~CondVar() {
    pthread_condattr_destroy(&_caid);
    pthread_cond_destroy(&_cvid);
    pthread_mutex_destroy(&_mid);
}

void CondVar::signal() {
    error_check(pthread_cond_signal(&_cvid));
}

void CondVar::broadcast() {
    error_check(pthread_cond_broadcast(&_cvid));
}

void CondVar::wait() {
    error_check(pthread_cond_wait(&_cvid, &_mid));
}

void CondVar::timedwait(const struct timespec *abstime) {
    error_check(pthread_cond_timedwait(&_cvid, &_mid, abstime));
}
