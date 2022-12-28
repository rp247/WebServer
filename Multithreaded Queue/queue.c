#include "queue.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

sem_t lock, em_spots, readable;

// a circular bounded buffer/queue
struct queue {
    uint64_t head; // head index
    uint64_t tail; // tail index
    uint64_t cur_size; // size of queue
    uint64_t capacity; // capacity of queue
    void **elems; // pointer to array that holds items
};

// constructor
queue_t *queue_new(int size) {
    queue_t *q = (queue_t *) malloc(sizeof(queue_t));

    if (!q)
        return NULL; // if mem can't be allocated

    q->head = 0;
    q->tail = 0;
    q->cur_size = 0;
    q->capacity = size;
    q->elems = (void **) calloc(size, sizeof(void *));

    // if mem can't be allocated for items
    if (!q->elems) {
        free(q);
        q = NULL;
    }

    sem_init(&lock, 0, 1);
    sem_init(&em_spots, 0, size);
    sem_init(&readable, 0, 0);

    return q;
}

// destructor
void queue_delete(queue_t **q) {
    if (q && *q && (*q)->elems) {
        free((*q)->elems);
        free(*q);
        *q = NULL;
    }
    sem_destroy(&lock);
    sem_destroy(&em_spots);
    sem_destroy(&readable);
    return;
}

// enqueue
bool queue_push(queue_t *q, void *elem) {
    if (!q) return false;
    sem_wait(&em_spots); // wait till empty spots
    sem_wait(&lock); // critical section

    q->elems[q->tail] = elem;
    q->tail = (q->tail + 1) % q->capacity;
    q->cur_size++;

    sem_post(&lock);
    sem_post(&readable);

    return true;
}

// dequeue
bool queue_pop(queue_t *q, void **elem) {
    if (!q || !elem) return false;
    sem_wait(&readable); // wait till readable
    sem_wait(&lock); // critical section

    *elem = q->elems[q->head];
    q->head = (q->head + 1) % q->capacity;
    q->cur_size--;

    sem_post(&lock);
    sem_post(&em_spots);

    return true;
}
