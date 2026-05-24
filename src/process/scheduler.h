#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "queue.h"
#include "cpu.h"

#define MLFQ_LEVELS 4
#define RR_DEFAULT_QUANTUM 4

typedef enum{
    SCHED_MLFQ,
    SCHED_RR
}SchedulerType;

typedef struct{
    SchedulerType type;

    // type = SCHED_MLFQ
    Queue readyQueues[MLFQ_LEVELS];

    // type = SCHED_RR
    Queue rrQueue;
    int rrQuantum;
} Scheduler;

void initialize_scheduler(Scheduler *scheduler, SchedulerType type, int rr_quantum);
void schedule_process(Scheduler *scheduler, PCB *process);
PCB* get_next_ready_process(Scheduler *scheduler);
void scheduler_tick(Scheduler *scheduler, CPU *cpu, ProcessTable *table);
void destroy_scheduler(Scheduler *scheduler);

#endif