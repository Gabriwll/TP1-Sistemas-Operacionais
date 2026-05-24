#ifndef SUPERVISOR_H
#define SUPERVISOR_H

#include <pthread.h>
#include <semaphore.h>

#include ".../Lists/threadsList.h" //eu acredito que esse erro aqui seja só um bug de visualização do vscode



typedef struct Thread{
    pthread_t thread;
    
    pthread_mutex_t ;
    pthread_con_t ;

    int priority;
}Thread; //TODO: The thread may be implemented as List type

void initialize_thread();

static int enqueue_thread_by_priority(List* list, Thread thread);

#endif //SUPERVISOR_H