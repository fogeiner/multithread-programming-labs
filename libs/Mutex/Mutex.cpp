#include "Mutex.h"

inline void Mutex::error_check(int retv) {

    if (retv != 0) {
		char buf[256];
	    ::strerror_r(errno, buf, sizeof (buf));
        throw MutexException(buf);
    }
}

Mutex::Mutex(enum mutex_type type) {
    error_check(pthread_mutexattr_init(&_mattrid));
    error_check(pthread_mutexattr_settype(&_mattrid, type));
    error_check(pthread_mutex_init(&_mid, &_mattrid));
    locks = 0;
}

Mutex::Mutex(const Mutex& other){
	error_check(pthread_mutexattr_init(&_mattrid));
	int type;
	error_check(pthread_mutexattr_gettype(&other._mattrid, &type));
	error_check(pthread_mutexattr_settype(&_mattrid, type));
	error_check(pthread_mutex_init(&_mid, &_mattrid));
	locks = 0;
}

Mutex::~Mutex() {
    while(locks-- > 0){
        // not unlock, because we don't want to get exception in destructor
        pthread_mutex_unlock(&_mid);
    }
    error_check(pthread_mutex_destroy(&_mid));
    error_check(pthread_mutexattr_destroy(&_mattrid));
}

void Mutex::lock() {
    error_check(pthread_mutex_lock(&_mid));
    locks++;
}

void Mutex::unlock() {
    error_check(pthread_mutex_unlock(&_mid));
    locks--;
}

bool Mutex::trylock() {
    int retv = pthread_mutex_trylock(&_mid);

    if (retv == EBUSY) {
        return false;
    } else {
        error_check(retv);
        locks++;
        return true;
    }
}



