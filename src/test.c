/*
PROJECT MADE BY:

NAISILA PUKA 21600336 SECTION 2
KUNDUZ EFRONOVA 21600469 SECTION 2

30 MARCH 2019
*/
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/stat.h> 
#include "pthread.h"
#include "hash.h"

HashTable *ht1; // space allocated inside library
int W; //number of operations per thread
void *runner(void *param);

int main(int argc, char **argv)
{
	int N, K, T;

	pthread_t * thread_id;
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	//get arguments
	N = atoi(argv[1]);
	K = atoi(argv[2]);
	T = atoi(argv[3]);
	W = atoi(argv[4]);

	//initialize table
	ht1  = hash_init (N, K);

	//allocate threads
	thread_id = malloc(T * sizeof(pthread_t));

	//start time
	struct timeval  tv0;
    gettimeofday(&tv0, NULL);

    //create threads
	for(int i = 0; i < T; i++)
	{
		pthread_create(&(thread_id[i]), &attr, runner, (void*)(i));
	}

	//wait for threads to finish
	for(int i = 0; i < T; i++)
	{
		pthread_join(thread_id[i], NULL);
	}

	//end time
    struct timeval  tv1;
    gettimeofday(&tv1, NULL);

    //get start and end in microseconds
    long long start = tv0.tv_sec*10000000 + tv0.tv_usec;
    long long end = tv1.tv_sec*10000000 + tv1.tv_usec;

    //get elapsed time of write
    printf("----------------------------------------------\n");
    printf("Doing insert operations ... \n");
    printf("For N = %d, K = %d, W = %d, T = %d:\nRequired time is %lld milliseconds.\n", N, K, W, T, (end-start));
    printf("----------------------------------------------\n");

	hash_destroy (ht1);
}

/* The thread will begin control in this function */ 
void *runner(void *param)
{
	//insert inside ht1
	for (int i = W * ((int)param) + 1 ; i < W * ((int)param) + W; i++) {
		hash_insert (ht1, i, (void *) i); 
	}

	pthread_exit(0);
}
