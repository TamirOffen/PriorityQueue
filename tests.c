#include <stdlib.h>
#include "test_utilities.h"
#include "priority_queue.h"

#define PQ PriorityQueue


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


int *randInt() {
    int *num = malloc(sizeof(int));
    *num = rand() % 50000;
    return num;
}

PriorityQueue createPQ() {
    PriorityQueue pq = pqCreate(copyIntGeneric, freeIntGeneric, equalIntsGeneric, copyIntGeneric, freeIntGeneric,
                                compareIntsGeneric);
    return pq;
}

PriorityQueue getSingleElementPQ() {
    PriorityQueue pq = createPQ();
    int *element = randInt();
    int *priority = randInt();
    pqInsert(pq, element, priority);
    free(element);
    free(priority);
    return pq;
}

PriorityQueue getMultipleElementPQ() {
    PriorityQueue pq = createPQ();
    for (int i = 0; i < 10; i++) {
        int *element = randInt();
        int *priority = randInt();
        pqInsert(pq, element, priority);
        free(element);
        free(priority);
    }
    return pq;
}


/* ============= TESTING pqCreate ============= */
bool testPQCreateStandardTest() {
    bool result = true;
    PQ pq = createPQ();
    ASSERT_TEST(pq != NULL, destroy);

    destroy:
    pqDestroy(pq);
    return result;
}

/* ============= TESTING pqDestroy ============= */
bool testMemoryPQDestroyEmptyPQ() {
    PriorityQueue pq = createPQ();
    pqDestroy(pq);
    return true;
}

bool testMemoryPQDestroySingleElement() {
    PriorityQueue pq = getSingleElementPQ();
    pqDestroy(pq);
    return true;
}

bool testMemoryPQDestroyMultipleElement() {
    PriorityQueue pq = getMultipleElementPQ();
    pqDestroy(pq);

    return true;
}

/* ============= TESTING pqCopy ============= */
bool testPQCopySampleNULLArgument() {
    PQ new_pq = pqCopy(NULL);
    return new_pq == NULL;
}

bool testPQCopySingleElement() {
    bool result = true;
    PQ pq = getSingleElementPQ();
    PQ new_pq = pqCopy(pq);
    ASSERT_TEST(new_pq != NULL, destroy);
    ASSERT_TEST(compareIntsGeneric(pqGetFirst(new_pq), pqGetFirst(pq)) == 0, destroy);
    ASSERT_TEST(pqGetFirst(new_pq) != pqGetFirst(pq),
                destroy); // Copy should create a new copy so the pointers shouldn't be pointing to the same address

    destroy:
    pqDestroy(pq);
    pqDestroy(new_pq);
    return result;
}

bool testPQCopyMultipleElement() {
    bool result = true;
    PQ pq = getMultipleElementPQ();
    PQ new_pq = pqCopy(pq);
    ASSERT_TEST(new_pq != NULL, destroy);

    int *new_pq_current_elem = pqGetFirst(new_pq);
    PQ_FOREACH(int *, pq_current_elem, pq) {
        ASSERT_TEST(compareIntsGeneric(pq_current_elem, new_pq_current_elem) == 0, destroy);
        ASSERT_TEST(pq_current_elem != new_pq_current_elem,
                    destroy); // Copy should create a new copy so the pointers shouldn't be pointing to the same address
        new_pq_current_elem = pqGetNext(new_pq);
    }

    destroy:
    pqDestroy(pq);
    pqDestroy(new_pq);
    return result;
}

bool testPQCopyMultipleSameElementDifferentPriority() {
    // This test checks that if the same element was inserted more than once with a different priority it is copied correctly
    bool result = true;
    PQ pq = getSingleElementPQ();
    int *element = randInt();
    int *priority1 = randInt();
    int *priority2 = randInt();
    int *priority3 = randInt();

    pqInsert(pq, element, priority1);
    pqInsert(pq, element, priority2);
    pqInsert(pq, element, priority3);

    PQ new_pq = pqCopy(pq);
    ASSERT_TEST(new_pq != NULL, destroy);

    int *new_pq_current_elem = pqGetFirst(new_pq);
    PQ_FOREACH(int *, pq_current_elem, pq) {
        ASSERT_TEST(compareIntsGeneric(pq_current_elem, new_pq_current_elem) == 0, destroy);
        ASSERT_TEST(pq_current_elem != new_pq_current_elem,
                    destroy); // Copy should create a new copy so the pointers shouldn't be pointing to the same address
        new_pq_current_elem = pqGetNext(new_pq);
    }

    destroy:
    free(element);
    free(priority1);
    free(priority2);
    free(priority3);
    pqDestroy(pq);
    pqDestroy(new_pq);
    return result;
}

// TODO: ADD
bool testPQCopyMultipleSameElementSamePriority() {
    // This test checks that if the same element was inserted more than once with the same priority it is copied correctly
    bool result = true;
    PQ pq = getSingleElementPQ();
    int *element = randInt();
    int *priority1 = randInt();
    int *priority2 = randInt();

    pqInsert(pq, element, priority1);
    pqInsert(pq, element, priority1);
    pqInsert(pq, element, priority2);
    pqInsert(pq, element, priority1);

    PQ new_pq = pqCopy(pq);
    ASSERT_TEST(new_pq != NULL, destroy);

    int *new_pq_current_elem = pqGetFirst(new_pq);
    PQ_FOREACH(int *, pq_current_elem, pq) {
        ASSERT_TEST(compareIntsGeneric(pq_current_elem, new_pq_current_elem) == 0, destroy);
        ASSERT_TEST(pq_current_elem != new_pq_current_elem,
                    destroy); // Copy should create a new copy so the pointers shouldn't be pointing to the same address
        new_pq_current_elem = pqGetNext(new_pq);
    }

    destroy:
    free(element);
    free(priority1);
    free(priority2);
    pqDestroy(pq);
    pqDestroy(new_pq);
    return result;
}


/* ============= TESTING pqGetSize ============= */
//TODO: ADD
bool testPQGetSizeSampleNullArgument() {
    bool result = true;
    int size = pqGetSize(NULL);
    ASSERT_TEST(size == -1, destroy);
    destroy:
    return result;
}

void pqInsertRandom(PriorityQueue queue) {
    int *elem = randInt();
    int *prio = randInt();
    pqInsert(queue, elem, prio);
    free(elem);
    free(prio);
}
//TODO: ADD
bool testPQGetSizeStandardTest() {
    bool result = true;
    PQ pq = getSingleElementPQ();
    ASSERT_TEST(pqGetSize(pq) == 1, destroy);

    pqInsertRandom(pq);
    ASSERT_TEST(pqGetSize(pq) == 2, destroy);

    pqInsertRandom(pq);
    pqInsertRandom(pq);
    ASSERT_TEST(pqGetSize(pq) == 4, destroy);

    pqRemove(pq);
    ASSERT_TEST(pqGetSize(pq) == 3, destroy);

    destroy:
    pqDestroy(pq);
    return result;
}



/* ============= TESTING pqContains ============= */
// TODO: add
bool testPQContainsSampleNullArgument() {
    bool result = true;
    PQ pq = createPQ();

    ASSERT_TEST(pqContains(pq, NULL) == false, destroy);

    destroy:
    pqDestroy(pq);
    return result;

}

// TODO: add
bool testPQContainsStandardTest() {
    bool result = true;
    PQ pq = getMultipleElementPQ();

    int *elem = randInt();
    pqInsert(pq, elem, elem);
    ASSERT_TEST(pqContains(pq, elem) == true, destroy);

    destroy:
    free(elem);
    pqDestroy(pq);
    return result;
}

// TODO: add
bool testPQContainsSameElementMultipleTimesAlsoWithDifferentPriority() {
    bool result = true;
    PQ pq = getMultipleElementPQ();

    int *elem = randInt();
    int *prio = randInt();
    pqInsert(pq, elem, prio);
    pqInsert(pq, elem, elem);
    pqInsert(pq, elem, prio);

    ASSERT_TEST(pqContains(pq, elem) == true, destroy);

    destroy:
    free(elem);
    free(prio);
    pqDestroy(pq);
    return result;
}

// TODO: add
bool testPQContainsFalseAfterRemove() {
    bool result = true;
    PQ pq = createPQ();

    int *elem = randInt();
    pqInsert(pq, elem, elem);
    ASSERT_TEST(pqContains(pq, elem) == true, destroy);

    pqRemove(pq);
    ASSERT_TEST(pqContains(pq, elem) == false, destroy);

    destroy:
    free(elem);
    pqDestroy(pq);
    return result;
}



/* ============= TESTING pqInsert ============= */
bool testPQInsertSampleNULLArgument() {
    bool result = true;
    PQ pq = createPQ();

    int elem = 5;
    ASSERT_TEST(pqInsert(pq, &elem, NULL) == PQ_NULL_ARGUMENT, destroy);

    destroy:
    pqDestroy(pq);
    return result;
}

bool testPQInsertStandardTest() {
    bool result = true;
    PQ pq = createPQ();

    int elem = 10;
    ASSERT_TEST(pqInsert(pq, &elem, &elem) == PQ_SUCCESS, destroy);

    destroy:
    pqDestroy(pq);
    return result;
}

bool testPQInsertPutsNewElementsInCorrectPlace() {
    bool result = true;
    PQ pq = createPQ();

    int elem1 = 1;
    int prio1 = 2;
    pqInsert(pq, &elem1, &prio1);

    int elem2 = 2;
    int prio2 = 1;
    pqInsert(pq, &elem2, &prio2);

    int elem3 = 3;
    int prio3 = 3;
    pqInsert(pq, &elem3, &prio3);

    ASSERT_TEST((*(int *) pqGetFirst(pq)) == 3, destroy);
    ASSERT_TEST((*(int *) pqGetNext(pq)) == 1, destroy);
    ASSERT_TEST((*(int *) pqGetNext(pq)) == 2, destroy);

    destroy:
    pqDestroy(pq);
    return result;
}


/* ============= TESTING pqChangePriority ============= */

bool testPQChangePrioritySampleNullArgument() {
    bool result = true;
    PQ pq = createPQ();

    int prio = 5;
    ASSERT_TEST(pqChangePriority(pq, NULL, NULL, &prio) == PQ_NULL_ARGUMENT, destroy);

    destroy:
    pqDestroy(pq);
    return result;
}

bool testPQChangePriorityNonExistentElement() {
    bool result = true;
    PQ pq = getMultipleElementPQ();

    int elem1 = 10;
    int old_prio = 20;
    int new_prio = 15;
    ASSERT_TEST(pqChangePriority(pq, &elem1, &old_prio, &new_prio) == PQ_ELEMENT_DOES_NOT_EXISTS, destroy);

    pqInsert(pq, &elem1, &old_prio);
    int wrong_old_prio = 1001;
    ASSERT_TEST(pqChangePriority(pq, &elem1, &wrong_old_prio, &new_prio) == PQ_ELEMENT_DOES_NOT_EXISTS, destroy);

    int wrong_elem = 1001;
    ASSERT_TEST(pqChangePriority(pq, &wrong_elem, &old_prio, &new_prio) == PQ_ELEMENT_DOES_NOT_EXISTS, destroy);

    destroy:
    pqDestroy(pq);
    return result;
}

bool testPQChangePriorityAlsoChangesPositionInQueue() {
    bool result = true;
    PQ pq = createPQ();

    int elem1 = 1;
    int prio1 = 1;
    int elem2 = 2;
    int prio2 = 2;
    int elem3 = 3;
    int prio3 = 3;
    pqInsert(pq, &elem1, &prio1);
    pqInsert(pq, &elem2, &prio2);
    pqInsert(pq, &elem3, &prio3);

    int new_prio2 = 5;
    ASSERT_TEST(pqChangePriority(pq, &elem2, &prio2, &new_prio2) == PQ_SUCCESS, destroy);

    ASSERT_TEST((*(int *) pqGetFirst(pq)) == 2, destroy);
    ASSERT_TEST((*(int *) pqGetNext(pq)) == 3, destroy);
    ASSERT_TEST((*(int *) pqGetNext(pq)) == 1, destroy);

    destroy:
    pqDestroy(pq);
    return result;
}

bool testPQChangePriorityWOnlyChangesFirstOfMultipleSameElementsAndReinsertsIt() {
    bool result = true;
    PQ pq = createPQ();

    int prio = 1;
    int elem = 1;
    pqInsert(pq, &elem, &prio);
    pqInsert(pq, &elem, &prio);
    pqInsert(pq, &elem, &prio);

    int mid_elem = 5;
    int mid_prio = 5;
    pqInsert(pq, &mid_elem, &mid_prio);

    int elem2 = 10;
    int prio2 = 10;
    pqInsert(pq, &elem2, &prio2);

    int new_prio = prio2;
    ASSERT_TEST(pqChangePriority(pq, &elem, &prio, &new_prio) == PQ_SUCCESS, destroy);

    ASSERT_TEST((*(int *) pqGetFirst(pq)) == elem2, destroy); // Original elem2
    ASSERT_TEST((*(int *) pqGetNext(pq)) == elem, destroy); // Reinserted change prio
    ASSERT_TEST((*(int *) pqGetNext(pq)) == mid_elem, destroy); // Mid Elem
    ASSERT_TEST((*(int *) pqGetNext(pq)) == elem, destroy); // Left overs
    ASSERT_TEST((*(int *) pqGetNext(pq)) == elem, destroy);

    destroy:
    pqDestroy(pq);
    return result;
}



/* ============= TESTING pqRemove ============= */

bool testPQRemoveSampleNullArgument() {
    bool result = true;
    ASSERT_TEST(pqRemove(NULL) == PQ_NULL_ARGUMENT, destroy);

    destroy:
    return result;
}


bool testPQRemoveOnEmptyQueueReturnsSuccess () {
    bool result = true;
    PQ pq = createPQ();
    ASSERT_TEST(pqRemove(pq) == PQ_SUCCESS, destroy);

    destroy:
    pqDestroy(pq);
    return result;
}

bool testPQRemoveStandardTest() {
    bool result = true;
    PQ pq = getMultipleElementPQ();

    pqRemove(pq);
    pqRemove(pq);
    pqRemove(pq);
    ASSERT_TEST(pqGetSize(pq) == 7, destroy); // Test it removed 3 successfully

    pqClear(pq);
    int elem = 5;
    pqInsert(pq, &elem, &elem);
    int elem2 = 4;
    pqInsert(pq, &elem2, &elem2);
    pqRemove(pq);

    ASSERT_TEST((*(int *) pqGetFirst(pq)) == elem2, destroy); // Checks it removes the highest priority

    destroy:
    pqDestroy(pq);
    return result;
}


/* ============= TESTING pqRemoveElement ============= */

bool testPQRemoveElementStandardTestAndAlsoOnlyRemovesFirstOfMultipleSameElements() {
    bool result = true;
    PQ pq = createPQ();

    int elem = 5;
    int elem2 = 4;
    pqInsert(pq, &elem, &elem);
    pqInsert(pq, &elem2, &elem2);
    pqInsert(pq, &elem, &elem);
    pqInsert(pq, &elem2, &elem2);

    pqRemoveElement(pq, &elem2);
    ASSERT_TEST(pqGetSize(pq) == 3, destroy);
    ASSERT_TEST((*(int *) pqGetFirst(pq)) == elem, destroy);
    ASSERT_TEST((*(int *) pqGetNext(pq)) == elem, destroy);
    ASSERT_TEST((*(int *) pqGetNext(pq)) == elem2, destroy);

    destroy:
    pqDestroy(pq);
    return result;
}

bool testPQRemoveElementDoesNotExistAfterRemove() {
    bool result = true;
    PQ pq = getMultipleElementPQ();

    int elem = 5;
    ASSERT_TEST(pqRemoveElement(pq, &elem) == PQ_ELEMENT_DOES_NOT_EXISTS, destroy);

    pqInsert(pq, &elem, &elem);
    ASSERT_TEST(pqRemoveElement(pq, &elem) == PQ_SUCCESS, destroy);
    ASSERT_TEST(pqRemoveElement(pq, &elem) == PQ_ELEMENT_DOES_NOT_EXISTS, destroy);

    destroy:
    pqDestroy(pq);
    return result;
}


/* ============= TESTING pqGetFirst ============= */

bool testPQGetFirstEmptyQueueReturnNull() {
    bool result = true;
    PQ pq = createPQ();

    ASSERT_TEST(pqGetFirst(pq) == NULL, destroy);

    destroy:
    pqDestroy(pq);
    return result;
}

bool testPQSGetFirstStandardTest() {
    bool result = true;
    PQ pq = createPQ();

    int elem1 = 5;
    int elem2 = 7;
    pqInsert(pq, &elem1, &elem1);
    pqInsert(pq, &elem2, &elem2);
    ASSERT_TEST((*(int *) pqGetFirst(pq)) == elem2, destroy);

    destroy:
    pqDestroy(pq);
    return result;
}


/* ============= TESTING pqGetNext ============= */
bool testPQGetNextStandardTest() {
    bool result = true;
    PQ pq = getMultipleElementPQ();

    pqGetFirst(pq);
    for (int i = 0; i < 9; i++) {
        ASSERT_TEST(pqGetNext(pq) != NULL, destroy);
    }

    ASSERT_TEST(pqGetNext(pq) == NULL, destroy);

    destroy:
    pqDestroy(pq);
    return result;
}

bool testPQGetNextTraversesTheQueueCorrectlyByPriority() {
    bool result = true;
    PQ pq = createPQ();

    int elem1 = 1;
    int elem2 = elem1 + 1;
    int elem3 = elem2 + 1;
    int elem4 = elem3 + 1;
    int elem5 = elem4 + 1;
    int elem6 = elem5 + 1;
    pqInsert(pq, &elem6, &elem6);
    pqInsert(pq, &elem4, &elem4);
    pqInsert(pq, &elem3, &elem3);
    pqInsert(pq, &elem1, &elem1);
    pqInsert(pq, &elem2, &elem2);
    pqInsert(pq, &elem6, &elem6);
    pqInsert(pq, &elem2, &elem2);
    pqInsert(pq, &elem5, &elem5);

    ASSERT_TEST((*(int *) pqGetFirst(pq)) == elem6, destroy);
    ASSERT_TEST((*(int *) pqGetNext(pq)) == elem6, destroy);
    ASSERT_TEST((*(int *) pqGetNext(pq)) == elem5, destroy);
    ASSERT_TEST((*(int *) pqGetNext(pq)) == elem4, destroy);
    ASSERT_TEST((*(int *) pqGetNext(pq)) == elem3, destroy);
    ASSERT_TEST((*(int *) pqGetNext(pq)) == elem2, destroy);
    ASSERT_TEST((*(int *) pqGetNext(pq)) == elem2, destroy);
    ASSERT_TEST((*(int *) pqGetNext(pq)) == elem1, destroy);
    ASSERT_TEST(pqGetNext(pq) == NULL, destroy);

    destroy:
    pqDestroy(pq);
    return result;
}

bool testPQGetNextIteratorStaysTheSameInMostFunction() {
    bool result = true;
    PQ pq = createPQ();

    int elem1 = 1;
    int elem2 = 2;
    int elem3 = 3;
    pqInsert(pq, &elem1, &elem1);
    pqInsert(pq, &elem2, &elem2);
    pqInsert(pq, &elem3, &elem3);

    ASSERT_TEST((*(int *) pqGetFirst(pq)) == 3, destroy);
    ASSERT_TEST((*(int *) pqGetNext(pq)) == 2, destroy);

    PQ pq_temp = createPQ();
    pqDestroy(pq_temp);
    pqContains(pq, &elem1);
    pqGetSize(pq);

    ASSERT_TEST((*(int *) pqGetNext(pq)) == 1, destroy);

    destroy:
    pqDestroy(pq);
    return result;
}


bool testPQIteratorPointsToNullAfterSomeOfTheFunctions() {
    bool result = true;
    PQ pq = getMultipleElementPQ();

    int elem1 = 1;
    pqInsert(pq, &elem1, &elem1);

    pqGetFirst(pq);
    ASSERT_TEST(pqGetNext(pq) != NULL, destroy);

    ASSERT_TEST(pqGetFirst(pq) != NULL, destroy);
    pqInsert(pq, &elem1, &elem1);
    ASSERT_TEST(pqGetNext(pq) == NULL, destroy);

    ASSERT_TEST(pqGetFirst(pq) != NULL, destroy);
    pqRemove(pq);
    ASSERT_TEST(pqGetNext(pq) == NULL, destroy);

    ASSERT_TEST(pqGetFirst(pq) != NULL, destroy);
    pqRemoveElement(pq, &elem1);
    ASSERT_TEST(pqGetNext(pq) == NULL, destroy);

    ASSERT_TEST(pqGetFirst(pq) != NULL, destroy);
    PQ new_pq = pqCopy(pq);
    ASSERT_TEST(pqGetNext(pq) == NULL, destroyNewPq);
    ASSERT_TEST(pqGetNext(new_pq) == NULL, destroyNewPq);

    ASSERT_TEST(pqGetFirst(pq) != NULL, destroy);
    int new_prio = 5;
    pqChangePriority(pq, &elem1, &elem1, &new_prio);
    ASSERT_TEST(pqGetNext(pq) == NULL, destroy);

    destroyNewPq:
    pqDestroy(new_pq);
    destroy:
    pqDestroy(pq);
    return result;
}


/* ============= TESTING pqClear ============= */
bool testPQClearWorksOkayOnEmptyQueue() {
    bool result = true;
    PQ pq = createPQ();

    ASSERT_TEST(pqClear(pq) == PQ_SUCCESS, destroy);

    destroy:
    pqDestroy(pq);
    return result;
}

bool testPQClearStandardTest() {
    bool result = true;
    PQ pq = getMultipleElementPQ();

    ASSERT_TEST(pqClear(pq) == PQ_SUCCESS, destroy);
    ASSERT_TEST(pqGetSize(pq) == 0, destroy);

    ASSERT_TEST(pqGetFirst(pq) == NULL, destroy);
    ASSERT_TEST(pqGetNext(pq) == NULL, destroy);

    destroy:
    pqDestroy(pq);
    return result;
}



/* ============= Course given tests ============= */
bool testPQCreateDestroy() {
    bool result = true;

    PriorityQueue pq = createPQ();
    ASSERT_TEST(pq != NULL, returnPQCreateDestroy);
    ASSERT_TEST(pqGetSize(pq) == 0, destroyPQCreateDestroy);
    ASSERT_TEST(pqGetFirst(pq) == NULL, destroyPQCreateDestroy);

    destroyPQCreateDestroy:
    pqDestroy(pq);
    returnPQCreateDestroy:
    return result;
}

bool testPQInsertAndSize() {
    bool result = true;
    PriorityQueue pq = pqCreate(copyIntGeneric, freeIntGeneric, equalIntsGeneric, copyIntGeneric, freeIntGeneric,
                                compareIntsGeneric);
    ASSERT_TEST(pqGetSize(pq) == 0, destroyPQInsertAndSize);
    int to_add = 1;
    ASSERT_TEST(pqInsert(pq, &to_add, &to_add) == PQ_SUCCESS, destroyPQInsertAndSize);
    ASSERT_TEST(pqGetSize(pq) == 1, destroyPQInsertAndSize);

    destroyPQInsertAndSize:
    pqDestroy(pq);
    return result;
}

bool testPQGetFirst() {
    bool result = true;
    PriorityQueue pq = pqCreate(copyIntGeneric, freeIntGeneric, equalIntsGeneric, copyIntGeneric, freeIntGeneric,
                                compareIntsGeneric);
    ASSERT_TEST(pqGetSize(pq) == 0, destroyPQGetFirst);
    int to_add = 1;
    ASSERT_TEST(pqInsert(pq, &to_add, &to_add) == PQ_SUCCESS, destroyPQGetFirst);
    int *first_value = pqGetFirst(pq);
    ASSERT_TEST(first_value != NULL, destroyPQGetFirst);
    ASSERT_TEST(*first_value == to_add, destroyPQGetFirst);

    destroyPQGetFirst:
    pqDestroy(pq);
    return result;
}

bool testPQIterator() {
    bool result = true;
    PriorityQueue pq = pqCreate(copyIntGeneric, freeIntGeneric, equalIntsGeneric, copyIntGeneric, freeIntGeneric,
                                compareIntsGeneric);

    int max_value = 10;

    for (int i = 0; i < max_value; i++) {
        ASSERT_TEST(pqInsert(pq, &i, &i) == PQ_SUCCESS, destroyPQIterator);
    }

    int i = 0;
    PQ_FOREACH(int*, iter, pq) {
        if (i != max_value) {
            ASSERT_TEST(iter != NULL, destroyPQIterator);
        } else {
            ASSERT_TEST(iter == NULL, destroyPQIterator);
        }
        i++;
    }

    destroyPQIterator:
    pqDestroy(pq);
    return result;
}


bool (*tests[])(void) = {
        testPQCreateStandardTest,
        testPQCreateDestroy,
        testPQInsertAndSize,
        testPQGetFirst,
        testPQIterator,
        testPQIteratorPointsToNullAfterSomeOfTheFunctions,
        testPQGetNextIteratorStaysTheSameInMostFunction,
        testMemoryPQDestroyEmptyPQ,
        testMemoryPQDestroySingleElement,
        testMemoryPQDestroyMultipleElement,
        testPQCopySampleNULLArgument,
        testPQCopySingleElement,
        testPQCopyMultipleElement,
        testPQCopyMultipleSameElementDifferentPriority,
        testPQCopyMultipleSameElementSamePriority,
        testPQGetSizeSampleNullArgument,
        testPQGetSizeStandardTest,
        testPQContainsSampleNullArgument,
        testPQContainsStandardTest,
        testPQContainsFalseAfterRemove,
        testPQContainsSameElementMultipleTimesAlsoWithDifferentPriority,
        testPQInsertSampleNULLArgument,
        testPQInsertStandardTest,
        testPQInsertPutsNewElementsInCorrectPlace,
        testPQChangePriorityAlsoChangesPositionInQueue,
        testPQChangePriorityNonExistentElement,
        testPQChangePrioritySampleNullArgument,
        testPQChangePriorityWOnlyChangesFirstOfMultipleSameElementsAndReinsertsIt,
        testPQRemoveSampleNullArgument,
        testPQRemoveOnEmptyQueueReturnsSuccess,
        testPQRemoveStandardTest,
        testPQRemoveElementDoesNotExistAfterRemove,
        testPQRemoveElementStandardTestAndAlsoOnlyRemovesFirstOfMultipleSameElements,
        testPQGetFirstEmptyQueueReturnNull,
        testPQSGetFirstStandardTest,
        testPQGetNextStandardTest,
        testPQGetNextTraversesTheQueueCorrectlyByPriority,
        testPQClearStandardTest,
        testPQClearWorksOkayOnEmptyQueue
};

const char *testNames[] = {
        "testPQCreateStandardTest",
        "testPQCreateDestroy",
        "testPQInsertAndSize",
        "testPQGetFirst",
        "testPQIterator",
        "testPQIteratorPointsToNullAfterSomeOfTheFunctions",
        "testPQGetNextIteratorStaysTheSameInMostFunction",
        "testMemoryPQDestroyEmptyPQ",
        "testMemoryPQDestroySingleElement",
        "testMemoryPQDestroyMultipleElement",
        "testPQCopySampleNULLArgument",
        "testPQRemoveOnEmptyQueueReturnsSuccess",
        "testPQCopySingleElement",
        "testPQCopyMultipleElement",
        "testPQCopyMultipleSameElementDifferentPriority",
        "testPQCopyMultipleSameElementSamePriority",
        "testPQGetSizeSampleNullArgument",
        "testPQGetSizeStandardTest",
        "testPQContainsSampleNullArgument",
        "testPQContainsStandardTest",
        "testPQContainsFalseAfterRemove",
        "testPQContainsSameElementMultipleTimesAlsoWithDifferentPriority",
        "testPQInsertSampleNULLArgument",
        "testPQInsertStandardTest",
        "testPQInsertPutsNewElementsInCorrectPlace",
        "testPQChangePriorityAlsoChangesPositionInQueue",
        "testPQChangePriorityNonExistentElement",
        "testPQChangePrioritySampleNullArgument",
        "testPQChangePriorityWOnlyChangesFirstOfMultipleSameElementsAndReinsertsIt",
        "testPQRemoveSampleNullArgument",
        "testPQRemoveStandardTest",
        "testPQRemoveElementDoesNotExistAfterRemove",
        "testPQRemoveElementStandardTestAndAlsoOnlyRemovesFirstOfMultipleSameElements",
        "testPQGetFirstEmptyQueueReturnNull",
        "testPQSGetFirstStandardTest",
        "testPQGetNextStandardTest",
        "testPQGetNextTraversesTheQueueCorrectlyByPriority",
        "testPQClearStandardTest",
        "testPQClearWorksOkayOnEmptyQueue"
};

const char *testFailDescriptions[] = {
        "Please refer to the code at function: testPQCreateStandardTest",
        "Please refer to the code at function: testPQCreateDestroy",
        "Please refer to the testing code at function: testPQInsertAndSize",
        "Please refer to the testing code at function: testPQGetFirst",
        "Please refer to the testing code at function: testPQIterator",
        "According to the course segel after the following functions the iterator should be null: pqInsert, pqCopy (both pqs), pqChangePriority, pqRemove, pqRemoveElement. If the test failed, your iterator isn't NULL after one of these functions",
        "Please refer to the testing code at function: testPQGetNextIteratorStaysTheSameInMostFunction",
        "Please refer to the testing code at function: testMemoryPQDestroyEmptyPQ",
        "Please refer to the testing code at function: testMemoryPQDestroySingleElement",
        "Please refer to the testing code at function: testMemoryPQDestroyMultipleElement",
        "Please refer to the testing code at function: testPQCopySampleNULLArgument",
        "Please refer to the testing code at function: testPQCopySingleElement",
        "Please refer to the testing code at function: testPQCopyMultipleElement",
        "Please refer to the testing code at function: testPQCopyMultipleSameElementDifferentPriority",
        "Please refer to the testing code at function: testPQCopyMultipleSameElementSamePriority",
        "Please refer to the testing code at function: testPQGetSizeSampleNullArgument",
        "Please refer to the testing code at function: testPQGetSizeStandardTest",
        "Please refer to the testing code at function: testPQContainsSampleNullArgument",
        "Please refer to the testing code at function: testPQContainsStandardTest",
        "Please refer to the testing code at function: testPQContainsFalseAfterRemove",
        "Please refer to the testing code at function: PQContainsSameElementMultipleTimesAlsoWithDifferentPriority",
        "Please refer to the testing code at function: testPQInsertSampleNULLArgument",
        "Please refer to the testing code at function: testPQInsertStandardTest",
        "Please refer to the testing code at function: testPQInsertPutsNewElementsInCorrectPlace",
        "Please refer to the testing code at function: testPQChangePriorityAlsoChangesPositionInQueue",
        "Please refer to the testing code at function: testPQChangePriorityNonExistentElement",
        "Please refer to the testing code at function: testPQChangePrioritySampleNullArgument",
        "Please refer to the testing code at function: testPQChangePriorityWOnlyChangesFirstOfMultipleSameElementsAndReinsertsIt",
        "Please refer to the testing code at function: testPQRemoveSampleNullArgument",
        "Please refer to the testing code at function: testPQRemoveOnEmptyQueueReturnsSuccess",
        "Please refer to the testing code at function: testPQRemoveStandardTest",
        "Please refer to the testing code at function: testPQRemoveElementDoesNotExistAfterRemove",
        "Please refer to the testing code at function: testPQRemoveElementStandardTestAndAlsoOnlyRemovesFirstOfMultipleSameElements",
        "Please refer to the testing code at function: testPQGetFirstEmptyQueueReturnNull",
        "Please refer to the testing code at function: testPQSGetFirstStandardTest",
        "Please refer to the testing code at function: testPQGetNextStandardTest",
        "Please refer to the testing code at function: testPQGetNextTraversesTheQueueCorrectlyByPriority",
        "Please refer to the testing code at function: testPQClearStandardTest",
        "Please refer to the testing code at function: testPQClearWorksOkayOnEmptyQueue"
};

int main() {

    if(testPQCopySingleElement()) {
        printf("passed\n");
    }

    return 0;
}
