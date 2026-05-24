#include "supervisor.h"

void initialize_thread(){

}

/**
* @brief Lines up the thread in list by priority of the thread
* 
* This function takes a list of threads previously initialized by the user and a thread and insert the thread taking care of the priority order.  
* Obviously the list must be pre-ordered by priority, or the insertion can fail.
*
* @param list list of threads already sorted by priority
* @param thread thread that will be inserted in the list
* @return 1 if insertion is succeded, 0 if fail
*/
static int enqueue_thread_by_priority(List* list, Thread thread){
    int position = 0;
    Cell currentCell = List->begin;

    while(currentCell.item.priority <= thread.priority){ //thread should be inserted in the last position of threads with same level of priority
        currentCell = currentCell->next;
        position++;
    }
    
    insertIntoPos(List, thread, position);
}

// 1 -> 3 -> 5 ->
// 2