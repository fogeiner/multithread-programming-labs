#include "Thread.h"

Thread::Thread(const Thread& orig) {
    if (!orig._run) {
        _func = orig._func;
        _arg = orig._arg;
        _needs_resource_release = orig._needs_resource_release;
        _run = orig._run;
    } else {
        throw ThreadException("Assigning used thread");
    }
}

void Thread::error_check(int retv) {
    char buf[256];
    ::strerror_r(errno, buf, sizeof (buf));
    if (retv != 0) {
        throw ThreadException(buf);
    }
}

Thread::~Thread() {
    if (_run && _needs_resource_release) {
        this->detach();
    }
}

void Thread::run() {
    _run = true;
    error_check(pthread_create(&_tid, NULL, _func, _arg));
    _needs_resource_release = true;
}

void Thread::detach() {
    Thread::error_check(pthread_detach(_tid));
    _needs_resource_release = false;
}

void Thread::join(void **ptr) {
    Thread::error_check(pthread_join(_tid, ptr));
    _needs_resource_release = false;
}
