#include "Thread.h"

void Thread::error_check(int retv) {
    if (retv != 0) {
		char buf[256];
#ifdef __GNU
		char *msg_ptr;
		msg_ptr = ::strerror_r(retv, buf, sizeof (buf));
        throw ThreadException(msg_ptr);
#else
		::strerror_r(retv, buf, sizeof(buf));
		throw ThreadException(buf);
#endif
    }
}

void Thread::run() {
    _run = true;
    error_check(pthread_create(&_tid, NULL, _func, _arg));
}

void Thread::detach() {
    Thread::error_check(pthread_detach(_tid));
}

void Thread::join(void **ptr) {
    Thread::error_check(pthread_join(_tid, ptr));
}
