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
// stop_flag is used to indicate running threads to terminate
bool stop_flag = false;

// ~ --- X is semaphore corresponding to Y object
// a ~ detA, b ~ detB, c ~ detC, m ~ module, w ~ widget
Semaphore a, b, c, m, w;

// count of seconds each of production threads sleep
const int 	A_PROD_TIME = 1,
			B_PROD_TIME = 2,
			C_PROD_TIME = 3;

// count of seconds between semaphores state printing
const int PRINT_DELAY_TIME = 1;

void *widget(void *ptr) {
	try{
		for (;!stop_flag;) {
			m--;
			c--;
			w++;
		}
	} catch(std::exception &ex) {
		fprintf(stderr, "Error: %s\n", ex.what());
	}

	return NULL;
}

void *detC(void *ptr) {
	try{
		for (;!stop_flag;) {
			sleep(C_PROD_TIME);
			c++;
		}
	} catch(std::exception &ex) {
		fprintf(stderr, "Error: %s\n", ex.what());
	}

	return NULL;
}

void *detA(void *ptr) {
	try{
		for (;!stop_flag;) {
			sleep(A_PROD_TIME);
			a++;
		}
	} catch(std::exception &ex) {
		fprintf(stderr, "Error: %s\n", ex.what());
	}

	return NULL;
}

void *detB(void *ptr) {
	try{
		for (;!stop_flag;) {
			sleep(B_PROD_TIME);
			b++;
		}
	} catch(std::exception &ex) {
		fprintf(stderr, "Error: %s\n", ex.what());
	}

	return NULL;
}

void *module(void *ptr) {
	try{
		for (;!stop_flag;) {
			a--;
			b--;
			m++;
		}
	} catch(std::exception &ex) {
		fprintf(stderr, "Error: %s\n", ex.what());
	}

	return NULL;
}

// function for printing out the current state of semaphores
// actually, there's no guarantee that printed values are correct
void *print_state(void *ptr) {
	for (;!stop_flag;) {
		fprintf(stdout, "a: %d b: %d c: %d m: %d w: %d\n",
				a.getvalue(), b.getvalue(), c.getvalue(), m.getvalue(), w.getvalue());
		sleep(1);
	}
	fprintf(stdout, "Stopping due to stop flag\n");
	return NULL;
}

// simple but ugly signal handler
// lambda functions make sense for things like 

void stop(int sig_num){
	stop_flag = true;
}

int main(int argc, char *argv[]) {
	try{
		// initing Thread objects
		Thread threads[] = {
			Thread(detA), Thread(detB), Thread(detC),
			Thread(module), Thread(widget), Thread(print_state)
		};

		// starting Threads
		for(int i = 0; i < sizeof(threads)/sizeof(Thread); ++i){
			threads[i].run();
		}

		// setting signal handler for stopping programm
		signal(SIGINT, stop);
		// stopping main thread; not actually needed
		// main thread will continue execution after the signal arrives
		pause();

		// singal has arrived, need to join threads to release taken resources
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
