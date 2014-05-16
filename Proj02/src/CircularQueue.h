/*
	SOPE Project-02

	Authors:
		- Anaís Dias
		- Maria João Marques

	2014 FEUP
*/
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

//------------------------------------------------------------------------------------------ 
// Type of the circular queue elements  
typedef unsigned long QueueElem; 

//------------------------------------------------------------------------------------------ 
// Struct for representing a "circular queue" 
// Space for the queue elements will be allocated dinamically by queue_init() 

typedef struct { 
 QueueElem *v; // pointer to the queue buffer
 unsigned int capacity; // queue capacity 
 unsigned int first; // head of the queue 
 unsigned int last; // tail of the queue 
 sem_t empty; // semaphores and mutex for implementing the 
 sem_t full; // producer-consumer paradigm
 pthread_mutex_t mutex; 
} CircularQueue; 

//------------------------------------------------------------------------------------------ 
// Allocates space for circular queue 'q' having 'capacity' number of elements 
// Initializes semaphores & mutex needed to implement the producer-consumer paradigm 
// Initializes indexes of the head and tail of the queue
int queue_init(CircularQueue **q, unsigned int capacity) { 
	*q = (CircularQueue *) malloc(sizeof(CircularQueue)); 

	if (sem_init(&((*q)->empty), 0, capacity) != 0)
		return -1; 
	if (sem_init(&((*q)->full), 0, 0) != 0)
		return -1; 
	if (pthread_mutex_init(&((*q)->mutex), NULL) != 0)
		return -1; 

	(*q)->v = (QueueElem *) malloc(capacity * sizeof(QueueElem)); 
	(*q)->capacity = capacity; 
	(*q)->first = 0; 
	(*q)->last = 0; 

	return 0;
} 

//------------------------------------------------------------------------------------------ 
// Inserts 'value' at the tail of queue 'q' 
void queue_put(CircularQueue *q, QueueElem value) { 
	sem_wait(&q->empty);
	pthread_mutex_lock(&q->mutex);

	q->v[q->last] = value;

	q->last++;
	if(q->last >= q->capacity)
		q->last = 0;

	pthread_mutex_unlock(&q->mutex);
	sem_post(&q->full);
} 

//------------------------------------------------------------------------------------------ 
// Removes element at the head of queue 'q' and returns its 'value' 
QueueElem queue_get(CircularQueue *q) {
	QueueElem head;

	sem_wait(&q->full);
	pthread_mutex_lock(&q->mutex);

	head = q->v[q->first];

	q->first++;
	if(q->first >= q->capacity)
		q->first = 0;

	pthread_mutex_unlock(&q->mutex);
	sem_post(&q->empty);

	return head;
} 

//------------------------------------------------------------------------------------------ 
// Frees space allocated for the queue elements and auxiliary management data 
// Must be called when the queue is no more needed 
void queue_destroy(CircularQueue *q) {
	free(q->v);
	free(q);
}
