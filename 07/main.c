#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#define __USE_GNU
#include <math.h>
#undef __USE_GNU

#define TRUE 1
#define FALSE 0

struct data{
	pthread_t tid;
	int is_valid;
	int start_index;
	int steps_count;
	long double result;
};


void *part_sum(void *str_data_arg){
	struct data *data = str_data_arg;

	for (int i = data->start_index; i < data->steps_count ; i++) {
		data->result += 1.0/(i*4.0 + 1.0);
		data->result -= 1.0/(i*4.0 + 3.0);
	}

	return NULL;
}

int main(int argc, char *argv[]) {

	if(argc < 2 || argc > 3){
		printf("Usage: %s threads_count [num_of_iters]", argv[0]);
		return 1;
	}

	int ret;
	int threads_count;
	int steps_count = 20;

	threads_count = atoi(argv[1]);

	if(argc == 3){
		steps_count = atoi(argv[2]);
	}

	long double PI_MULTIPLIER = 4.0;
	long double pi = 0;
	struct data *threads = malloc(sizeof(struct data)*threads_count);

	for(int i = 0; i < threads_count; ++i){
		threads[i].start_index = i * steps_count;
		threads[i].steps_count = steps_count;
		threads[i].result = 0.0;

		threads[i].is_valid = TRUE;
		ret = pthread_create(&threads[i].tid, NULL, part_sum, &threads[i]);

		if(ret != 0){
			threads[i].is_valid = FALSE;
			fprintf(stderr, "Error creating thread: %s", strerror(ret));
		}
	}


	for(int i = 0; i < threads_count; ++i){
		if(threads[i].is_valid == 1){
			pthread_join(threads[i].tid, NULL);
		}
		pi += threads[i].result;
	}

	free(threads);

	pi = pi * PI_MULTIPLIER;
	printf("pi\t\t%.15Lg\nM_PIl-pi:\t%Lg\n", pi, M_PIl - pi);    

	return (EXIT_SUCCESS);
}
