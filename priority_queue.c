#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "priority_queue.h"


/*----------------------------------------------------------------------
                         Implementation constants
 ----------------------------------------------------------------------*/


#define INITIAL_SIZE 10

struct PriorityQueue_t {

PQElement* elements;
PQElementPriority* priorities;

int size;
int max_size;
int iterator;

CopyPQElement copy_element;
FreePQElement free_element;
EqualPQElements compare_elements;

CopyPQElementPriority copy_priority;
FreePQElementPriority free_priority;
ComparePQElementPriorities compare_priorities;

};


/*----------------------------------------------------------------------
                             Static helper functions
 ----------------------------------------------------------------------*/


static int findSpecificElement(PriorityQueue queue, PQElement element) 
{
    assert(queue != NULL);

    int max_priority_index = -1, j = 0;

    for (j; j < queue->size; ++j) 
    {
        if(queue->compare_elements(queue->elements[j], element) == 0)
        {
            max_priority_index = j;
        }
    }

    for (j; j < queue->size; ++j) 
    {
        if (queue->compare_elements(queue->elements[j], element) == 0) 
        {
            if(queue->compare_priorities(queue->priorities[j], queue->priorities[max_priority_index]) > 0)
                max_priority_index = j;
        }
    }

    if(max_priority_index == -1){
    return PQ_ELEMENT_DOES_NOT_EXISTS;
    }

    return max_priority_index;
}


static int findHighestPriorityElement(PriorityQueue queue)//Made this for you so you can use it in pqRemove, improve it if you want. 
{
    assert(queue != NULL);

    int max_priority_index = 0;

    for (int i = 1; i < queue->size; ++i) 
    {
        if(queue->compare_priorities(queue->priorities[i], queue->priorities[max_priority_index]) > 0)
        {
            max_priority_index = i;
        }
    }

    return max_priority_index;
}


static bool pqIsEmpty(PriorityQueue queue) 
{
  return pqGetSize(queue) == 0;
}


/*----------------------------------------------------------------------
                             Creation functions
 ----------------------------------------------------------------------*/


PriorityQueue pqCreate(CopyPQElement copy_element,
                       FreePQElement free_element,
                       EqualPQElements equal_elements,
                       CopyPQElementPriority copy_priority,
                       FreePQElementPriority free_priority,
                       ComparePQElementPriorities compare_priorities)
{
    assert(copy_element != NULL && 
           free_element != NULL && 
           equal_elements != NULL && 
           copy_priority != NULL && 
           free_priority != NULL && 
           compare_priorities != NULL);


    PriorityQueue queue = malloc (sizeof(*queue));
    if (queue == NULL)
    {
        return NULL;
    }


    queue->elements = malloc(INITIAL_SIZE * sizeof(PQElement));
    if(queue->elements == NULL)
    {
        free(queue);
        return NULL;
    }


    queue->priorities = malloc(INITIAL_SIZE * sizeof(PQElementPriority));
    if(queue->priorities == NULL)
    {
        pqDestroy(queue);
        return NULL;
    }


    queue->size = 0;
    queue->max_size = INITIAL_SIZE;
    queue->iterator = 0;


    queue->copy_element = copy_element;
    queue->free_element = free_element;
    queue->compare_elements = equal_elements;


    queue->copy_priority = copy_priority;
    queue->free_priority = free_priority;
    queue->compare_priorities = compare_priorities;
}


void pqDestroy (PriorityQueue queue)
{
    if(queue == NULL)
    {
        return;
    }

    while(!pqIsEmpty(queue))
    {
        pqRemoveElement(queue, pqGetFirst(queue));
    }

    free(queue->elements);
    free(queue->priorities);
    free(queue);
}


/*----------------------------------------------------------------------
                              Queue operations
 ----------------------------------------------------------------------*/



PriorityQueueResult pqRemoveElement (PriorityQueue queue, PQElement element)
{
    assert(queue != NULL);

    int index = findSpecificElement(queue, element);
    if (index == PQ_ELEMENT_DOES_NOT_EXISTS) {
        return PQ_ERROR;
    }

    queue->free_element(queue->elements[index]);
    queue->elements[index] = queue->elements[queue->size - 1];

    queue->free_priority(queue->priorities[index]);
    queue->priorities[index] = queue->priorities[queue->size - 1];

    queue->size--;
    queue->iterator = 0;

    return PQ_SUCCESS;
}


int pqGetSize(PriorityQueue queue) 
{
    assert(queue != NULL); 
    return queue->size;
}


/*----------------------------------------------------------------------
                               Queue iteration
 ----------------------------------------------------------------------*/


PQElement pqGetFirst(PriorityQueue queue) 
{
    assert(queue != NULL);
    queue->iterator = 0;
    return pqGetNext(queue);
}


PQElement pqGetNext(PriorityQueue queue)
{
    assert(queue != NULL);
    if (queue->iterator >= queue->size) 
    {
        return NULL;
    }
    return queue->elements[queue->iterator++];
}