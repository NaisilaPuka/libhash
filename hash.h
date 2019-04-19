/*
PROJECT MADE BY:

NAISILA PUKA 21600336 SECTION 2
KUNDUZ EFRONOVA 21600469 SECTION 2

30 MARCH 2019
*/
#ifndef HASH_H
#define HASH_H

#include <pthread.h>

#define MIN_N 100
#define MAX_N 1000
#define MIN_M 10
#define MAX_M 1000

struct hash_table
{
	//structure to keep linked-list elements
	struct node
	{
		int key;
		void * value;
		struct node * next;
	};

	//each bucket is a linked list
	struct node ** table;

	//table size
	int N;

	//number of locks/regions
	int K;

	//size of a region
	int M;

	//number of elements currently found in the table
	int currentSize;

	//locks
	pthread_mutex_t* locks;
};

typedef struct hash_table HashTable; 

HashTable *hash_init (int N, int K);
int hash_insert (HashTable *hp, int k, void* v);
int hash_delete (HashTable *hp, int k);
int hash_update (HashTable *hp, int, void *v); 
int hash_get (HashTable *hp, int k, void **vp);
int hash_destroy (HashTable *hp); 


#endif /* HASH_H */
