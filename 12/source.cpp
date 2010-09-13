/*NOT READY!*/
#include <pthread.h>
#include <cstdio>
#include <cerrno>
#include <cstring>
#include <unistd.h>
pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv = PTHREAD_COND_INITIALIZER;

void *child(void *ptr) {
    pthread_mutex_lock(&m1);

    for (int i = 0; i < 10; ++i) {

        printf("%d child\n", i);
        pthread_cond_wait(&cv, &m1);
        pthread_mutex_unlock(&m2);
    }


}

void *parent(void *ptr) {

    for (int i = 0; i < 10; ++i) {
        pthread_mutex_lock(&m1);
        pthread_mutex_lock(&m2);
        printf("%d parent\n", i);
        pthread_cond_signal(&cv);
    }


}

void check_error(int retv) {
    if (retv != 0) {
        char buf[256];
        strerror_r(errno, buf, sizeof (buf));
        fprintf(stderr, "Error: %s\n", buf);
    }
}

int main(int argc, char *argv[]) {

    pthread_t tid;
    check_error(pthread_create(&tid, NULL, child, NULL));
    ::sleep(1);
    parent(NULL);
    check_error(pthread_join(tid, NULL));
    pthread_exit(NULL);
}
