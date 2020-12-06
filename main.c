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
    pqInsert(pq, &three, &threePriority);
    
    PQ_FOREACH(int*, currentNumber, pq) {
        printf("%d\n", *currentNumber);
    }
    printf("\n");

    
    pqInsert(pq, &one, &onePriority);
    PQ_FOREACH(int*, currentNumber, pq) {
        printf("%d\n", *currentNumber);
    }

    pqDestroy(pq);

    return 0;
}
//testing