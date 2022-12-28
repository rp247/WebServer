#include "queue.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

sem_t pop_lock;
bool got_a = false, got_b = false, got_c = false, got_d = false, fin = true;

void *push1(void *q) {
    char a = 'a'; char b = 'b';
    queue_push(q, (void *) &a);
    queue_push(q, (void *) &b);
    return NULL;
}

void *push2(void *q) {
    char c = 'c'; char d = 'd';
    queue_push(q, (void *) &c);
    queue_push(q, (void *) &d);
    return NULL;
}

void *pop1(void *q) {
    sem_wait(&pop_lock);
    void *elem;
    queue_pop(q, &elem);
    char *c = (char *) elem;

    switch (*c) {
        case 'a':
            if (got_b) {fin = false; break;}
            got_a = true;
            break;
        case 'b':
            if (!got_a) fin = false;
            got_b = true;
            break;
        case 'c':
            if (got_d) {fin = false; break;};
            got_c = true;
            break;
        case 'd':
            if (!got_c) fin = false;
            got_d = true;
            break;
    }
    sem_post(&pop_lock);
    
    return NULL;
}

void *pop2(void *q) {
    sem_wait(&pop_lock);
    void *elem;
    queue_pop(q, &elem);
    char *c = (char *) elem;

    switch (*c) {
        case 'a':
            if (got_b) {fin = false; break;}
            got_a = true;
            break;
        case 'b':
            if (!got_a) fin = false;
            got_b = true;
            break;
        case 'c':
            if (got_d) {fin = false; break;};
            got_c = true;
            break;
        case 'd':
            if (!got_c) fin = false;
            got_d = true;
            break;
    }
    sem_post(&pop_lock);
    
    return NULL;
}

void *pop3(void *q) {
    sem_wait(&pop_lock);
    void *elem;
    queue_pop(q, &elem);
    char *c = (char *) elem;

    switch (*c) {
        case 'a':
            if (got_b) {fin = false; break;}
            got_a = true;
            break;
        case 'b':
            if (!got_a) fin = false;
            got_b = true;
            break;
        case 'c':
            if (got_d) {fin = false; break;};
            got_c = true;
            break;
        case 'd':
            if (!got_c) fin = false;
            got_d = true;
            break;
    }
    sem_post(&pop_lock);
    
    return NULL;
}

void *pop4(void *q) {
    sem_wait(&pop_lock);
    void *elem;
    queue_pop(q, &elem);
    char *c = (char *) elem;

    switch (*c) {
        case 'a':
            if (got_b) {fin = false; break;}
            got_a = true;
            break;
        case 'b':
            if (!got_a) fin = false;
            got_b = true;
            break;
        case 'c':
            if (got_d) {fin = false; break;};
            got_c = true;
            break;
        case 'd':
            if (!got_c) fin = false;
            got_d = true;
            break;
    }
    sem_post(&pop_lock);
    
    return NULL;
}

// tests thread order correctness
int main() {
    sem_init(&pop_lock, 0, 1);
    queue_t *q = queue_new(4);

    pthread_t pu1, pu2, po1, po2, po3, po4;

    pthread_create(&pu1, NULL, &push1, (void *) q);
    pthread_create(&pu2, NULL, &push2, (void *) q);
    pthread_create(&po1, NULL, &pop1, (void *) q);
    pthread_create(&po2, NULL, &pop2, (void *) q);
    pthread_create(&po3, NULL, &pop3, (void *) q);
    pthread_create(&po4, NULL, &pop4, (void *) q);

    pthread_join(pu1, NULL);
    pthread_join(pu2, NULL);
    pthread_join(po1, NULL);
    pthread_join(po2, NULL);
    pthread_join(po3, NULL);
    pthread_join(po4, NULL);

    queue_delete(&q);
    sem_destroy(&pop_lock);

    // checks order
    if (!fin) return 1;

    return 0;
}
