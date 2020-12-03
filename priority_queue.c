#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "priority_queue.h"


/*----------------------------------------------------------------------
                         Implementation constants
 ----------------------------------------------------------------------*/

// Returned by find() when a value is not in the set
#define ELEMENT_NOT_FOUND -1

#define INITIAL_SIZE 10

typedef struct ElementsStruct {
    PQElement element;
    PQElementPriority priority; 
} Element;                                  


struct PriorityQueue_t {
    Element* list_of_elements;                 
    Element iterator;

    int size;
    int max_size;

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

// Returns the index of the same element as the recieved element with the highest priority.
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
        return ELEMENT_NOT_FOUND;
    }

    return max_priority_index;
}

// Returns the index of the highest priority element.
// If there are multiple elements with the high priority, than return the first one in list_of_elements
static int findHighestPriorityElement(PriorityQueue queue) 
{
    assert(queue != NULL);
    
    //    1 2 3 3 2 2
        // 2 for loops:
        //   1st: get you the highest priority (in the exmample, return 3)
        //   2nd: return the index of the first element with this highest priority

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
                       ComparePQElementPriorities compare_priorities) {
    if(copy_element == NULL || 
           free_element == NULL || 
           equal_elements == NULL || 
           copy_priority == NULL || 
           free_priority == NULL || 
           compare_priorities == NULL) {
               return NULL;
           }


    PriorityQueue queue = malloc (sizeof(*queue));
    if (queue == NULL)
    {
        return NULL;
    }

    //Add allocate elements struct
    queue->list_of_elements = malloc(INITIAL_SIZE * sizeof(Element));
    if(queue->list_of_elements == NULL) {
        free(queue);
        return NULL;
    }

    queue->list_of_elements[0].element = malloc(sizeof(PQElement));
    if(queue->list_of_elements[0].element == NULL) {
        free(queue->list_of_elements);
        free(queue);
        return NULL;
    }

    queue->list_of_elements[0].priority = malloc(sizeof(PQElementPriority));
    if(queue->list_of_elements[0].priority == NULL) {
        free(queue->list_of_elements[0].element);
        free(queue->list_of_elements);
        free(queue);
        return NULL;
    }
 
    queue->size = 0;
    queue->max_size = INITIAL_SIZE;
    queue->iterator = NULL;
    
    queue->copy_element = copy_element;
    queue->free_element = free_element;
    queue->compare_elements = equal_elements;

    queue->copy_priority = copy_priority;
    queue->free_priority = free_priority;
    queue->compare_priorities = compare_priorities;
}


// void pqDestroy (PriorityQueue queue)
// {
//     if(queue == NULL) {
//         return;
//     }

//     while(!pqIsEmpty(queue))
//     {
//         pqRemoveElement(queue, pqGetFirst(queue));
//     }
//     
//     //add free elements struct func instead
//     free(queue->list_of_elements);
//     free(queue->iterator);
//     free(queue);
// }


/*----------------------------------------------------------------------
                              Queue operations
 ----------------------------------------------------------------------*/



// PriorityQueueResult pqRemoveElement (PriorityQueue queue, PQElement element)
// {
//     assert(queue != NULL);

//     int index = findSpecificElement(queue, element);
//     if (index == PQ_ELEMENT_DOES_NOT_EXISTS) {
//         return PQ_ERROR;
//     }

//     queue->free_element(queue->elements[index]);
//     queue->elements[index] = queue->elements[queue->size - 1];

//     queue->free_priority(queue->priorities[index]);
//     queue->priorities[index] = queue->priorities[queue->size - 1];

//     queue->size--;
//     queue->iterator = 0;

//     return PQ_SUCCESS;
// }


// int pqGetSize(PriorityQueue queue) 
// {
//     assert(queue != NULL); 
//     return queue->size;
// }


// /*----------------------------------------------------------------------
//                                Queue iteration
//  ----------------------------------------------------------------------*/


PQElement pqGetFirst(PriorityQueue queue) 
{
    if(queue == NULL) {
        return NULL;
    }

    int highest_priority_index = findHighestPriorityElement(queue);
    if(highest_priority_index != ELEMENT_NOT_FOUND) {
        return NULL;
    }
    
    queue->iterator = queue->list_of_elements[highest_priority_index];

    return queue->list_of_elements[highest_priority_index].element;
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