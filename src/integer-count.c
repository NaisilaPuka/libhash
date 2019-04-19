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
#include "pthread.h"
#include "hash.h"

#define MAXCHAR 50000

HashTable *ht1; // space allocated inside library
pthread_mutex_t motherOfLocks; //MAIN LOCK
void *runner(void *param);

int main(int argc, char **argv)
{
	if(argc < 2)
	{
		printf("Error! Not enough arguments!\n");
		return 0;
	}

	pthread_t * thread_id;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	struct node ** list;

	pthread_mutex_init(&motherOfLocks, NULL);

	//get number of text files
	int c = atoi(argv[1]);

	if(argc < (c + 2))
	{
		printf("Error! Not enough arguments!\n");
		return 0;
	}

	//initialize hash table
	ht1  = hash_init (1000, 1);

	//allocate threads
	thread_id = malloc(c * sizeof(pthread_t));

	//create a thread for each text file
	for(int i = 0; i < c; i++)
	{
		pthread_create(&(thread_id[i]),&attr,runner,argv[i + 2]);
	}

	//wait for all threads to finish
	for(int i = 0; i < c; i++)
	{
		pthread_join(thread_id[i],NULL);
	}

	list = (struct node**)malloc(ht1->currentSize * sizeof(struct node));
	int index = 0;
	struct node * tmp;

	//put all nodes in a Node array
	for(int i = 0; i < 1000; i++)
	{
		tmp = ht1->table[i];
		while(tmp != NULL)
		{
			list[index] = ht1->table[i];
			index += 1;
			tmp = tmp->next;
		}
	}

	struct node * a;

	//sort the list of Nodes based on key
	for (int i = 0; i < ht1->currentSize; ++i) 
	{

		for (int j = i + 1; j < ht1->currentSize; ++j)
		{

			if (list[i]->key > list[j]->key) 
			{
				a =  list[i];
				list[i] = list[j];
				list[j] = a;
			}

		}

	}

	//write each key and the number of its occurrences in the specified text file	
	FILE * fp;
	int i;
   /* open the file for writing*/
	fp = fopen (argv[c+2],"w");
	
   /* write 10 lines of text into the file stream*/
	for(i = 0; i < ht1->currentSize;i++){
		fprintf (fp, "%d: ", list[i]->key);
		fprintf (fp, "%d\n", (int *)(list[i]->value));
	}
	
   /* close the file*/  
	fclose (fp);
	return 0;
}

/* The thread will begin control in this function */ 
void *runner(void *param)
{
	FILE *fp;
	int current;
	void* value;
	int casted;

	//open specified text file
	fp = fopen((char*)param, "r");
	if (fp == NULL) {
		printf("Couldn't read file %s\n", (char*)param);
		pthread_exit(0);
	}

	char input[MAXCHAR];

	//read integers from file and update hash table accordingly
	while (fgets(input, MAXCHAR, fp) != NULL)
	{
		input[strcspn(input, "\n")] = 0;
		current = atoi(input);
		if(hash_insert(ht1, current, (void *)1) == -1)
		{
			pthread_mutex_lock(&motherOfLocks);
			if(hash_get(ht1, current, &value) == 0)
			{
				casted = (int)(value);
				casted += 1;
				hash_update(ht1, current, (void *)casted);
			}
			pthread_mutex_unlock(&motherOfLocks);
		}
	}
	pthread_exit(0);
}

