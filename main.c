#include "priority_queue.h"
#include <stdio.h>

static PQElementPriority copyIntGeneric(PQElementPriority n) {
    if (!n) {
        return NULL;
    }
    int *copy = malloc(sizeof(*copy));
    if (!copy) {
        return NULL;
    }
    *copy = *(int *) n;
    return copy;
}
static void freeIntGeneric(PQElementPriority n) {
    free(n);
}
static int compareIntsGeneric(PQElementPriority n1, PQElementPriority n2) {
    return (*(int *) n1 - *(int *) n2);
}






int main() {

    // PriorityQueue pq = pqCreate();

    printf("test\n");

    return 0;
}
//testing