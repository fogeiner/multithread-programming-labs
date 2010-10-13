#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>

static char path[] = "/dev/urandom";
static char msg[] = "He's trying to kill me!";
static const int BUFFER_SIZE  = 8*1024;

static void die_with_error(int code){
	char *s = strerror_r(code);
	write(STDERR_FILENO, s, strlen(s));
	exit(EXIT_FAILURE);
}


static void print_msg(void *arg){
	write(1, msg, strlen(msg));
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
	pthread_cleanup_pop(0);
	return NULL;
}

int main(int argc, char *argv[]){
	int ret;
	int fd = open(path, O_RDONLY);

	pthread_t tid;
	ret = pthread_create(&tid, NULL, &child, &fd);

	if(ret != 0){
		die_with_error(ret);
	}

	sleep(2);

	ret = pthread_cancel(tid);
	if(ret != 0){
		die_with_error(ret);
	}

	ret = pthread_join(tid, NULL);
	if(ret != 0){
		die_with_error(ret);
	}

	close(fd);
	return (EXIT_SUCCESS);

}
