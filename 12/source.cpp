/*NOT READY!*/
#include <pthread.h>
#include <cstdio>
#include <cerrno>
#include <cstring>
#include <unistd.h>
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv = PTHREAD_COND_INITIALIZER;

int counter = 1;

void check_error(int retv) {
    if (retv != 0) {
        char buf[256];
        strerror_r(errno, buf, sizeof (buf));
        fprintf(stderr, "Error: %s\n", buf);
    }
}

void *child(void *ptr) {

    for (int i = 0; i < 10;) {

		check_error(pthread_mutex_lock(&m));
		while(counter != 0){
			check_error(pthread_cond_wait(&cv, &m));
		}
        printf("%d child\n", i++);
		counter = 1;
        check_error(pthread_cond_signal(&cv));
        check_error(pthread_mutex_unlock(&m));
    }


}

void *parent(void *ptr) {

    for (int i = 0; i < 10;) {
		check_error(pthread_mutex_lock(&m));
		while(counter != 1){
			check_error(pthread_cond_wait(&cv, &m));
		}
        printf("%d parent\n", i++);
		counter = 0;
        check_error(pthread_cond_signal(&cv));
		check_error(pthread_mutex_unlock(&m));
    }


}


int main(int argc, char *argv[]) {

    pthread_t tid;
    check_error(pthread_create(&tid, NULL, child, NULL));
    parent(NULL);
    check_error(pthread_join(tid, NULL));
    pthread_exit(NULL);
}
