#include <stdio.h>

#include "executor.h"

ExecutionResult execute_next_instruction(CPU *cpu,
                                         ProcessTable *table,
                                         Queue *ready_queue,
                                         Queue *blocked_queue,
                                         int current_time) {
    (void)cpu;
    (void)table;
    (void)ready_queue;
    (void)blocked_queue;
    (void)current_time;

    return EXEC_OK;
}
