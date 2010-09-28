#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#define PHILO 5
#define DELAY 30000
#define FOOD 50

pthread_mutex_t forks[PHILO];
pthread_mutex_t forks_mutex;
pthread_cond_t cv;

pthread_t phils[PHILO];
void *philosopher(void *id);
int food_on_table();
void get_fork(int, int, char *);
void down_forks(int, int);
pthread_mutex_t foodlock;

int sleep_seconds = 0;

int main(int argn, char **argv) {
    int i;

    if (argn == 2) {
        sleep_seconds = atoi(argv[1]);
    }

    pthread_mutex_init(&foodlock, NULL);
    pthread_mutex_init(&forks_mutex, NULL);
    pthread_cond_init(&cv, NULL);

    for (i = 0; i < PHILO; i++)
        pthread_mutex_init(&forks[i], NULL);
    for (i = 0; i < PHILO; i++)
        pthread_create(&phils[i], NULL, philosopher, (void *) i);
    for (i = 0; i < PHILO; i++)
        pthread_join(phils[i], NULL);
    return 0;
}

void *philosopher(void *num) {
    int id;
    int left_fork, right_fork, f;

    id = (int) num;
    printf("Philosopher %d sitting down to dinner.\n", id);
    right_fork = id;
    left_fork = id + 1;

    /* Wrap around the forks. */
    if (left_fork == PHILO)
        left_fork = 0;

    while (f = food_on_table()) {

        /* Thanks to philosophers #1 who would like to
         * take a nap before picking up the forks, the other
         * philosophers may be able to eat their dishes and
         * not deadlock.
         */
        if (id == 1)
            sleep(sleep_seconds);

        printf("Philosopher %d: get dish %d\n", id, f);


        pthread_mutex_lock(&forks_mutex);
        while (1) {

            int retv;

            retv = pthread_mutex_trylock(&forks[right_fork]);
            if (retv == EBUSY) {
                printf("Philosopher %d: failed getting fork %d\n", id, right_fork);
                pthread_cond_wait(&cv, &forks_mutex);
                continue;
            }
            printf("Philosopher %d: got fork %d\n", id, right_fork);

            retv = pthread_mutex_trylock(&forks[left_fork]);
            if (retv == EBUSY) {
                printf("Philosopher %d: failed getting fork %d\n", id, left_fork);
                printf("Philosopher %d: put fork %d\n", id, right_fork);
                pthread_mutex_unlock(&forks[right_fork]);
                pthread_cond_wait(&cv, &forks_mutex);
                continue;
            }
            printf("Philosopher %d: got fork %d\n", id, left_fork);
            pthread_mutex_unlock(&forks_mutex);
            break;
        }

        printf("Philosopher %d: eating.\n", id);
        usleep(DELAY * (FOOD - f + 1));

        down_forks(left_fork, right_fork);
    }
    printf("Philosopher %d is done eating.\n", id);
    return (NULL);
}

int food_on_table() {
    static int food = FOOD;
    int myfood;

    pthread_mutex_lock(&foodlock);
    if (food > 0) {
        food--;
    }
    myfood = food;
    pthread_mutex_unlock(&foodlock);
    return myfood;
}

void down_forks(int f1, int f2) {
    pthread_mutex_unlock(&forks[f1]);
    pthread_mutex_unlock(&forks[f2]);

    pthread_cond_signal(&cv);
}
