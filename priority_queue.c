#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "priority_queue.h"

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


    PriorityQueue priority_queue = malloc (sizeof(*priority_queue));
    if (priority_queue == NULL)
    {
        return NULL;
    }


    priority_queue->elements = malloc(INITIAL_SIZE * sizeof(PQElement));
    if(priority_queue->elements == NULL)
    {
        free(priority_queue);
        return NULL;
    }


    priority_queue->priorities = malloc(INITIAL_SIZE * sizeof(PQElementPriority));
    if(priority_queue->priorities == NULL)
    {
        pqDestroy(priority_queue);
        return NULL;
    }


    priority_queue->size = 0;
    priority_queue->max_size = INITIAL_SIZE;
    priority_queue->iterator = 0;


    priority_queue->copy_element = copy_element;
    priority_queue->free_element = free_element;
    priority_queue->compare_elements = equal_elements;


    priority_queue->copy_priority = copy_priority;
    priority_queue->free_priority = free_priority;
    priority_queue->compare_priorities = compare_priorities;
}

