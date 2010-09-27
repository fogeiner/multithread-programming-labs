#include "../libs/Mutex/Mutex.h"
#include "../libs/Thread/Thread.h"
#include <cstdio>

Mutex m1(Mutex::ERRORCHECK_MUTEX), m2(Mutex::ERRORCHECK_MUTEX), m3(Mutex::ERRORCHECK_MUTEX);

void *child(void *ptr) {
    try {
        for (int i = 0; i < 10; ++i) {
            m2.lock();
            m1.lock();
            //m3.lock();
           
            printf("%d child\n", i);
            
            m3.unlock();
            //m2.unlock();
            m1.unlock();
        }
    } catch (std::exception &ex) {
        fprintf(stderr, "Exception: %s", ex.what());
    }
}

void *parent(void *ptr = NULL) {
    try {
        for (int i = 0; i < 10; ++i) {
            m3.lock();
            m1.lock();
            //m2.lock();

            printf("%d parent\n", i);
            
            m2.unlock();
           // m3.unlock();
            m1.unlock();
        }
    } catch (std::exception &ex) {
        fprintf(stderr, "Exception: %s", ex.what());
    }
}

int main(int argc, char *argv[]) {
    m2.lock();
    try {
        Thread chld(child);
        chld.run();
        parent();
        chld.join();
        Thread::exit();
    } catch (std::exception &ex) {
        fprintf(stderr, "Exception: %s", ex.what());
    }
}
