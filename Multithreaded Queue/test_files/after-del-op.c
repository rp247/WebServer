#include "queue.h"

#include <stdbool.h>

// tests delete and pop not possible after deleting queue
int main() {
    queue_t *q = queue_new(10);
    queue_delete(&q);

    void *elem;
    bool b = queue_pop(q, &elem);
    bool b2 = queue_push(q, &elem);
    return (!b && !b2) ? 0 : 1;
}
