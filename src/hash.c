/*
PROJECT MADE BY:

NAISILA PUKA 21600336 SECTION 2
KUNDUZ EFRONOVA 21600469 SECTION 2

30 MARCH 2019
*/
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"

HashTable *hash_init (int N, int K)
{
	HashTable *ht;

	if (N < MIN_N || N > MAX_N) {
		printf("Error! Value of N should be between 100 and 1000.\n");
		return NULL;
	}

	if(K > N)
	{
		printf("Error! Value of K should not be greater than N.\n");
		return NULL;
	}

	if(N % K != 0) {
		printf("Error! N should be a multiple of K.\n");
		return NULL;
	}

	ht = malloc(sizeof(HashTable));
	if (ht == NULL) {
		printf("HASHTABLE ALLOCATION FAILED!!!\n");
		return (NULL);
	}

	ht->M = N/K;

	if(ht->M < MIN_M || ht->M > MAX_M) {
		printf("Error! Value of K should be between 1 and 100.\n");
		return NULL;
	}

	ht->table = (struct node**)malloc(N * sizeof(struct node));
	if (ht->table == NULL) {
		printf("HASHTABLE BUCKETS ALLOCATION FAILED!!!\n");
		return (NULL);
	}

	//initialize K locks
	ht->locks = malloc(K * sizeof(pthread_mutex_t));
	for(int j = 0; j < K; j++)
		pthread_mutex_init(&((ht->locks)[j]),NULL);

	//initialize each bucket to null
	for (int i = 0; i < N; i++)
	{
		ht->table[i] = NULL;
	}

	ht->N = N;
	ht->K = K;
	ht->currentSize = 0;
	printf ("Hash Table successfully created!\n"); 
	return ht;
}

int hash_insert (HashTable *hp, int k, void *v)
{
	if(k < 1)
	{
		printf("ERROR! Key should be a positive integer!\n");
		return -1;
	}

	if (hp == NULL) {
		printf("ERROR! Hash Table is NULL!\n");
		return -1;
	}

	struct node *node;
	node = malloc(sizeof(struct node));
	if (node == NULL) {
		printf("ERROR! New item couldn't be allocated!\n");
		return (-1);
	}

	node->key = k;
	node->value = v;

	//calculate the bucket to which the key belongs to
	int bucket = k % (hp->N);

	int region = bucket / (hp->M);

	pthread_mutex_lock(&((hp->locks)[region]));

	if (hp->table[bucket] != NULL) {
		struct node *tmp = hp->table[bucket];
		while (tmp != NULL) {
			if (tmp->key == node->key) {
				break;
			}
			tmp = tmp->next;
		}
		if (tmp == NULL) {
			//add new node at the end of LL
			node->next = hp->table[bucket];
			hp->table[bucket] = node;
		} else {
			pthread_mutex_unlock(&((hp->locks)[region]));
			// If key already present, does nothing and returns -1.
			return(-1);
		}
	} else {
		//head node
		node->next = NULL;
		hp->table[bucket] = node;
	}

	hp->currentSize = hp->currentSize + 1;

	pthread_mutex_unlock(&((hp->locks)[region]));
	return (0);
}

int hash_delete (HashTable *hp, int k) {
	if (hp == NULL) {
		printf("ERROR! Hash Table is NULL!\n");
		return -1;

	}

	if(k < 1){
		printf("ERROR! Key should be a positive integer!\n");
		return -1;
	}

	//find to which bucket this key belongs to
	int bucket = k % hp->N;
	int region = bucket / hp->M;

	pthread_mutex_lock(&((hp->locks)[region]));

	struct node * tmp = hp->table[bucket];
	struct node * tmp2 = hp->table[bucket];
	
	while (tmp != NULL) {
		if (tmp->key == k) {
			break;
		}
		tmp2 = tmp;
		tmp = tmp->next;
	}

	if(tmp == NULL)
	{
		pthread_mutex_unlock(&((hp->locks)[region]));
		printf("Key doesn't exist!\n");
		return (-1);
	}

	//delete key
	if(tmp == tmp2){
		hp->table[bucket] = NULL;
		free(tmp); 
	} 
	else
	{
		tmp2->next = tmp->next;
		free(tmp);
	}

	//decrease the number of elements in the list
	hp->currentSize = hp->currentSize - 1;

	pthread_mutex_unlock(&((hp->locks)[region]));
	return (0); 
}

int hash_update (HashTable *hp, int k, void *v)
{
	if (hp == NULL) {
		printf("ERROR! Hash Table is NULL!\n");
		return -1;
	}
	if(k < 1){
		printf("ERROR! Key should be a positive integer!\n");
		return -1;
	}

	//find the bucket to which the key belongs to
	int bucket = k % hp->N;
	int region = bucket / (hp->M);
	pthread_mutex_lock(&((hp->locks)[region]));
	struct node *tmp = hp->table[bucket];
	
	while (tmp != NULL) {
		if (tmp->key == k) {
			break;
		}
		tmp = tmp->next;
	}
	if (tmp == NULL) {
		pthread_mutex_unlock(&((hp->locks)[region]));
		printf("Key doesn't exist!\n");
		return (-1);
	} else {
		//change the value of that particular key
		tmp->value = v;
		
	}

	pthread_mutex_unlock(&((hp->locks)[region]));
	return (0);
}

int hash_get (HashTable *hp, int k, void **vp)
{
	if (hp == NULL) {
		printf("ERROR! Hash Table is NULL!\n");
		return -1;
	}

	if(k < 1){
		printf("ERROR! Key should be a positive integer!\n");
		return -1;
	}

	//find the bucket to which the key belongs to
	int bucket = k % hp->N;
	int region = bucket / hp->M;
	pthread_mutex_lock(&((hp->locks)[region]));

	struct node * tmp = hp->table[bucket];

	while (tmp != NULL) {
		if (tmp->key == k) {
			break;
		}
		tmp = tmp->next;
	}

	if (tmp == NULL) {
		pthread_mutex_unlock(&((hp->locks)[region]));
		printf("Key doesn't exist!\n");
		return -1;
	}
	else
	{
		//get the value of the node to the argument
		(*vp) = tmp->value;
	}

	pthread_mutex_unlock(&((hp->locks)[region]));
	return (0);
}

int hash_destroy (HashTable *hp)
{
	struct node *tmp;

	if (hp == NULL) {
		printf ("Destroy Job done!\n");
		return (0);
	}

	for(int j = 0; j < hp->K; j++)
	{
		pthread_mutex_lock(&((hp->locks)[j]));
	}

	for (int i = 0; i < hp->N; i++) {
		if (hp->table[i] != NULL) {
            //Traverse all the list and free the nodes
			while(hp->table[i] != NULL) {
				tmp = hp->table[i]->next;
				hp->table[i]->value=NULL;
				free(hp->table[i]);
				hp->table[i] = tmp;
			}
		}
	}
	free(hp->table);

	//destroy all locks

	for(int j = 0; j < hp->K; j++)
	{
		pthread_mutex_destroy(&((hp->locks)[j]));
	}

	//free the table
	free(hp->locks);
	free(hp);
	return (0); 
}
