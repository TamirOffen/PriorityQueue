#include "priority_queue.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

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
    bool used; //when the iterator has pointed to this Element, used will be set to true
} Element;                                  

struct PriorityQueue_t {
    Element* list_of_elements;                 
    Element* iterator; 

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
    PQElementPriority highest_priority = queue->list_of_elements[0].priority; //TODO: might need to use copyPriority

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
    PQElementPriority highest_priority = findHighestPriorityInQueue(queue); //TODO: might need to use copyPriority
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

// sets the iterator to be in an undefined state
static void clearIterator(PriorityQueue queue) {
    assert(queue != NULL);
    for(int index = 0; index < queue->size; index++) {
        queue->list_of_elements[index].used = false;
    }
}

// takes into account used (aka iterator)
// returns NULL if queue is empty or if iterator has already iterated over all of the elements in queue
static PQElementPriority getNextHighestPriority(const PriorityQueue queue) {
    assert(queue != NULL);

    // queue is empty, therefore there is no element with 
    // highest priority because there are no elements in queue
    if(pqIsEmpty(queue)) {
        return NULL;
    }

    PQElementPriority highest_priority = NULL;
    for(int i = 0; i < queue->size; i++) {
        if(queue->list_of_elements[i].used == false) {
            highest_priority = queue->list_of_elements[i].priority;
            // printf("in getting highest priority: %d\n", queue->list_of_elements[i].used);
            break;
        }
    }

    // the iterator has iterated over all of the elements in the queue
    if(highest_priority == NULL) {
        return NULL;
    }

    for(int index = 0; index < queue->size; index++) {
        //if the iterator already looked at this element in list_of_elements
        if(queue->list_of_elements[index].used == true) {
            continue;
        }

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

// returns the index in "list_of_elements" of the next highest priority element
// NOTE: takes into account what the iterator has already looked at, aka used = true;
static int getNextHighestPriorityElementIndex(PriorityQueue queue) {
    assert(queue != NULL);

    // gets the highest priority in queue
    PQElementPriority highest_priority = getNextHighestPriority(queue); 

    // if highest_priority = NULL, than the iteator has iterated over all of the elements in queue
    if(highest_priority == NULL) {
        return ELEMENT_NOT_FOUND;
    }

    for(int index = 0; index < queue->size; index++) {
        //if the iterator has already looked into this element in list_of_elements
        if(queue->list_of_elements[index].used == true) {
            continue;
        }

        // the compare_priorities() func returns 0 when the two inputted priorities are equal
        if(queue->compare_priorities(queue->list_of_elements[index].priority, highest_priority) == 0) {
            return index;
        }
    }

    return ELEMENT_NOT_FOUND;   
}


// // frees the memory used by queue->list_of_elements
// static void destroyListOfElementsInQueue(PriorityQueue queue) {
//     //TODO:
// }

// /**
//  * @brief expands the queue's internal array (list_of_elements) so it can store more elements
//  * 
//  * @return PriorityQueueResult :
//  *         PQ_SUCCESS if the expansion was succesful
//  *         PQ_OUT_OF_MEMORY if there was not enough memory
//  */
// static PriorityQueueResult expand(PriorityQueue queue) {
//     assert(queue != NULL); //Not needed, i think ?

//     int newSize = queue->max_size * EXPAND_RATE;
//     Element* new_list_of_elements = malloc(sizeof(Element) * newSize);
//     if(new_list_of_elements == NULL) {
//         return PQ_OUT_OF_MEMORY;
//     }

//     for(int index = 0; index < queue->max_size; index++) {
//         new_list_of_elements[index].element = malloc(sizeof(PQElement));
//         if(new_list_of_elements[index].element == NULL) {
//             free(new_list_of_elements[index].element);
//             free(new_list_of_elements);
//             return PQ_OUT_OF_MEMORY;
//         }

//         new_list_of_elements[index].priority = malloc(sizeof(PQElementPriority));
//         if(new_list_of_elements[index].priority == NULL) {
//             free(new_list_of_elements[index].element);
//             free(new_list_of_elements[index].priority);
//             free(new_list_of_elements);
//             return PQ_OUT_OF_MEMORY;
//         }

//         Element current_element = queue->list_of_elements[index];
//         new_list_of_elements[index].element = current_element.element;
//         new_list_of_elements[index].priority = current_element.priority;
//     }
//     // after this point, all of the new memory needed has been succesfully allocated 
//     // and therefore the old list_of_elements can be destroyed
//     destroyListOfElementsInQueue(queue);

//     queue->list_of_elements = new_list_of_elements;
//     queue->max_size = newSize;

//     return PQ_SUCCESS;
// }

// Returns the index of the same element as the recieved element with the highest priority.
// static int findSpecificElement(PriorityQueue queue, PQElement element) {
//     assert(queue != NULL);

//     int max_priority_index = -1, j = 0;

//     for (j; j < queue->size; ++j) 
//     {
//         if(queue->compare_elements(queue->elements[j], element) == 0)
//         {
//             max_priority_index = j;
//         }
//     }

//     for (j; j < queue->size; ++j) 
//     {
//         if (queue->compare_elements(queue->elements[j], element) == 0) 
//         {
//             if(queue->compare_priorities(queue->priorities[j], queue->priorities[max_priority_index]) > 0)
//                 max_priority_index = j;
//         }
//     }

//     if(max_priority_index == -1){
//         return ELEMENT_NOT_FOUND;
//     }

//     return max_priority_index;
// }


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
 
    queue->size = 0;
    queue->max_size = INITIAL_SIZE;
    queue->iterator = NULL;
    
    queue->copy_element = copy_element;
    queue->free_element = free_element;
    queue->compare_elements = equal_elements;

    queue->copy_priority = copy_priority;
    queue->free_priority = free_priority;
    queue->compare_priorities = compare_priorities;

    return queue;
}

void pqDestroy (PriorityQueue queue) {
    if(queue == NULL) {
        return;
    }

    // first free the elements inside of the "list_of_elements" 
    while(!pqIsEmpty(queue)) {
        pqRemove(queue);
    }
    
    free(queue->list_of_elements);
    free(queue->iterator);
    free(queue);
}

PriorityQueue pqCopy(PriorityQueue queue)
{
    PriorityQueue queue_copy = malloc(sizeof(*queue_copy));
    if (queue_copy == NULL){
        return NULL;
    }

    queue_copy->list_of_elements = malloc (queue->size * sizeof(Element));
    if(queue_copy->list_of_elements == NULL){
        free(queue_copy);
        return NULL;
    }

    queue_copy->size = queue->size;
    queue_copy->max_size = queue->max_size;
    queue_copy->iterator = NULL;
    queue->iterator = NULL;

    queue_copy->copy_element = queue->copy_element;
    queue_copy->free_element = queue->free_element;
    queue_copy->compare_elements = queue->compare_elements;

    queue_copy->copy_priority = queue->copy_priority;
    queue_copy->free_priority = queue->free_priority;
    queue_copy->compare_priorities = queue->compare_priorities;

    for(int i = 0; i < queue_copy->size; i++){
        pqInsert(queue_copy, queue->list_of_elements[i].element, queue->list_of_elements[i].priority);
    }
    return queue_copy;

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
        //TODO: expand()
    }

    // the index of the element that wants to be inserted into the queue
    int current_element_index = queue->size;

    // allocated the memory for the Element in list_of_elements in queue
    // also copies the inputted element and priority into the Element in list_of_elements
    queue->list_of_elements[current_element_index].element = queue->copy_element(element);
    if(queue->list_of_elements[current_element_index].element == NULL) {
        queue->free_element(queue->list_of_elements[current_element_index].element);
        return PQ_OUT_OF_MEMORY;
    }
    queue->list_of_elements[current_element_index].priority = queue->copy_priority(priority);
    if(queue->list_of_elements[current_element_index].priority == NULL) {
        queue->free_element(queue->list_of_elements[current_element_index].element);
        queue->free_priority(queue->list_of_elements[current_element_index].priority);
        return PQ_OUT_OF_MEMORY;
    }

    queue->size++;

    clearIterator(queue);

    return PQ_SUCCESS;
}


PriorityQueueResult pqRemove(PriorityQueue queue) {
    if(queue == NULL) {
        return PQ_NULL_ARGUMENT;
    }

    int highest_priority_element_index = findHighestPriorityElementIndex(queue);

    // freeing the memory inside Element (both element and priority)
    queue->free_element(queue->list_of_elements[highest_priority_element_index].element);
    queue->free_priority(queue->list_of_elements[highest_priority_element_index].priority);

    // moving over the elements after highest_piority_element_index
    queue->list_of_elements[highest_priority_element_index] = queue->list_of_elements[queue->size - 1];

    queue->size--;
    //todo: check to reduce size! will only help with memory but i dont think its worth spending the time on it

    // iterator is undefined after pqRemove
    queue->iterator = NULL; 
    clearIterator(queue);

    return PQ_SUCCESS;
}


int pqGetSize(PriorityQueue queue) {
    assert(queue != NULL); 
    return queue->size;
}


// /*----------------------------------------------------------------------
//                                Queue iteration
//  ----------------------------------------------------------------------*/


PQElement pqGetFirst(PriorityQueue queue) 
{
    if(queue == NULL || pqIsEmpty(queue)) {
        return NULL;
    }

    int highest_priority_element_index = findHighestPriorityElementIndex(queue);
    Element highest_priority_element = queue->list_of_elements[highest_priority_element_index];

    queue->list_of_elements[highest_priority_element_index].used = true;
    queue->iterator = &highest_priority_element; // Not needed!
    // printf("%d %d %d \n", queue->list_of_elements[0].used, queue->list_of_elements[1].used ,queue->list_of_elements[2].used);

    return highest_priority_element.element;
}


PQElement pqGetNext(PriorityQueue queue) {
    
    if(queue == NULL || queue->iterator == NULL) {
        return NULL;
    }

    int next_highest_priority_element_index = getNextHighestPriorityElementIndex(queue);
    // printf("%d\n", next_highest_priority_element_index);
    if(next_highest_priority_element_index == ELEMENT_NOT_FOUND) {
        return NULL; //reached the end of the queue
    }

    // printf("%d %d %d \n", queue->list_of_elements[0].used, queue->list_of_elements[1].used ,queue->list_of_elements[2].used);

    Element next_highest_priority_element = queue->list_of_elements[next_highest_priority_element_index];

    queue->list_of_elements[next_highest_priority_element_index].used = true;
    queue->iterator = &next_highest_priority_element; // not needed!
    
    return next_highest_priority_element.element;
}





