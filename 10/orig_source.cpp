#include "../libs/Mutex/Mutex.h"
#include "../libs/Thread/Thread.h"
#include <cstdio>

Mutex m1, m2, m3;

void *child(void *ptr = NULL) {
    try {
        for (int i = 0; i < 10; ++i) {
            m1.lock();
            m3.lock();
            m2.lock();
           
            printf("%d child\n", i);

            m2.unlock();
            m1.unlock();
            m3.unlock();

        }
    } catch (std::exception &ex) {
        fprintf(stderr, "Exception: %s", ex.what());
    }
}

void *parent(void *ptr = NULL) {
    try {
        for (int i = 0; i < 10; ++i) {
            m1.lock();
            m2.lock();
            m3.lock();

            printf("%d parent\n", i);

            m3.unlock();
            m1.unlock();
            m2.unlock();

        }
    } catch (std::exception &ex) {
        fprintf(stderr, "Exception: %s", ex.what());
    }
}

int main(int argc, char *argv[]) {
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
