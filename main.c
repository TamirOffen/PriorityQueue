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
 if (n != NULL) {
      free(n);
    }
    n = NULL;
}

static int compareIntsGeneric(PQElementPriority n1, PQElementPriority n2) {
    return (*(int *) n1 - *(int *) n2);
}

static bool equalIntsGeneric(PQElementPriority n1, PQElementPriority n2) {
    return *(int *) n1 == *(int *) n2;
}


int main() {

    PriorityQueue pq = pqCreate(copyIntGeneric, freeIntGeneric, equalIntsGeneric, copyIntGeneric, freeIntGeneric, compareIntsGeneric);

    int one = 1; //, two = 2, three = 3;
    int onePriority = 1; // twoPriority = 10, threePriority = 5;


    int elem = 5;
    int elem2 = 4;
    pqInsert(pq, &one, &onePriority);
    pqInsert(pq, &elem2, &elem2);
    pqInsert(pq, &one, &onePriority);
    pqInsert(pq, &elem2, &elem2);

    if(pqRemoveElement(pq, &elem) == PQ_ELEMENT_DOES_NOT_EXISTS) printf("ok1\n");


    pqInsert(pq, &elem, &elem);
    if(pqRemoveElement(pq, &elem) == PQ_SUCCESS) printf("ok2\n");
    if(pqRemoveElement(pq, &elem) == PQ_ELEMENT_DOES_NOT_EXISTS) printf("ok3\n");

    pqDestroy(pq);

    return 0;
}
//testing