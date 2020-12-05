#include "priority_queue.h"
#include <stdio.h>
#include <stdlib.h>

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

static bool equalIntsGeneric(PQElementPriority n1, PQElementPriority n2) {
    return *(int *) n1 == *(int *) n2;
}

int main() {

    PriorityQueue pq = pqCreate(copyIntGeneric, freeIntGeneric, equalIntsGeneric, copyIntGeneric, freeIntGeneric, compareIntsGeneric);

    int one = 1, two = 2, three = 3;
    int onePriority = 10, twoPriority = 10, threePriority = 5;
    
    pqInsert(pq, &two, &twoPriority);
    pqInsert(pq, &one, &onePriority);
    pqInsert(pq, &three, &threePriority);
    printf("queue size: %d\n", pqGetSize(pq));

    int element1 = *((int *)pqGetFirst(pq));
    printf("first element in the pq: %d\n", element1);

    pqRemove(pq);
    int element2 = *((int *)pqGetFirst(pq));
    printf("first element in the pq after pqRemove(): %d\n", element2);

    pqRemove(pq);
    printf("queue size: %d\n", pqGetSize(pq));

    if(pqGetFirst(pq) == NULL) printf("null 1\n");

    pqDestroy(pq);

    return 0;
}
//testing