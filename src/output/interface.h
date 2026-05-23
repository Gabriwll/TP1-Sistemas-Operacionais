#ifndef INTERFACE_H
#define INTERFACE_H

#include "../process/cpu.h"
#include "../process/scheduler.h"
#include "../process/queue.h"

void print_dashboard(int current_time, CPU *cpu, Scheduler *scheduler, Queue *blocked_queue);

void print_memory_dump(PCB *process);

#endif