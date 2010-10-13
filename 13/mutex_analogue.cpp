#include "../libs/Mutex/Mutex.h"
#include "../libs/Thread/Thread.h"
#include <cstdio>

/*
       If the mutex is already locked by the  calling  thread,  the  behavior  of
       pthread_mutex_lock  depends  on  the kind of the mutex. If the mutex is of
       the ``fast'' kind, the calling thread is  suspended  until  the  mutex  is
       unlocked,  thus effectively causing the calling thread to deadlock. If the
       mutex is of the ``error checking'' kind, pthread_mutex_lock returns  imme‐
       diately with the error code EDEADLK.  If the mutex is of the ``recursive''
       kind, pthread_mutex_lock succeeds and returns immediately,  recording  the
       number  of  times the calling thread has locked the mutex. An equal number
       of pthread_mutex_unlock operations must  be  performed  before  the  mutex
       returns to the unlocked state.
 */

Mutex m1/*(Mutex::ERRORCHECK_MUTEX)*/, m2/*(Mutex::ERRORCHECK_MUTEX)*/;

void *child(void *ptr) {
    try {
        for (int i = 0; i < 10; ++i) {
            m1.lock();
            printf("%d child\n", i);
            m2.unlock();
        }
    } catch (std::exception &ex) {
        fprintf(stderr, "Exception: %s\n", ex.what());
    }
}

void parent() {
    try {
        for (int i = 0; i < 10; ++i) {
            m2.lock();
            printf("%d parent\n", i);
            m1.unlock();
        }
    } catch (std::exception &ex) {
        fprintf(stderr, "Exception: %s\n", ex.what());
    }
}

int main(int argc, char *argv[]) {
    try {
        m1.lock();
        Thread chld(child);
        chld.run();
        parent();
        chld.join();
        Thread::exit();
    } catch (std::exception &ex) {
        fprintf(stderr, "Exception: %s\n", ex.what());
    }
}
