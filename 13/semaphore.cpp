#include "../libs/Semaphore/Semaphore.h"
#include "../libs/Thread/Thread.h"
#include <cstdio>

Semaphore s1(0), s2(1);

void *child(void *ptr) {
    try {
        for (int i = 0; i < 10; ++i) {
            s1--;
            printf("%d child\n", i);
            s2++;
            
        }
    } catch (std::exception &ex) {
        fprintf(stderr, "Exception: %s", ex.what());
    }
}

void *parent(void *ptr = NULL) {
    try {
        for (int i = 0; i < 10; ++i) {
            s2--;
            printf("%d parent\n", i);
            s1++;
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
