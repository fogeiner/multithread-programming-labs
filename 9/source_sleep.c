#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#define PHILO 5
#define DELAY 30000
#define FOOD 50

void wait(int);
void down_forks(int, int);
void *philosopher(void*);
int food_on_table();
void get_fork(int, int, char*);

pthread_mutex_t forks[PHILO];
pthread_t phils[PHILO];
pthread_mutex_t foodlock;

int sleep_seconds = 0;

int
main(int argn, char **argv) {
    int i;

    if (argn == 2)
        sleep_seconds = atoi(argv[1]);

    pthread_mutex_init(&foodlock, NULL);

    for (i = 0; i < PHILO; i++)
        pthread_mutex_init(&forks[i], NULL);

    for (i = 0; i < PHILO; i++)
        pthread_create(&phils[i], NULL, philosopher, (void *) i);

    for (i = 0; i < PHILO; i++)
        pthread_join(phils[i], NULL);

    return 0;
}

void *
philosopher(void *num) {
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

        printf("Philosopher %d: get dish %d.\n", id, f);

        for (;;) {
            pthread_mutex_lock(&forks[left_fork]);
            printf("Philosopher %d: got %s fork %d\n", id, "left", left_fork);

            if (pthread_mutex_trylock(&forks[right_fork]) != EBUSY) {
                printf("Philosopher %d: got %s fork %d\n", id, "right", right_fork);
                break;
            } else {
                printf("Philosopher %d: put %s fork %d (another was already taken)\n", id, "left", left_fork);
                pthread_mutex_unlock(&forks[left_fork]);
                wait(f);
				continue;
            }
        }
        printf("Philosopher %d: eating.\n", id);
        wait(f);
        down_forks(right_fork, left_fork);
    }
    printf("Philosopher %d is done eating.\n", id);
    return (NULL);
}

void wait(int f) {
    usleep(DELAY * (FOOD - f + 1));
}

int
food_on_table() {
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

void
down_forks(int f1, int f2) {
    pthread_mutex_unlock(&forks[f1]);
    pthread_mutex_unlock(&forks[f2]);
}

