#include "CondVar.h"

void CondVar::error_check(int retv) {
	if(retv != 0){
    char buf[256];
#ifdef __GNU
	char *msg_ptr;
    msg_ptr = ::strerror_r(retv, buf, sizeof (buf));
    throw CondVarException(msg_ptr);
#else
	::strerror_r(retv, buf, sizeof(buf));
	throw CondVarException(buf);
#endif
	}
}

CondVar::CondVar() {
    error_check(pthread_cond_init(&_cvid, NULL));
}

CondVar::~CondVar() {
    pthread_cond_destroy(&_cvid);
}

void CondVar::signal() {
    error_check(pthread_cond_signal(&_cvid));
}

void CondVar::broadcast() {
    error_check(pthread_cond_broadcast(&_cvid));
}

void CondVar::wait(Mutex &m) {
    error_check(pthread_cond_wait(&_cvid, &m._mid));
}

void CondVar::timedwait(Mutex &m, const struct timespec *abstime) {
    error_check(pthread_cond_timedwait(&_cvid, &m._mid, abstime));
}
