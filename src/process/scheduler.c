#include <stdio.h>
#include <stdlib.h>

#include "scheduler.h"

// MLFQ scheduler implementation

void initialize_scheduler(Scheduler *scheduler){
    if(scheduler == NULL)
        return;
    
    initialize_queue(&scheduler->readyQueues[0], "PRIORIDADE 0");
    initialize_queue(&scheduler->readyQueues[1], "PRIORIDADE 1");
    initialize_queue(&scheduler->readyQueues[2], "PRIORIDADE 2");
    initialize_queue(&scheduler->readyQueues[3], "PRIORIDADE 3");
}

void schedule_process(Scheduler *scheduler, PCB *process){
    if(scheduler == NULL || process == NULL)
        return;
    
    // garante limites na prioridade dos processos
    if(process->priority < 0){
        process->priority = 0;
    }
    if(process->priority > 3){
        process->priority = 3;
    }

    enqueue(&scheduler->readyQueues[process->priority], process);
}

/* FUNÇÕES COMPLEMENTARES */

int get_quantum_limit(int priority){
    int quantums[] = {1, 2, 4, 8};
    if(priority >= 0 && priority <= 3){
        return quantums[priority];
    }
    return 1;
}

PCB* get_next_ready_process(Scheduler *scheduler){
    for(int i=0; i<4; i++){
        if(!is_queue_empty(&scheduler->readyQueues[i])){
            return dequeue(&scheduler->readyQueues[i]);
        }
    }
    return NULL;
}

void scheduler_tick(Scheduler *scheduler, CPU *cpu){
    if(scheduler == NULL || cpu == NULL)
        return;
    
    // Com a cpu ociosa fazemos
    if(cpu->current_process == NULL){
        PCB *next = get_next_ready_process(scheduler);
        if(next != NULL){
            execute_context_switch(cpu, next, RUNNING);
        }
        return;
    }

    // existe processo rodando na cpu
    PCB *current = cpu->current_process;
    current->quantum_used++;

    if(current->quantum_used >= get_quantum_by_priority(current->priority)){
        if(current->priority < 3){
            current->priority++;
        }

        PCB *next = get_next_ready_process(scheduler);

        if(next != NULL){
            execute_context_switch(cpu, next, READY);
            schedule_process(scheduler, current);
        } else{
            current->quantum_used = 0;
        }
    }
}

void destroy_scheduler(Scheduler *scheduler){
    if(scheduler == NULL)
        return;
    
    for(int i=0; i<4; i++){
        clear_queue(&scheduler->readyQueues[i]);
    }
}