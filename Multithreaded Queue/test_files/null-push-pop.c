#include "queue.h"

#include <stdbool.h>
#include <stdio.h>

// tests pushing and popping null elements
int main() {
    queue_t *q = queue_new(10);

    void *elem = 0;
    
    bool b2 = queue_push(q, &elem);
    bool b = queue_pop(q, NULL);
    queue_delete(&q);

    return (!b && b2) ? 0 : 1;
}
