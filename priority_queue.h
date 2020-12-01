#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <stdbool.h>

/**
* Generic Priority Queue Container
*
* Implements a priority queue container type.
* The priority queue has an internal iterator for external use. For all functions
* where the state of the iterator after calling that function is not stated,
* it is undefined. That means that you cannot assume anything about it.
*
* The following functions are available:
*   pqCreate		    - Creates a new empty priority queue
*   pqDestroy		    - Deletes an existing priority queue and frees all resources
*   pqCopy		        - Copies an existing priority queue
*   pqGetSize		    - Returns the size of a given priority queue
*   pqContains	        - returns whether or not an element exists inside the priority queue.
*   pqInsert	        - Insert an element with a given priority to the queue.
*   				        Duplication in the priority queue is allowed.
*   				        Iterator value is undefined after this operation.
*   pqChangePriority  	- Changes priority of an element with specific priority
*					        Iterator value is undefined after this operation.
*   pqRemove		    - Removes the highest priority element in the queue
*                           Iterator value is undefined after this operation.
*   pqGetFirst	        - Sets the internal iterator to the first element in the priority queue and returns it
*   pqGetNext		    - Advances the internal iterator to the next key and returns it.
*	pqClear		        - Clears the contents of the priority queue. Frees all the elements of
*	 				        the queue using the free function.
* 	PQ_FOREACH	        - A macro for iterating over the priority queue's elements.
*/

/** Type for defining the priority queue */
typedef struct PriorityQueue_t *PriorityQueue;

/** Type used for returning error codes from priority queue functions */
typedef enum PriorityQueueResult_t {
    PQ_SUCCESS,
    PQ_OUT_OF_MEMORY,
    PQ_NULL_ARGUMENT,
    PQ_ELEMENT_DOES_NOT_EXISTS,
    PQ_ITEM_DOES_NOT_EXIST,
    PQ_ERROR
} PriorityQueueResult;

/** Data element data type for priority queue container */
typedef void *PQElement;

/** priority data type for priority queue container */
typedef void *PQElementPriority;

/** Type of function for copying a data element of the priority queue */
typedef PQElement(*CopyPQElement)(PQElement);

/** Type of function for copying a key element of the priority queue */
typedef PQElementPriority(*CopyPQElementPriority)(PQElementPriority);

/** Type of function for deallocating a data element of the priority queue */
typedef void(*FreePQElement)(PQElement);

/** Type of function for deallocating a key element of the priority queue */
typedef void(*FreePQElementPriority)(PQElementPriority);


/**
* Type of function used by the priority queue to identify equal elements.
* This function should return:
* 		true if they're equal;
*		false otherwise;
*/
typedef bool(*EqualPQElements)(PQElement, PQElement);


/**
* Type of function used by the priority queue to compare priorities.
* This function should return:
* 		A positive integer if the first element is greater;
* 		0 if they're equal;
*		A negative integer if the second element is greater.
*/
typedef int(*ComparePQElementPriorities)(PQElementPriority, PQElementPriority);


/**
* pqCreate: Allocates a new empty priority queue.
*
* @param copy_element - Function pointer to be used for copying data elements into
*  	the priority queue or when copying the priority queue.
* @param free_element - Function pointer to be used for removing data elements from
* 		the priority queue
* @param compare_element - Function pointer to be used for comparing elements
* 		inside the priority queue. Used to check if new elements already exist in the priority queue.
* @param copy_priority - Function pointer to be used for copying priority into
*  	the priority queue or when copying the priority queue.
* @param free_priority - Function pointer to be used for removing priority from
* 		the priority queue
* @param compare_priority - Function pointer to be used for comparing elements
* 		inside the priority queue. Used to check if new elements already exist in the priority queue.
* @return
* 	NULL - if one of the parameters is NULL or allocations failed.
* 	A new Map in case of success.
*/
PriorityQueue pqCreate(CopyPQElement copy_element,
                       FreePQElement free_element,
                       EqualPQElements equal_elements,
                       CopyPQElementPriority copy_priority,
                       FreePQElementPriority free_priority,
                       ComparePQElementPriorities compare_priorities);

/**
* pqDestroy: Deallocates an existing priority queue. Clears all elements by using the
* free functions.
*
* @param queue - Target priority queue to be deallocated. If priority queue is NULL nothing will be
* 		done
*/
void pqDestroy(PriorityQueue queue);

/**
* pqCopy: Creates a copy of target priority queue.
* Iterator values for both priority queues are undefined after this operation.
*
* @param queue - Target priority queue.
* @return
* 	NULL if a NULL was sent or a memory allocation failed.
* 	A Priority Queue containing the same elements as queue otherwise.
*/
PriorityQueue pqCopy(PriorityQueue queue);

/**
* pqGetSize: Returns the number of elements in a priority queue
* @param queue - The priority queue which size is requested
* @return
* 	-1 if a NULL pointer was sent.
* 	Otherwise the number of elements in the priority queue.
*/
int pqGetSize(PriorityQueue queue);

/**
* pqContains: Checks if an element exists in the priority queue. The element will be
* considered in the priority queue if one of the elements in the priority queue it determined equal
* using the comparison function used to initialize the priority queue.
*
* @param queue - The priority queue to search in
* @param element - The element to look for. Will be compared using the
* 		comparison function.
* @return
* 	false - if one or more of the inputs is null, or if the key element was not found.
* 	true - if the key element was found in the priority queue.
*/
bool pqContains(PriorityQueue queue, PQElement element);

/**
*   pqInsert: add a specified element with a specific priority.
*   Iterator's value is undefined after this operation.
*
* @param queue - The priority queue for which to add the data element
* @param element - The element which need to be added.
* @param priority - The new priority to associate with the given element.
*      A copy of the element will be inserted as supplied by the copying function
*      which is given at initialization.
* @return
* 	PQ_NULL_ARGUMENT if a NULL was sent as one of the parameters
* 	PQ_OUT_OF_MEMORY if an allocation failed (Meaning the function for copying
* 	an element failed)
* 	PQ_SUCCESS the paired elements had been inserted successfully
*/
PriorityQueueResult pqInsert(PriorityQueue queue, PQElement element, PQElementPriority priority);

/**
*	pqChangePriority: Changes a priority of specific element with a specific priority in the priority queue.
*           If there are multiple same elements with same priority,
*           only the first element's priority needs to be changed.
*           Element that its value has changed is considered as reinserted element.
*			Iterator's value is undefined after this operation
*
* @param queue - The priority queue for which the element from.
* @param element - The element which need to be found and whos priority we want to change.
* @param old_priority - The old priority of the element which need to be changed.
* @param new_priority - The new priority of the element.
* @return
* 	PQ_NULL_ARGUMENT if a NULL was sent as one of the parameters
* 	PQ_OUT_OF_MEMORY if an allocation failed (Meaning the function for copying
* 	an element failed)
* 	PQ_ELEMENT_DOES_NOT_EXISTS if element with old_priority does not exists in the queue.
* 	PQ_SUCCESS the paired elements had been inserted successfully
*/
PriorityQueueResult pqChangePriority(PriorityQueue queue, PQElement element,
                                     PQElementPriority old_priority, PQElementPriority new_priority);

/**
*   pqRemove: Removes the highest priority element from the priority queue.
*   If there are multiple elements with the same highest priority, the first inserted element should be removed first.
*   the elements are removed and deallocated using the free functions supplied at initialization.
*   Iterator's value is undefined after this operation.
*
* @param queue - The priority queue to remove the element from.
* @return
* 	PQ_NULL_ARGUMENT if a NULL was sent to the function.
* 	PQ_SUCCESS the most prioritized element had been removed successfully.
*/
PriorityQueueResult pqRemove(PriorityQueue queue);

/**
*   pqRemoveElement: Removes the highest priority element from the priority queue which have its value equal to element.
*   If there are multiple elements with the same highest priority, the first inserted element should be removed first.
*   the elements are removed and deallocated using the free functions supplied at initialization.
*   Iterator's value is undefined after this operation.
*
* @param queue - The priority queue to remove the elements from.
* @param element
* 	The element to find and remove from the priority queue. The element will be freed using the
* 	free function given at initialization. The priority associated with this element
*   will also be freed using the free function given at initialization.
* @return
* 	PQ_NULL_ARGUMENT if a NULL was sent to the function.
* 	PQ_ELEMENT_DOES_NOT_EXISTS if given element does not exists.
* 	PQ_SUCCESS the most prioritized element had been removed successfully.
*/
PriorityQueueResult pqRemoveElement(PriorityQueue queue, PQElement element);

/**
*	pqGetFirst: Sets the internal iterator (also called current element) to
*	the first element in the priority queue. The internal order derived from the priorities, and the tie-breaker between
*   two equal priorities is the insertion order.
*	Use this to start iterating over the priority queue.
*	To continue iteration use pqGetNext
*
* @param queue - The priority queue for which to set the iterator and return the first element.
* @return
* 	NULL if a NULL pointer was sent or the priority queue is empty.
* 	The first key element of the priority queue otherwise
*/
PQElement pqGetFirst(PriorityQueue queue);

/**
*	pqGetNext: Advances the priority queue iterator to the next element and returns it.
*
* @param queue - The priority queue for which to advance the iterator
* @return
* 	NULL if reached the end of the priority queue, or the iterator is at an invalid state
* 	or a NULL sent as argument
* 	The next element on the priority queue in case of success
*/
PQElement pqGetNext(PriorityQueue queue);

/**
* pqClear: Removes all elements and priorities from target priority queue.
* The elements are deallocated using the stored free functions.
* @param queue
* 	Target priority queue to remove all element from.
* @return
* 	MAP_NULL_ARGUMENT - if a NULL pointer was sent.
* 	MAP_SUCCESS - Otherwise.
*/
PriorityQueueResult pqClear(PriorityQueue queue);

/*!
* Macro for iterating over a priority queue.
* Declares a new iterator for the loop.
*/
#define PQ_FOREACH(type, iterator, queue) \
    for(type iterator = (type) pqGetFirst(queue) ; \
        iterator ;\
        iterator = pqGetNext(queue))

#endif /* PRIORITY_QUEUE_H_ */
