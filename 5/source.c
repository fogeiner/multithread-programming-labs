#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>

static char path[] = "/dev/urandom";
static char msg[] = "He's trying to kill me!";

static const int BUFFER_SIZE  = 8*1024;

static void print_msg(void *arg){
	printf("%s\n", msg);
	fflush(stdout);
}

static void *child(void *ptr){
	pthread_cleanup_push(print_msg, NULL);

	int fd = *((int*)ptr);
	char buf[BUFFER_SIZE];

	for(;;){
		int n = read(fd, buf, sizeof(buf));

		for(int i = 0; i < n; ++i){
			if(isprint(buf[i])){
				write(STDOUT_FILENO, &buf[i], 1);
			}
		}

	}
	pthread_cleanup_pop(1);
}

int main(int argc, char *argv[]){

	int fd = open(path, O_RDONLY);

	pthread_t tid;
	pthread_create(&tid, NULL, &child, &fd);
	sleep(2);
	pthread_cancel(tid);
	pthread_join(tid, NULL);

	close(fd);
	return (EXIT_SUCCESS);
}
