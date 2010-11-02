#include <pthread.h>
#include <signal.h>
#include <cstdio>
#include <iostream>

#include "../libs/Semaphore/Semaphore.h"
#include "../libs/MsgQueue/SemMsgQueue.h"
#include "../libs/Thread/Thread.h"

int flag_to_stop = 0;
const int size = 256;
MsgQueue *msgq;

void *f1(void *arg) {
	try{
		MsgQueue *m = static_cast<MsgQueue*> (arg);
		char buf[size];
		for (int i = 0;; ++i) {
			if (flag_to_stop) {
				m->drop();
				break;
			}

			sprintf(buf, "f1 %d\n", i);
			m->put(buf);
			sleep(2);
		}
	} catch(std::exception &ex){
		fprintf(stderr, "Error: %s", ex.what());
		exit(EXIT_FAILURE);
	}
	return NULL;
}

void *f2(void *arg) {
	try{
		MsgQueue *m = static_cast<MsgQueue*> (arg);
		char buf[size];
		for (int i = 0;; ++i) {
			if (flag_to_stop)
				break;

			sprintf(buf, "f2 %d\n", i);
			m->put(buf);
			sleep(3);
		}
	} catch(std::exception &ex){
		fprintf(stderr, "Error: %s", ex.what());
		exit(EXIT_FAILURE);
	}
	return NULL;

}

void *f3(void *arg) {
	try{
		MsgQueue *m = static_cast<MsgQueue*> (arg);
		char buf[size];
		for (int i = 0;; ++i) {
			if (flag_to_stop)
				break;

			int r = m->get(buf, sizeof (buf));
			fprintf(stdout, "f3 read %d symbols: %s", r, buf);
			sleep(2);
		}
	} catch(std::exception &ex){
		fprintf(stderr, "Error: %s", ex.what());
		exit(EXIT_FAILURE);
	}
	return NULL;
}

void *f4(void *arg) {
	try{
		MsgQueue *m = static_cast<MsgQueue*> (arg);
		char buf[size];
		for (;;) {
			if (flag_to_stop)
				break;

			int r = m->get(buf, sizeof (buf));
			fprintf(stdout, "f4 read %d symbols: %s", r, buf);
			sleep(1);
		}
	} catch(std::exception &ex){
		fprintf(stderr, "Error: %s", ex.what());
		exit(EXIT_FAILURE);
	}
	return NULL;
}

// changing names to <signal name>_handler will affect the semantics badly
void drop(int sig) {
	msgq->drop();
}

void stop(int sig){
	flag_to_stop = 1;
}


int main(int argc, char *argv[]) {
	try {
		msgq = new SemMsgQueue(10);
		Thread threads[] = {Thread(f1, msgq),
			Thread(f2, msgq), Thread(f3, msgq), Thread(f4, msgq)};
		for (int i = 0; i < sizeof (threads) / sizeof (Thread); ++i) {
			threads[i].run();
		}

		signal(SIGINT, drop);
		signal(SIGQUIT, stop);

		for (int i = 0; i < sizeof (threads) / sizeof (Thread); ++i) {
			threads[i].join();
		}

		delete msgq;

		Thread::exit(NULL);
	} catch (std::exception &ex) {
		std::cerr << ex.what();
		return EXIT_FAILURE;
	}
}
