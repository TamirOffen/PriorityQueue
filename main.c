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
    int onePriority = 11, twoPriority = 10, threePriority = 5;
   
    for(int i = 0; i < 20; i++) {
        pqInsert(pq, &one, &onePriority);
    }
    pqInsert(pq, &two, &twoPriority);
    pqInsert(pq, &three, &threePriority);

    printf("element found: %d\n", pqContains(pq, &one));
    
    PQ_FOREACH(int*, i, pq) {
        printf("%d\n", *i);
    }
    printf("\n");
    
    pqClear(pq);
    pqInsert(pq, &two, &twoPriority);
    pqInsert(pq, &three, &threePriority);

    printf("element found: %d\n", pqContains(pq, &one));
    PQ_FOREACH(int*, i, pq) {
        printf("%d\n", *i);
    }

    pqInsert(pq, &one, &onePriority);
    printf("element found: %d\n", pqContains(pq, &one));

    pqDestroy(pq);


    return 0;
}
//testing