#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "priority_queue.h"


/*----------------------------------------------------------------------
                         Implementation constants
 ----------------------------------------------------------------------*/

// Returned when an element is not in the queue
#define ELEMENT_NOT_FOUND -1

// The initial size of the queue (i.e. list_of_elements)
#define INITIAL_SIZE 10

// The factor by which the queue's memory is expanded
#define EXPAND_RATE 2


typedef struct ElementsStruct {
    PQElement element;
    PQElementPriority priority; 
} Element;                                  

struct PriorityQueue_t {
    Element* list_of_elements;                 
    Element* iterator; //changed to a pointer @NabeehAS

    int size; //number of elements in list_of_elements
    int max_size; //size of list_of_elements

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

// returns true if queue is empty, false otherwise
// Note: an empty queue means that the "list_of_elements" in queue is of size 0.
//      (the size of "list_of_elements" is equal to the "size" field in PriorityQueue_t)
static bool pqIsEmpty(const PriorityQueue queue) {
    assert(queue != NULL);
    if(queue->size == 0) return true;
    else return false;
}

// Returns the highest priority of all of the "list_of_elements" in queue
// NOTE: does not return the index of the highest priority element in "list_of_elements"
// returns NULL if queue is empty (i.e. the "list_of_elements" array in queue has 0 enteries)
static PQElementPriority findHighestPriorityInQueue(const PriorityQueue queue) {
    assert(queue != NULL);

    // queue is empty, therefore there is no element with 
    // highest priority because there are no elements in queue
    if(pqIsEmpty(queue)) {
        return NULL;
    }

    // highest_priority is first set to the priority of the first element in "list_of_element"
    PQElementPriority highest_priority = queue->list_of_elements[0].priority;

    for(int index = 1; index < queue->size; index++) {
        PQElementPriority current_element_priority = queue->list_of_elements[index].priority;

        // the compare_priorities function returns a positive number if 
        // current_element_priority is greater than highest_priority
        if(queue->compare_priorities(current_element_priority, highest_priority) > 0) {
            highest_priority = current_element_priority;
        }
    }
    // at the end of this for loop, highest_priority will be set to the highest priority in "list_of_elements"

    return highest_priority;
}

// Returns the index of the highest priority element in the "list_of_elements".
// If there are multiple elements with the high priority, than return the first one in "list_of_elements"
// Note: if the queue is empty, than return ELEMENT_NOT_FOUND
static int findHighestPriorityElementIndex(const PriorityQueue queue) {
    assert(queue != NULL);

    // gets the highest priority in queue
    PQElementPriority highest_priority = findHighestPriorityInQueue(queue);
    if(highest_priority == NULL) {
        return ELEMENT_NOT_FOUND;
    }

    for(int index = 0; index < queue->size; index++) {
        // the compare_priorities() func returns 0 when the two inputted priorities are equal
        if(queue->compare_priorities(queue->list_of_elements[index].priority, highest_priority) == 0) {
            return index;
        }
    }

    return ELEMENT_NOT_FOUND;
}

// frees the memory used by queue->list_of_elements
static void destroyListOfElementsInQueue(PriorityQueue queue) {
    //TODO:
}

/**
 * @brief expands the queue's internal array (list_of_elements) so it can store more elements
 * 
 * @return PriorityQueueResult :
 *         PQ_SUCCESS if the expansion was succesful
 *         PQ_OUT_OF_MEMORY if there was not enough memory
 */
static PriorityQueueResult expand(PriorityQueue queue) {
    assert(queue != NULL); //Not needed, i think ?

    int newSize = queue->max_size * EXPAND_RATE;
    Element* new_list_of_elements = malloc(sizeof(Element) * newSize);
    if(new_list_of_elements == NULL) {
        return PQ_OUT_OF_MEMORY;
    }

    for(int index = 0; index < queue->max_size; index++) {
        new_list_of_elements[index].element = malloc(sizeof(PQElement));
        if(new_list_of_elements[index].element == NULL) {
            free(new_list_of_elements[index].element);
            free(new_list_of_elements);
            return PQ_OUT_OF_MEMORY;
        }

        new_list_of_elements[index].priority = malloc(sizeof(PQElementPriority));
        if(new_list_of_elements[index].priority == NULL) {
            free(new_list_of_elements[index].element);
            free(new_list_of_elements[index].priority);
            free(new_list_of_elements);
            return PQ_OUT_OF_MEMORY;
        }

        Element current_element = queue->list_of_elements[index];
        new_list_of_elements[index].element = current_element.element;
        new_list_of_elements[index].priority = current_element.priority;
    }
    // after this point, all of the new memory needed has been succesfully allocated 
    // and therefore the old list_of_elements can be destroyed
    destroyListOfElementsInQueue(queue);

    queue->list_of_elements = new_list_of_elements;
    queue->max_size = newSize;

    return PQ_SUCCESS;
}

// Returns the index of the same element as the recieved element with the highest priority.
static int findSpecificElement(PriorityQueue queue, PQElement element) {
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
    if (queue == NULL) {
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

void pqDestroy (PriorityQueue queue) {
    if(queue == NULL) {
        return;
    }

    while(!pqIsEmpty(queue))
    {
        pqRemoveElement(queue, pqGetFirst(queue));
    }
    
    //add free elements struct func instead
    free(queue->list_of_elements);
    free(queue->iterator);
    free(queue);
}


// 
PriorityQueue pqCopy(PriorityQueue queue) {

}


/*----------------------------------------------------------------------
                              Queue operations
 ----------------------------------------------------------------------*/


PriorityQueueResult pqInsert(PriorityQueue queue, PQElement element, PQElementPriority priority) {
    if(queue == NULL || element == NULL || priority == NULL) {
        return PQ_NULL_ARGUMENT;
    }

    // the list_of_elements array has reached its max size, therefore we need to expand its size
    if(queue->size == queue->max_size) {

    }


}


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