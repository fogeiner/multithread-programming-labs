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

int main(int argc, char *argv[]) {
	// idea: create new named semaphores,
	// fork, each process will print it's own message;
	// every process will close semaphores
	// and one process will unlink them

	// in case these named semaphores already exist (which can mean
	// that, for example, that some other instance of the programm is
	// currently running), error msg will be printed and program will exit

	sem_t *sid1 = sem_open(sem_name1, O_CREAT | O_EXCL, 0600, 0);
	if (sid1 == SEM_FAILED) {
		error_check(-1);
	}
	sem_t *sid2 = sem_open(sem_name2, O_CREAT | O_EXCL, 0600, 1);
	if (sid2 == SEM_FAILED) {
		error_check(sem_close(sid1));
		error_check(sem_unlink(sem_name1));
		error_check(-1);
	}

	pid_t pid = fork();
	switch (pid) {
		// child
		case 0:
			child(sid1, sid2);
			error_check(sem_close(sid1));
			error_check(sem_close(sid2));
			break;
		// error
		case -1:
			error_check(-1);
			break;
		// parent
		default:
			parent(sid1, sid2);
			error_check(sem_close(sid1));
			error_check(sem_close(sid2));
			// parent unlinks semaphores;
			// as soon as all processes close them
			// they'll delete from the system
			error_check(sem_unlink(sem_name1));
			error_check(sem_unlink(sem_name2));
			// not really needed buf if parent finishes
			// earlier than child, the last print by child
			// can be printed after shell PS
			waitpid(pid, NULL, 0);
			break;
	}
}
