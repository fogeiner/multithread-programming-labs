#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>


// names must start with '/'; on Linux named semaphores
// can be found in /dev/shm and it's not true for Solaris
const char *sem_name1 = "/lab15_1";
const char *sem_name2 = "/lab15_2";

void error_check(int retv) {
	if (retv == -1) {
		fprintf(stderr, "Error occured: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
}

void child(sem_t *sid1, sem_t *sid2) {

	for (int i = 0; i < 10; ++i) {
		error_check(sem_wait(sid1));
		printf("%d child\n", i);
		error_check(sem_post(sid2));
	}
}

void parent(sem_t *sid1, sem_t *sid2) {
	
	for (int i = 0; i < 10; ++i) {
		error_check(sem_wait(sid2));
		printf("%d parent\n", i);
		error_check(sem_post(sid1));
	}

}
void print_usage_and_exit(const char *prog_name){
		fprintf(stderr, "Usage: %s parent|child\n", prog_name);
		exit(EXIT_FAILURE);
}
int main(int argc, char *argv[]) {
	const char parent_string[] = "parent";
	const char child_string[] = "child";

	if(argc != 2){
		print_usage_and_exit(argv[0]);
	}

	sem_t *sid1, *sid2;
	if(strcmp(argv[1], parent_string) == 0){
		if((sid1 = sem_open(sem_name1, O_CREAT, 0600, 0)) == SEM_FAILED){
			error_check(-1);
		}

		// waiting for child to signal it's ready
		error_check(sem_wait(sid1));
		
		if((sid2 = sem_open(sem_name2, O_RDWR)) == SEM_FAILED){
			error_check(-1);
		}

		parent(sid1, sid2);
		error_check(sem_close(sid1));
		error_check(sem_close(sid2));
		error_check(sem_unlink(sem_name1));
	}
	else if (strcmp(argv[1], child_string) == 0){
		if((sid1 = sem_open(sem_name1, O_RDWR)) == SEM_FAILED){
			error_check(-1);
		}

		if((sid2 = sem_open(sem_name2, O_CREAT, 0600, 1)) == SEM_FAILED){
			error_check(-1);
		}

		error_check(sem_post(sid1));

		child(sid1, sid2);
		error_check(sem_close(sid1));
		error_check(sem_close(sid2));
		error_check(sem_unlink(sem_name2));
	} else {
		print_usage_and_exit(argv[0]);
	}
}
