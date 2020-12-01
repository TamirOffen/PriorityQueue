#include <stdio.h>
#include "priority_queue.h"


struct PriorityQueue_t {

PQElement* elements;
PQElementPriority* priorities;

int size;
int maxsize;
int iterator;

CopyPQElement copy_element;
FreePQElement free_element;
EqualPQElements compare_elements;

CopyPQElementPriority copy_priority;
FreePQElementPriority free_priority;
ComparePQElementPriorities compare_priorities;

};