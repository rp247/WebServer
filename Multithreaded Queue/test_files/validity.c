#include "queue.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define ALPHA 10

sem_t arr_lock, pop_lock;
bool *arr; int *val;
int push_ind = -1;
bool all_good = true;

void *push1(void *q) {
    for (int i = 0; i < ALPHA; i++) {
        sem_wait(&arr_lock);
        push_ind++;
        arr[i] = true;
        val[push_ind] = i;
        queue_push((queue_t *) q, (void *) &val[push_ind]);
        sem_post(&arr_lock);
    }
    return NULL;
}

void *push2(void *q) {
    for (int i = ALPHA+10; i < ALPHA+20; i++) {
        sem_wait(&arr_lock);
        push_ind++;
        arr[i] = true;
        val[push_ind] = i;
        queue_push((queue_t *) q, (void *) &val[push_ind]);
        sem_post(&arr_lock);
    }
    return NULL;
}

void *pop1(void *q) {
    int *in; void *elem;
    for (int i = 0; i < ALPHA/2; i++) {
        sem_wait(&pop_lock);
        queue_pop((queue_t *) q, &elem);
        in = (int *) elem;
        all_good = arr[*in];
        sem_post(&pop_lock);
    }
    return NULL;
}

void *pop2(void *q) {
    int *in; void *elem;
    for (int i = ALPHA/2; i < ALPHA; i++) {
        sem_wait(&pop_lock);
        queue_pop((queue_t *) q, &elem);
        in = (int *) elem;
        all_good = arr[*in];
        sem_post(&pop_lock);
    }
    return NULL;
}

// tests for validity: if someting is popped, it had to be pushed first
int main() {
    arr = (bool *) calloc(ALPHA*3, sizeof(bool));
    val = (int *) calloc(ALPHA*3, sizeof(int));
    sem_init(&arr_lock, 0, 1);
    sem_init(&pop_lock, 0, 1);
    queue_t *q = queue_new(ALPHA*10);

    pthread_t pu1, pu2, po1, po2;

    for (int i = 0; i < ALPHA*2; i++) arr[i] = false;

    pthread_create(&pu1, NULL, &push1, (void *) q);
    pthread_create(&pu2, NULL, &push2, (void *) q);
    pthread_create(&po1, NULL, &pop1, (void *) q);
    pthread_create(&po2, NULL, &pop2, (void *) q);
    
    pthread_join(pu1, NULL);
    pthread_join(pu2, NULL);
    pthread_join(po1, NULL);
    pthread_join(po2, NULL);

    queue_delete(&q);
    free(arr);
    free(val);
    sem_destroy(&arr_lock); sem_destroy(&pop_lock);

    return all_good ? 0 : 1;
}
