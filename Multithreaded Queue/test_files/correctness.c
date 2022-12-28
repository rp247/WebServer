#include "queue.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define ALPHA 26

sem_t arr_lock;
char *arr;
int push_ind = -1;
bool got_F = false;

// push a..z
void *push1(void *q) {
    for (int i = 0; i < ALPHA; i++) {
        sem_wait(&arr_lock);
        push_ind++;
        arr[push_ind] = i+97;
        queue_push((queue_t *) q, (void *) &arr[push_ind]);
        sem_post(&arr_lock);
    }
    return NULL;
}

// push A..Z
void *push2(void *q) {
    for (int i = 0; i < ALPHA; i++) {
        sem_wait(&arr_lock);
        push_ind++;
        arr[push_ind] = i+65;
        queue_push((queue_t *) q, (void *) &arr[push_ind]);
        sem_post(&arr_lock);
    }
    return NULL;
}

// tests for correctness
int main() {
    arr = (char *) calloc(ALPHA*2, sizeof(char));
    sem_init(&arr_lock, 0, 1);
    queue_t *q = queue_new(ALPHA*2);

    pthread_t pu1, pu2;

    pthread_create(&pu1, NULL, &push1, (void *) q);
    pthread_create(&pu2, NULL, &push2, (void *) q);

    pthread_join(pu1, NULL);
    pthread_join(pu2, NULL);

    char *in; void *elem;
    for (int i = 0; i < ALPHA*2; i++) {
        queue_pop((queue_t *) q, &elem);
        in = (char *) elem;
        if (*in == 'F') {got_F = true; break;}
    }

    queue_delete(&q);
    sem_destroy(&arr_lock);

    return got_F ? 0 : 1;
}
