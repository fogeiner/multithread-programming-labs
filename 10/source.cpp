#include "../libs/Mutex/Mutex.h"
#include "../libs/Thread/Thread.h"
#include <cstdio>

Mutex m[3];

const int PRINT_TIMES = 10;
int printed_flag = 0;

void *print(void *str) {
    try {
		
		int k = 1;
		m[2].lock();	
		if(printed_flag == 1){
			m[0].unlock();
		}
        for (int i = 0; i < PRINT_TIMES * 3; ++i) {
            m[k].lock();
            k = (k + 1) % 3;
            m[k].unlock();
            if(k == 2){
				printf("%d %s", i/3, static_cast<char*>(str));
				printed_flag = 1;
			}
			k = (k + 1) % 3;
        }
        
        m[2].unlock();
    } catch (std::exception &ex) {
        fprintf(stderr, "Exception: %s", ex.what());
    }
    return NULL;
}


int main(int argc, char *argv[]) {

    m[0].lock();
    try {
		char child_msg[] = "child\n";
		char parent_msg[] = "parent\n";
        Thread chld(print, child_msg);
        chld.run();
        while(printed_flag == 0){
			sched_yield();
		}
        print(parent_msg);
        chld.join();
        Thread::exit();
    } catch (std::exception &ex) {
        fprintf(stderr, "Exception: %s", ex.what());
    }
}
