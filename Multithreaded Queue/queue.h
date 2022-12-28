#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <stdbool.h>

// a circular bounded buffer/queue
typedef struct queue queue_t;

// constructor 
queue_t *queue_new(int size);

// destructor
void queue_delete(queue_t **q);

// enqueue
bool queue_push(queue_t *q, void *elem);

// dequeue
bool queue_pop(queue_t *q, void **elem);

#endif
