#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

const int N = 10;
const int child_id = 0;
const int parent_id = 1;

static char msg_parent[] = "parent";
static char msg_child[] = "child";

pthread_mutex_t mutex = PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP;
int current_id;


static void print_msgs(char *s, int var){
	for(int i = 0; i < N; ){
		do{
			pthread_mutex_lock(&mutex);
			if(current_id == var){
				break;
			}
			
			current_id = (current_id == 1) ? 0 : 1;
			
			fprintf(stdout, "%2d: %s\n", i+1, s);
			i++;
			
		} while(0);
		
		pthread_mutex_unlock(&mutex);
	}
		
}


static void *child(void *ptr){
	print_msgs(msg_child, child_id);
}

int main(int argc, char *argv[]){

	// parent starts
	current_id = child_id;

	pthread_t tid;
	pthread_create(&tid, NULL, &child, NULL);
	print_msgs(msg_parent, parent_id);
	pthread_join(tid, NULL);
	return (EXIT_SUCCESS);
}
