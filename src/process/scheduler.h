#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "queue.h"
#include "cpu.h"

typedef struct{
    Queue readyQueues[4];
} Scheduler;

void initialize_scheduler(Scheduler *scheduler);
void schedule_process(Scheduler *scheduler, PCB *process);
PCB* get_next_ready_process(Scheduler *scheduler);
void scheduler_tick(Scheduler *scheduler, CPU *cpu);
void destroy_scheduler(Scheduler *scheduler);

#endif