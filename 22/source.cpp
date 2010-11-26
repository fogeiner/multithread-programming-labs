#include "../libs/Thread/Thread.h"
#include "../libs/Semaphore/Semaphore.h"
#include <cstdio>
#include <csignal>

/*Разработайте имитатор производственной линии, изготавливающей винтики (widget).
 * Винтик собирается из детали C и модуля, который, в свою очередь,
 * состоит из деталей A и B. Для изготовления детали A требуется 1 секунда,
 * В – две секунды, С – три секунды.
 * Задержку изготовления деталей имитируйте при помощи sleep. Используйте семафоры-счетчики.
 */

// program is stopped by sending SIGINT;
// stop_signal_received is used to indicate running threads to terminate
bool stop_signal_received = false;

// ~ --- X is semaphore corresponding to Y object
// a ~ detA, b ~ detB, c ~ detC, m ~ module, w ~ widget
Semaphore a, b, c, m, w;

// count of seconds each of production threads sleep
const int 	A_PROD_TIME = 0,//1,
			B_PROD_TIME = 0,//2,
			C_PROD_TIME = 0;//3;

// count of seconds between semaphores state printing
const int PRINT_DELAY_TIME = 1;

void *produce_widget(void *ptr) {
	try{
		while(!stop_signal_received) {
			m--;
			c--;
			w++;
		}
	} catch(SemaphoreException &ex) {
		fprintf(stderr, "Error: %s\n", ex.what());
	}

	return NULL;
}


void *produce_module(void *ptr) {
	try{
		while(!stop_signal_received) {
			a--;
			b--;
			m++;
		}
	} catch(SemaphoreException &ex) {
		fprintf(stderr, "Error: %s\n", ex.what());
	}

	return NULL;
}

void *produce_detail_A(void *ptr) {
	try{
		while(!stop_signal_received) {
			sleep(A_PROD_TIME);
			a++;
		}
	} catch(SemaphoreException &ex) {
		fprintf(stderr, "Error: %s\n", ex.what());
	}

	return NULL;
}

void *produce_detail_B(void *ptr) {
	try{
		while(!stop_signal_received) {
			sleep(B_PROD_TIME);
			b++;
		}
	} catch(SemaphoreException &ex) {
		fprintf(stderr, "Error: %s\n", ex.what());
	}

	return NULL;
}

void *produce_detail_C(void *ptr) {
	try{
		while(!stop_signal_received) {
			sleep(C_PROD_TIME);
			c++;
		}
	} catch(SemaphoreException &ex) {
		fprintf(stderr, "Error: %s\n", ex.what());
	}

	return NULL;
}

// function for printing out the current state of semaphores
// actually, there's no guarantee that printed values are correct
void *print_state(void *ptr) {
	while(!stop_signal_received) {
		fprintf(stdout, "a: %d b: %d c: %d m: %d w: %d\n",
				a.getvalue(), b.getvalue(), c.getvalue(), m.getvalue(), w.getvalue());
		sleep(1);
	}
	fprintf(stdout, "Stopping due to the signal\n");
	return NULL;
}

// simple but ugly signal handler
// lambda functions make sense for things like 

void sigint_handler(int sig_num){
	stop_signal_received = true;
}

int main(int argc, char *argv[]) {
	try{
		// initing Thread objects
		Thread threads[] = {
			Thread(produce_detail_A), Thread(produce_detail_B), Thread(produce_detail_C),
			Thread(produce_module), Thread(produce_widget), Thread(print_state)
		};

		// starting Threads
		for(int i = 0; i < sizeof(threads)/sizeof(Thread); ++i){
			threads[i].run();
		}

		// setting signal handler for stopping programm
		signal(SIGINT, sigint_handler);
		// stopping main thread; not actually needed
		// main thread will continue execution after the signal arrives
		pause();

		// singal has arrived, need to join threads to release taken resources
		// in case threads type is Thread[some_number] this will work
		// Problem may occur if someone passes that array like a pointer
		// -- information needed by sizeof operator will be lost
		for(int i = 0; i < sizeof(threads)/sizeof(Thread); ++i){
			threads[i].join();
		}

		// exiting main thread
		// here can successfully be replaced with exit(0), return 0 and so forth
	} catch(std::exception &ex) {
		fprintf(stderr, "Error: %s\n", ex.what());
	}

	Thread::exit();
}
