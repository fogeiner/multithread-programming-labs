#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv = PTHREAD_COND_INITIALIZER;

int counter = 1;

void *child(void *ptr) {

    for (int i = 0; i < 10;) {

		pthread_mutex_lock(&m);
		while(counter != 0){
			pthread_cond_wait(&cv, &m);
		}
        printf("%d child\n", i++);
		counter = 1;
        pthread_cond_signal(&cv);
        pthread_mutex_unlock(&m);
    }
	return NULL;
}

void *parent(void *ptr) {

    for (int i = 0; i < 10;) {
		pthread_mutex_lock(&m);
		while(counter != 1){
			pthread_cond_wait(&cv, &m);
		}
        printf("%d parent\n", i++);
		counter = 0;
        pthread_cond_signal(&cv);
		pthread_mutex_unlock(&m);
    }
	return NULL;
}


int main(int argc, char *argv[]) {

    pthread_t tid;
    pthread_create(&tid, NULL, child, NULL);
    parent(NULL);
    pthread_join(tid, NULL);
    pthread_exit(NULL);
}
