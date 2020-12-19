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


// struct that defines an "element" in the queue.
// has a type PQElement and a priority
// "used" is for iterating 
typedef struct ElementsStruct {
    PQElement element;
    PQElementPriority priority; 
    //when the iterator has pointed to (used) this Element, used will be set to true
    bool used; 
} Element;                                  

struct PriorityQueue_t {
    // array of the elements of the queue
    Element* list_of_elements;                 

    //number of elements in list_of_elements
    int size; 

    //size of list_of_elements
    int max_size; 

    // function pointers that were defined in the header file
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

// sets the iterator to be in an undefined state
static void clearIterator(PriorityQueue queue) {
    if(queue == NULL) {
        return;
    }
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

/**
 * @brief expands the queue's internal array (list_of_elements) so it can store more elements
 * 
 * @return PriorityQueueResult :
 *         PQ_SUCCESS if the expansion was succesful
 *         PQ_OUT_OF_MEMORY if there was not enough memory
 */
static PriorityQueueResult expand(PriorityQueue queue) {
    assert(queue != NULL);

    // the new max size of the queue
    int new_size = queue->max_size * EXPAND_RATE;

    // reallocated the old list_of_elements with size of new_size
    Element* new_list_of_elements = realloc(queue->list_of_elements, new_size * sizeof(Element));
    if(new_list_of_elements == NULL) {
        return PQ_OUT_OF_MEMORY;
    }

    // if successful then change the old list_of_elements to the new list
    queue->list_of_elements = new_list_of_elements;
    queue->max_size = new_size;

    return PQ_SUCCESS;
}

// moves all of the elements in queue's list_of_elements array left in the index spot
// helper func used in pqChangePriority( ... )
static void moveElementsLeft(PriorityQueue queue, int index) {
    for(int i = index; i < queue->size-1; i++) {
        queue->list_of_elements[i] = queue->list_of_elements[i+1];
    }
}

// the "iterator" is defined when pqGetFirst has been called.
// this means that if pqGetFirst has been called, 
// than atleast one element in "list_of_elements" has been used (used = true)
static bool iteratorIsDefined(PriorityQueue queue) {
    if(pqIsEmpty(queue)) return false;
    for(int i = 0; i < queue->size; i++) {
        if(queue->list_of_elements[i].used == true) {
            return true;
        }
    }
    return false;
}


/*----------------------------------------------------------------------
                             Creation functions
 ----------------------------------------------------------------------*/

// Allocates a new empty priority queue
PriorityQueue pqCreate(CopyPQElement copy_element,
                       FreePQElement free_element,
                       EqualPQElements equal_elements,
                       CopyPQElementPriority copy_priority,
                       FreePQElementPriority free_priority,
                       ComparePQElementPriorities compare_priorities) {
    // check for NULL parameters
    if(copy_element == NULL || free_element == NULL || equal_elements == NULL || 
    copy_priority == NULL || free_priority == NULL || compare_priorities == NULL) {
        return NULL;
    }

    // allocate space for the queue
    PriorityQueue queue = malloc (sizeof(*queue));
    if (queue == NULL) {
        return NULL;
    }

    // allocate space for the list_of_elements, using the initial size defined
    queue->list_of_elements = malloc(INITIAL_SIZE * sizeof(Element));
    if(queue->list_of_elements == NULL) {
        free(queue);
        return NULL;
    }
 
    // the size of the queue when first created is 0
    queue->size = 0;
    queue->max_size = INITIAL_SIZE;
    
    // use the funcs given by the user
    queue->copy_element = copy_element;
    queue->free_element = free_element;
    queue->compare_elements = equal_elements;
    queue->copy_priority = copy_priority;
    queue->free_priority = free_priority;
    queue->compare_priorities = compare_priorities;

    return queue;
}

// Deallocates an existing priority queue. Clears all elements by using the free functions
void pqDestroy (PriorityQueue queue) {
    if(queue == NULL) {
        return;
    }

    // clears the iterator 
    clearIterator(queue);

    // first free the elements inside of the "list_of_elements" 
    while(!pqIsEmpty(queue)) {
        pqRemove(queue);
    }
    
    // first frees the list of elements array, and then the queue itself
    free(queue->list_of_elements);
    free(queue);

    // set queue to NULL so that user knows queue is now deallocated and not for use
    queue = NULL;
}

// Creates a copy of target priority queue.
// Iterator values for both priority queues are undefined after this operation.
PriorityQueue pqCopy(PriorityQueue queue) {
    if(queue == NULL) {
        return NULL;
    }

    // allocate space for a new queue
    PriorityQueue new_queue = malloc(sizeof(*new_queue));
    if(new_queue == NULL) {
        return NULL;
    }

    // create space for the list of elements array
    new_queue->list_of_elements = malloc(queue->max_size * sizeof(Element));
    if(new_queue->list_of_elements == NULL) {
        free(new_queue);
        return NULL;
    }

    // set the size of new_queue to be the same as queue from user
    new_queue->max_size = queue->max_size;

    // size of new_queue should be 0 because we are going to be inserting elements into it one by one, 
    // which will increase its size
    new_queue->size = 0;

    // set the funcs from the user's queue to be in the new_queue
    new_queue->copy_element = queue->copy_element;
    new_queue->free_element = queue->free_element;
    new_queue->compare_elements = queue->compare_elements;
    new_queue->copy_priority = queue->copy_priority;
    new_queue->free_priority = queue->free_priority;
    new_queue->compare_priorities = queue->compare_priorities;

    // insert each element/priority from the user's queue into the new_queue
    for(int i = 0; i < queue->size; i++) {
        // make sure that the insertion was succesful 
        PQElement queue_element = queue->list_of_elements[i].element;
        PQElementPriority queue_element_priority = queue->list_of_elements[i].priority;
        if(pqInsert(new_queue, queue_element, queue_element_priority) == PQ_OUT_OF_MEMORY) {
            pqDestroy(new_queue);
            return NULL;
        }
    }

    // set both the queue's and new_queue's iterators to be undefined 
    clearIterator(queue);
    clearIterator(new_queue);

    return new_queue;
}



/*----------------------------------------------------------------------
                              Queue operations
 ----------------------------------------------------------------------*/

// Returns the number of elements in a priority queue
int pqGetSize(PriorityQueue queue) {
    if(queue == NULL) {
        return -1;
    }
    return queue->size;
}

// Checks if an element exists in the priority queue. The element will be
// considered in the priority queue if one of the elements in the priority queue it determined equal
// using the comparison function used to initialize the priority queue.
bool pqContains(PriorityQueue queue, PQElement element) {
    if(queue == NULL || element == NULL) {
        return NULL;
    }

    // goes through the list_of_elements array to check for a matching element 
    for(int i = 0; i < queue->size; i++) {
        if(queue->compare_elements(queue->list_of_elements[i].element, element)) {
            return true; //matching element found
        }
    }

    return false; //element was not found
}

// Add a specified element with a specific priority.
// NOTE: Iterator's value is undefined after this operation.
PriorityQueueResult pqInsert(PriorityQueue queue, PQElement element, PQElementPriority priority) {
    if(queue == NULL || element == NULL || priority == NULL) {
        return PQ_NULL_ARGUMENT;
    }

    // the list_of_elements array has reached its max size, therefore we need to expand its size
    if(queue->size == queue->max_size) {
        if(expand(queue) == PQ_OUT_OF_MEMORY) {
            return PQ_OUT_OF_MEMORY;
        }
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

    queue->list_of_elements[current_element_index].used = false;

    // queue's size increased by 1
    queue->size++;

    // queue's iterator is undefined after insert
    clearIterator(queue);

    return PQ_SUCCESS;
}

// Changes the priority of specific element with a specific priority in the priority queue.
// See header file for important note.
PriorityQueueResult pqChangePriority(PriorityQueue queue, PQElement element,
                                        PQElementPriority old_priority, PQElementPriority new_priority) {
    if(queue == NULL || element == NULL || old_priority == NULL || new_priority == NULL) {
        return PQ_NULL_ARGUMENT;
    }    

    // first looks for the element in the queue's list_of_elements
    bool element_found = false;
    for(int i = 0; i < queue->size; i++) {
        // compares both the element and the priority
        if(queue->compare_elements(queue->list_of_elements[i].element, element)) {
            if(queue->compare_priorities(queue->list_of_elements[i].priority, old_priority) == 0) {
                element_found = true;

                // the element has been found:
                // free element and priority in Element
                queue->free_element(queue->list_of_elements[i].element); 
                queue->free_priority(queue->list_of_elements[i].priority);
                moveElementsLeft(queue, i);

                queue->size--;
                break;
            }
        }
    }

    if(!element_found) {
        return PQ_ELEMENT_DOES_NOT_EXISTS;
    }

    // insert the element with the new priority
    pqInsert(queue, element, new_priority);

    // the iterator is undefined after changing the priority
    clearIterator(queue);
    return PQ_SUCCESS;

}

// Removes the highest priority element from the priority queue.
// see header file for important notes.
PriorityQueueResult pqRemove(PriorityQueue queue) {
    if(queue == NULL) {
        return PQ_NULL_ARGUMENT;
    }
    if(queue->size == 0) {
        return PQ_SUCCESS;
    }

    // finds the index of the element in queue's list_of_elements that needs to be removed
    int highest_priority_element_index = findHighestPriorityElementIndex(queue);

    // freeing the memory inside Element (both element and priority)
    queue->free_element(queue->list_of_elements[highest_priority_element_index].element);
    queue->free_priority(queue->list_of_elements[highest_priority_element_index].priority);

    // moving over the elements after highest_piority_element_index
    queue->list_of_elements[highest_priority_element_index] = queue->list_of_elements[queue->size - 1];
    queue->size--;

    // iterator is undefined after pqRemove
    clearIterator(queue);

    return PQ_SUCCESS;
}

// Removes the highest priority element from the priority queue which have its value equal to element.
PriorityQueueResult pqRemoveElement(PriorityQueue queue, PQElement element) {
    if(queue == NULL || element == NULL) {
        return PQ_NULL_ARGUMENT;
    }

    // checks that the element is in the queue
    bool element_found = false;
    for(int i = 0; i < queue->size; i++) {
        if(queue->compare_elements(queue->list_of_elements[i].element, element)) {
            element_found = true;

            // free element and priority in Element
            queue->free_element(queue->list_of_elements[i].element); //ERRORS HERE
            queue->free_priority(queue->list_of_elements[i].priority);
            moveElementsLeft(queue, i);
                
            queue->size--;
            break;
        }
    }

    if(!element_found) {
        return PQ_ELEMENT_DOES_NOT_EXISTS;
    }

    // queue's iterator is undefined after removing an element
    clearIterator(queue);

    return PQ_SUCCESS;
}


// /*----------------------------------------------------------------------
//                                Queue iteration
//  ----------------------------------------------------------------------*/

// Sets the internal iterator (also called current element) to
// the first element in the priority queue. The internal order derived from the priorities, 
// and the tie-breaker between two equal priorities is the insertion order.
PQElement pqGetFirst(PriorityQueue queue) {
    if(queue == NULL || pqIsEmpty(queue)) {
        return NULL;
    }

    // finds the high priority element in the queue
    int highest_priority_element_index = findHighestPriorityElementIndex(queue);
    Element highest_priority_element = queue->list_of_elements[highest_priority_element_index];

    // sets that element's used to be true, meaning that it can't be used again
    queue->list_of_elements[highest_priority_element_index].used = true;

    return highest_priority_element.element;
}

// Advances the priority queue iterator to the next element and returns it.
PQElement pqGetNext(PriorityQueue queue) {
    if(queue == NULL || !iteratorIsDefined(queue)) {
        return NULL;
    }

    // looks for the high priority element in the queue that has not been used yet
    int next_highest_priority_element_index = getNextHighestPriorityElementIndex(queue);

    //reached the end of the queue
    if(next_highest_priority_element_index == ELEMENT_NOT_FOUND) {
        return NULL; 
    }

    Element next_highest_priority_element = queue->list_of_elements[next_highest_priority_element_index];

    queue->list_of_elements[next_highest_priority_element_index].used = true;
    
    return next_highest_priority_element.element;
}

// Removes all elements and priorities from target priority queue.
// NOTE: The elements are deallocated using the stored free functions.
PriorityQueueResult pqClear(PriorityQueue queue) {
    if(queue == NULL) {
        return PQ_NULL_ARGUMENT;
    }

    // removes the elements from the queue until there are none left
    while(!pqIsEmpty(queue)) {
        pqRemove(queue);
    }

    // iterator is undefined after this operation 
    clearIterator(queue);

    return PQ_SUCCESS;
}




