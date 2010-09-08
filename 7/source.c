#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

struct data{
	pthread_t tid;
	int valid;
	int start;
	int num_of_steps;
	double result;
};


void *part_sum(void *arg){
	struct data *data = arg;

	for (int i = data->start; i < data->num_of_steps ; i++) {
		data->result += 1.0/(i*4.0 + 1.0);
		data->result -= 1.0/(i*4.0 + 3.0);
	}

	return NULL;
}

int main(int argc, char *argv[]) {

	if(argc < 2 || argc > 3){
		printf("Usage: %s num_of_threads [num_of_iters]", argv[0]);
		return 1;
	}

	int ret;
	int num_of_threads;
	int num_of_steps = 20;

	num_of_threads = atoi(argv[1]);

	if(argc == 3){
		num_of_steps = atoi(argv[2]);
	}

	double pi = 0;
	struct data *threads = malloc(sizeof(struct data)*num_of_threads);

	for(int i = 0; i < num_of_threads; ++i){
		threads[i].start = i * num_of_steps;
		threads[i].num_of_steps = num_of_steps;
		threads[i].result = 0;
		ret = pthread_create(&threads[i].tid, NULL, part_sum, &threads[i]);
		threads[i].valid = 1;

		if(ret != 0){
			threads[i].valid = 0;
			fprintf(stderr, "Error creating thread: %s", strerror_r(ret));
		}
	}


	for(int i = 0; i < num_of_threads; ++i){
		if(threads[i].valid == 1){
			pthread_join(threads[i].tid, NULL);
		}
		pi += threads[i].result;
	}

	free(threads);

	pi = pi * 4.0;
	printf("pi done - %.15g \n", pi);    

	return (EXIT_SUCCESS);
}
