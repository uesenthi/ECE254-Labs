/**
 * @file:   sender.c
 * @brief:  send random points to a POSIX queue. Type 'q' to terminate. 
 * @date:   2014/06/25
 * NOTES:
 * To execute: <executable> <arg1>, where arg1 is a posix queue name.
 * EXAMPLE: ./sender.out /mailbox1_userid 
 * A valid posix queue name must start with a "/". 
 * Execute command "man mq_overview" for details.
 * Check /dev/mqueue to clean up the queue if sender fails to do so.
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <time.h>
#include "common.h"
#include "point.h"
#include <sys/wait.h>
#include <sys/types.h>
#include <math.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include <malloc.h>

static int consume_count;
static int prod_id;
static int cons_id;


struct circqueue {
   int front,rear;
   int capacity;
   int *array;
};

struct circqueue *init_circqueue(int size) {
   struct circqueue *q=malloc(sizeof(struct circqueue));
   if(!q)return NULL;
   q->capacity=size;
   q->front=-1;
   q->rear=-1;
   q->array=malloc(q->capacity*sizeof(int));
   if(!q->array)return NULL;
  return q;
}

void enqueue(struct circqueue *q,int x) {
  
      q->rear=(q->rear+1)%q->capacity;
      q->array[q->rear]=x;
      if(q->front==-1) {
         q->front=q->rear;
      }
}

int dequeue(struct circqueue *q) {
   int data=0;

      data=q->array[q->front];
      if(q->front==q->rear)
         q->front=q->rear=-1;
      else
         q->front=(q->front+1)%q->capacity;
   

   return data;
}

struct produce_params
{
	int N;
	int P;
	int ID;
	struct circqueue *q_pointer;
	pthread_mutex_t *mutex;
	sem_t *free_space;
	sem_t *avail_items;
};

struct consume_params
{
	int N;
	int C;
	int ID;
	struct circqueue *q_pointer;
	pthread_mutex_t *mutex;
	sem_t *avail_items;
	sem_t *free_space;
};

void* consumer(void* parameters)
{
	//Grab the consumer arguments and available consumerID
	struct consume_params* c = (struct consume_params*) parameters;
	int consumeID = cons_id;
	cons_id++;
	int number;

	
	while (consume_count < c->N) {
		//Can only continue if there are available items in the queue
		sem_wait(c->avail_items);
		pthread_mutex_lock(c->mutex);

		//Take from queue and increment slots free in the queue
		number = dequeue(c->q_pointer);
		sem_post (c->free_space);
		

		if (number == floor(sqrt(number))*floor(sqrt(number))){
			printf("%d %d %d \n", (int)consumeID, (int)number, (int)sqrt(number));	
		}
		consume_count++;

		//Something wrong right here, this makes multiple consumer work, but not multiple producers
		//If C > 1, disregard consume_count and faster threads leave earlier
		if (c-> C > 1 && number >= (c->N - c->C)) {
			pthread_mutex_unlock(c->mutex);
			break;
		}
		
		//Release lock
		pthread_mutex_unlock(c->mutex);
	}

	return 0;
}

void* producer(void* parameters)
{
	//Grab producer params and current available producerID
	struct produce_params* p = (struct produce_params*) parameters;
	int produceID = prod_id;
	prod_id++;
	do {
		//Wait until there is free space in queue
		sem_wait(p->free_space);
		pthread_mutex_lock(p->mutex);
		
		//Add to queue and post available items in the queue
		enqueue(p->q_pointer, produceID);
		produceID = produceID + p->P;
		sem_post (p->avail_items);
		
		pthread_mutex_unlock(p->mutex);	

	} while (produceID < p->N); //Quit once next produced item exceeds total count
	return 0;
}

int main(int argc, char *argv[])
{
	struct timeval duration;
	double timestart;
	double timeend;

	consume_count = 0;
	prod_id = 0;
	cons_id = 0;

	//Check for correct number of input arguments
	if ( argc !=5 ) {
		printf("Not enough input arguments\n");
		exit(1);
	}
	
	//Create a new circular queue of length B
	struct circqueue *queue = init_circqueue(atoi(argv[2]));

	pthread_mutex_t queue_mutex;
	pthread_mutex_init (&queue_mutex, NULL);
	
	//Initialize semaphore
	sem_t prod_items;
	sem_init(&prod_items, 0, 0); //can only consume items if there, initially no items
	sem_t cons_items;
	sem_init(&cons_items, 0, atoi(argv[2])); //Can only produce items if space, initially completely empty

	//Get the input parameters for consumer and producer 
	struct consume_params cons_arg;
	struct produce_params prod_arg;
	
	cons_arg.N = atoi(argv[1]); 
	cons_arg.C = atoi(argv[4]);
	cons_arg.q_pointer = queue;
	cons_arg.mutex = &queue_mutex;
	cons_arg.avail_items = &prod_items;
	cons_arg.free_space = &cons_items;

	prod_arg.N = atoi(argv[1]); 
	prod_arg.P = atoi(argv[3]);
	prod_arg.q_pointer = queue;
	prod_arg.mutex = &queue_mutex;
	prod_arg.free_space = &cons_items;
	prod_arg.avail_items = &prod_items;

	//Create arrays to hold producer/consumer threads
	pthread_t prod_threads[prod_arg.P];
	pthread_t cons_threads[cons_arg.C];
	int t = 0;

	srand(time(0));	

	//create multiple producer and consumer threads
	for (t = 0; t < prod_arg.P; ++t)
	{
		if(pthread_create(&prod_threads[t], NULL, &producer, &prod_arg)){
			printf("Error with pthread_create for producer");
			exit(-1);
		}

	}
	for (t = 0; t < cons_arg.C; t++)
	{
		cons_arg.ID = t;
		if(pthread_create(&cons_threads[t], NULL, &consumer, &cons_arg)){
			printf("Error with pthread_create for consumer");
			exit(-1);
		}
	}

	gettimeofday(&duration, NULL);
	timestart = duration.tv_sec +  duration.tv_usec/1000000.0;

	//Make sure all of the threads are done before getting the execution time
	for (t = 0; t < prod_arg.P; t++)
	{
		pthread_join(prod_threads[t], NULL);
	}
	for (t = 0; t < cons_arg.C; t++)
	{
		pthread_join(cons_threads[t], NULL);
	}

	//Print total system execution time
	gettimeofday(&duration, NULL);
	timeend = duration.tv_sec +  duration.tv_usec/1000000.0;
	
	printf("System execution time: %f seconds\n",(timeend-timestart));
	
	return 0;
}
