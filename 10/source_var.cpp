#include "../libs/Mutex/Mutex.h"
#include "../libs/Thread/Thread.h"
#include <cstdio>

Mutex m(Mutex::ERRORCHECK_MUTEX);
int counter;

void *child(void *ptr = NULL) {
    try {
		int i = 0;
		while(i < 10){
            m.lock();
           
			if((counter % 2) != 1){
				m.unlock();
				continue;
			}

            printf("%d child\n", i++);

			counter = 0;
            m.unlock();

        }
    } catch (std::exception &ex) {
        fprintf(stderr, "Exception: %s", ex.what());
    }
}

void *parent(void *ptr = NULL) {
    try {
		int i = 0;
        while(i < 10) {
            m.lock();
			if((counter % 2) != 0){
				m.unlock();
				continue;
			}

            printf("%d parent\n", i++);
			counter = 1;
            m.unlock();
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
