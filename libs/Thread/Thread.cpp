#include "Thread.h"

void Thread::error_check(int retv) {
    if (retv != 0) {
        char buf[256];
#if (_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600) && ! _GNU_SOURCE || defined ( sun ) || defined ( __sun )
        ::strerror_r(retv, buf, sizeof (buf));
        throw ThreadException(buf);
#else
        char *msg_ptr;
        msg_ptr = ::strerror_r(retv, buf, sizeof (buf));
        throw ThreadException(msg_ptr);
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
