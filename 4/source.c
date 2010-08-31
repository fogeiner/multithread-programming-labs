#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>

static char path[] = "/dev/urandom";
static const int BUFFER_SIZE  = 8*1024;

static void *child(void *ptr){
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
