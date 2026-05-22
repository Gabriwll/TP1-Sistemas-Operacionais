#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "cpu.h"
#include "process.h"
#include "queue.h"

typedef enum {
    EXEC_OK,
    EXEC_BLOCKED,
    EXEC_TERMINATED,
    EXEC_ERROR
} ExecutionResult;

ExecutionResult execute_next_instruction(CPU *cpu,
                                         ProcessTable *table,
                                         Queue *ready_queue,
                                         Queue *blocked_queue,
                                         int current_time);
void move_unblocked_processes(Queue *blocked_queue, Queue *ready_queue, int current_time);

#endif
