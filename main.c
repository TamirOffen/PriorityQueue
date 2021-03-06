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

    int one = 1, two = 2;

    for(int i = 0; i < 5; i++) {
        pqInsert(pq, &one, &one);
    }
    for(int i = 0; i < 5; i++) {
        pqInsert(pq, &two, &two);
    }

    PQ_FOREACH(int*, i, pq) {
        printf("%d\n", *i);
    }

    if(pqContains(pq, &one)) {
        printf("good\n");
    }


    pqDestroy(pq);

    return 0;
}
//testing