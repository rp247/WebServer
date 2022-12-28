#include "queue.h"

#include <stdbool.h>
#include <stdio.h>

// tests deleting the same queue twice, should not seg fault
int main() {
    queue_t *q = queue_new(10);
    queue_delete(&q);
    queue_delete(&q);

    return 0;
}
