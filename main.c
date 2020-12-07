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
    
    PQ_FOREACH(int*, i, pq) {
        printf("%d\n", *i);
    }
    printf("\n");
    
    for(int i = 0; i < 20; i++) {
        pqRemove(pq);
    }
    pqRemove(pq);
    pqRemove(pq);

    PQ_FOREACH(int*, i, pq) {
        printf("%d\n", *i);
    }
    

    pqDestroy(pq);


    return 0;
}
//testing