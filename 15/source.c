#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>

const char *sem_name1 = "/lab25_1";
const char *sem_name2 = "/lab25_2";

void error_check(int retv) {
    char buf[256];
    strerror_r(errno, buf, sizeof (buf));
    if (retv == -1) {
        fprintf(stderr, "Error occured: %s\n", buf);
        exit(EXIT_FAILURE);
    }
}

void child() {
    sem_t *sid1 = sem_open(sem_name1, O_CREAT, 0600, 0);
    if (sid1 == SEM_FAILED) {
        error_check(-1);
    }
    sem_t *sid2 = sem_open(sem_name2, O_CREAT, 0600, 1);
    if (sid2 == SEM_FAILED) {
        error_check(-1);
    }

    for (int i = 0; i < 10; ++i) {
        error_check(sem_wait(sid1));
        printf("%d child\n", i);
        error_check(sem_post(sid2));
    }

    error_check(sem_close(sid1));
    error_check(sem_close(sid2));

}

void parent() {
    sem_t *sid1 = sem_open(sem_name1, O_CREAT, 0600, 0);
    if (sid1 == SEM_FAILED) {
        error_check(-1);
    }
    sem_t *sid2 = sem_open(sem_name2, O_CREAT, 0600, 1);
    if (sid2 == SEM_FAILED) {
        error_check(-1);
    }

    for (int i = 0; i < 10; ++i) {
        error_check(sem_wait(sid2));
        printf("%d parent\n", i);
        error_check(sem_post(sid1));
    }

    error_check(sem_close(sid1));
    error_check(sem_close(sid2));
    error_check(sem_unlink(sem_name1));
    error_check(sem_unlink(sem_name2));
}

int main(int argc, char *argv[]) {


    pid_t pid = fork();
    switch (pid) {
            // child
        case 0:
            child();
            break;
            // error
        case -1:
            error_check(pid);
            // parent
        default:
            parent();
            break;
    }
}