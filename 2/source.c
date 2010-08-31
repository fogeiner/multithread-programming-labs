#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

static const int N = 10;

#define PRINT_MSGS(X) for(int i = 0; i < N; ++i)\
						fprintf(stdout, "%2d: %s\n", i+1, (X))

static char msg_parent[] = "hello from parent";
static char msg_child[] = "hello from child";

static void *child(void *ptr){
	PRINT_MSGS(msg_child);
}

int main(int argc, char *argv[]){

	pthread_t tid;
	pthread_create(&tid, NULL, &child, NULL);
	pthread_join(tid, NULL);
	PRINT_MSGS(msg_parent);
	return (EXIT_SUCCESS);
}
