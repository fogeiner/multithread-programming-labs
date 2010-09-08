#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>

static int time_to_stop = 0;

struct data{
	pthread_t tid;
	int valid;
	long double result;
};

void *part_sum(void *arg){
	struct data *data = arg;

	for (int i = 0; ; i++) {

		if ((i % 100000  == 0)&&(time_to_stop))
			break;

		data->result += 1.0/(i*4.0 + 1.0);
		data->result -= 1.0/(i*4.0 + 3.0);
	}

	return NULL;
}

void stop(int s){
	time_to_stop = 1;
}

int main(int argc, char *argv[]) {

	int ret;

	if(argc != 2){
		fprintf(stderr, "Usage: %s num_of_threads", argv[0]);
		return 1;
	}

	int num_of_threads;
	num_of_threads = atoi(argv[1]);
	if ((num_of_threads < 1) || (num_of_threads > 65535)){
		fprintf(stderr, "Invalid number of threads: %d", num_of_threads);
		return 1;
	}

	long double pi = 0;
	struct data threads[num_of_threads];
	for(int i = 0; i < num_of_threads; ++i){
		threads[i].result = 0;
		threads[i].valid = 1;
		ret = pthread_create(&threads[i].tid, NULL, &part_sum, &threads[i]);
		if(ret != 0){
			threads[i].valid = 0;
			fprintf(stderr, "Error creating thread: %s", strerror_r(ret));
		}
	}

	signal(SIGINT, stop);
	printf("To stop computing send SIGINT\n");

	for(int i = 0; i < num_of_threads; ++i){
		if(threads[i].valid == 1){
			pthread_join(threads[i].tid, NULL);
		}
		pi += threads[i].result;
	}

	pi = pi * 4.0;
	printf("pi done - %.15Lg \n", pi/num_of_threads);    

	return (EXIT_SUCCESS);
}
