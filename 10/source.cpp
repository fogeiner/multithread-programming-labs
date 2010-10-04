#include "../libs/Mutex/Mutex.h"
#include "../libs/Thread/Thread.h"
#include <cstdio>

Mutex m1(Mutex::ERRORCHECK_MUTEX), m2(Mutex::ERRORCHECK_MUTEX), m3(Mutex::ERRORCHECK_MUTEX);
Mutex m[3] = {m1, m2, m3};
Mutex mflag(Mutex::ERRORCHECK_MUTEX);
int flag = 0;

void *print(void *p_str) {
	char *str = static_cast<char*>(p_str);
	const int NUM_OF_PRINTS = 10;
    try {
		// init: [P][P][C]
		int k;
		
		if (str[0] == 'c'){
			m[0].lock();
			for(;;){
				mflag.lock();
				if(flag == 1){
					mflag.unlock();
					break;	
				}
				mflag.unlock();
			}
			k = 0;
		}
		
		if(str[0] == 'p'){
			m[1].lock();
			k = 1;
		}
		
        for (int i = 0; i != NUM_OF_PRINTS;) {
			k = (k + 1) % 3;
			m[k].lock();
			if(k == 2){
				printf("%d %s\n", i, str);
				i++;
				mflag.lock();
				flag = 1;
				mflag.unlock();
			}
			m[(k - 1 + 3) % 3].unlock();
        }
        
        m[k].unlock();
    } catch (std::exception &ex) {
        fprintf(stderr, "Exception: %s", ex.what());
    }
}

int main(int argc, char *argv[]) {
    try {
		char *child_msg = "child";
		char *parent_msg = "parent";
        Thread chld(print, child_msg);
        chld.run();
        print(parent_msg);
        chld.join();
        Thread::exit();
    } catch (std::exception &ex) {
        fprintf(stderr, "Exception: %s", ex.what());
    }
}
