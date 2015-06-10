#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int time_to_stop = 0;

struct data{
	pthread_t tid;
	int valid;
	int start_index;
	int step_size;
	pthread_rwlock_t *rwlock;
	long double result;
};

void *part_sum(void *data_p){
	struct data *data = (struct data*)data_p;
	
	pthread_rwlock_rdlock(data->rwlock);

	int i, j;
	for (j = 0, i = data->start_index; ; i += data->step_size, j++) {
		if (((j % 100000) == 0)&&(time_to_stop)){
			pthread_rwlock_unlock(data->rwlock);
			break;
		}

		data->result += 1.0/i;
		data->result -= 1.0/(i + 2.0);
	}

	return NULL;
}

void stop(int s){
	time_to_stop = 1;
}

int main(int argc, char *argv[]) {

	int ret;
	int i;

	if(argc != 2){
		fprintf(stderr, "Usage: %s num_of_threads", argv[0]);
		return 1;
	}

	int num_of_threads = 0;
	int num_of_threads_created = 0;

	num_of_threads = atoi(argv[1]);
	
	if ((num_of_threads < 1) || (num_of_threads > 65535)){
		fprintf(stderr, "Invalid number of threads: %d", num_of_threads);
		return 1;
	}

	long double pi = 0;
	struct data threads[num_of_threads];
	
	pthread_rwlock_t rwlock;
	pthread_rwlock_init(&rwlock, NULL);
	pthread_rwlock_wrlock(&rwlock);

	for(i = 0; i < num_of_threads; ++i){
		num_of_threads_created++;
		threads[i].result = 0;
		threads[i].valid = 1;
		threads[i].rwlock = &rwlock;
		ret = pthread_create(&threads[i].tid, NULL, &part_sum, &threads[i]);
		if(ret != 0){
			num_of_threads_created--;
			threads[i].valid = 0;
			fprintf(stderr, "Error creating thread: %s", strerror(ret));
		}
	}

	int current_start_index = 1;
	for(i = 0; i < num_of_threads; ++i){
		if(threads[i].valid == 1){
			threads[i].start_index = current_start_index;
			current_start_index += 4;
			threads[i].step_size = 4 * num_of_threads_created;
		}
	}	

	signal(SIGINT, stop);
	printf("Start computing\n");
	printf("To stop computing send SIGINT\n");

	pthread_rwlock_unlock(&rwlock);

	for(i = 0; i < num_of_threads; ++i){
		if(threads[i].valid == 1){
			pthread_join(threads[i].tid, NULL);
		}
		pi += threads[i].result;
	}

	pthread_rwlock_destroy(&rwlock);

	pi = pi * 4.0;
	printf("pi done - %.15Lg\nDifference from actual: %Lg\n", pi, M_PI - pi);    

	return (EXIT_SUCCESS);
}
