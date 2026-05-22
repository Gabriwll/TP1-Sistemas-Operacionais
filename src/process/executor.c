#include <stdio.h>
#include <stdlib.h>

#include "executor.h"

static int ensure_memory_index(PCB *process, int index) {
    return process && index >= 0 && index < process->memory_size;
}

static void dispatch_next_ready(CPU *cpu, Queue *ready_queue) {
    PCB *next = dequeue(ready_queue);

    if (next) {
        load_process_into_cpu(cpu, next);
    } else {
        cpu->current_process = NULL;
    }
}

void move_unblocked_processes(Queue *blocked_queue, Queue *ready_queue, int current_time) {
    QueueNode *current = blocked_queue->front;
    QueueNode *previous = NULL;

    while (current) {
        QueueNode *next = current->next;
        PCB *process = current->process;

        // Cumpriu o tempo de bloqueio, volta para a fila de prontos.
        if (process->blocked_until != UNBLOCKED && process->blocked_until <= current_time) {
            if (previous) {
                previous->next = next;
            } else {
                blocked_queue->front = next;
            }

            if (blocked_queue->rear == current) {
                blocked_queue->rear = previous;
            }

            blocked_queue->size--;
            free(current);

            process->blocked_until = UNBLOCKED;
            process->state = READY;
            enqueue(ready_queue, process);
        } else {
            previous = current;
        }

        current = next;
    }
}

// A instrucao N define o tamanho da memoria simulada do processo.
static ExecutionResult execute_instruction_n(PCB *process, int size) {
    if (size < 0) {
        fprintf(stderr, "Tamanho de memoria invalido: %d\n", size);
        return EXEC_ERROR;
    }

    free(process->memory);
    process->memory = NULL;
    process->memory_size = 0;

    if (size > 0) {
        process->memory = calloc((size_t)size, sizeof(int));
        if (!process->memory) {
            perror("calloc");
            return EXEC_ERROR;
        }
    }

    process->memory_size = size;
    process->pc++;
    return EXEC_OK;
}

ExecutionResult execute_next_instruction(CPU *cpu,
                                         ProcessTable *table,
                                         Queue *ready_queue,
                                         Queue *blocked_queue,
                                         int current_time) {
    PCB *process = cpu->current_process;

    if (!process) {
        dispatch_next_ready(cpu, ready_queue);
        process = cpu->current_process;
    }

    if (!process) {
        printf("[EXECUTOR] Nenhum processo pronto para executar.\n");
        return EXEC_OK;
    }

    if (!process->program || process->pc < 0 || process->pc >= process->program->size) {
        fprintf(stderr, "Processo %d sem instrucao valida no PC %d\n", process->pid, process->pc);
        return EXEC_ERROR;
    }

    // Cada chamada do executor representa uma unidade de CPU consumida.
    Instruction inst = process->program->instructions[process->pc];
    process->cpu_time++;
    process->quantum_used++;

    switch (inst.type) {
        case INST_N:
            return execute_instruction_n(process, inst.arg1);

        // Instrucoes que manipulam a memoria simulada do processo atual.
        case INST_D:
            if (!ensure_memory_index(process, inst.arg1)) {
                fprintf(stderr, "Processo %d tentou declarar indice invalido %d\n", process->pid, inst.arg1);
                return EXEC_ERROR;
            }
            process->memory[inst.arg1] = 0;
            process->pc++;
            return EXEC_OK;

        case INST_V:
            if (!ensure_memory_index(process, inst.arg1)) {
                fprintf(stderr, "Processo %d tentou acessar indice invalido %d\n", process->pid, inst.arg1);
                return EXEC_ERROR;
            }
            process->memory[inst.arg1] = inst.arg2;
            process->pc++;
            return EXEC_OK;

        case INST_A:
            if (!ensure_memory_index(process, inst.arg1)) {
                fprintf(stderr, "Processo %d tentou acessar indice invalido %d\n", process->pid, inst.arg1);
                return EXEC_ERROR;
            }
            process->memory[inst.arg1] += inst.arg2;
            process->pc++;
            return EXEC_OK;

        case INST_S:
            if (!ensure_memory_index(process, inst.arg1)) {
                fprintf(stderr, "Processo %d tentou acessar indice invalido %d\n", process->pid, inst.arg1);
                return EXEC_ERROR;
            }
            process->memory[inst.arg1] -= inst.arg2;
            process->pc++;
            return EXEC_OK;

        case INST_B:
            // Bloqueia o processo atual e deixa a CPU livre para o proximo pronto.
            process->pc++;
            process->state = BLOCKED;
            process->blocked_until = current_time + inst.arg1;
            cpu->current_process = NULL;
            enqueue(blocked_queue, process);
            dispatch_next_ready(cpu, ready_queue);
            return EXEC_BLOCKED;

        case INST_T: {
            // Processo terminou: sai da tabela e nao volta para nenhuma fila.
            int pid = process->pid;
            process->state = TERMINATED;
            cpu->current_process = NULL;
            remove_process(table, pid, 1);
            dispatch_next_ready(cpu, ready_queue);
            return EXEC_TERMINATED;
        }

        default:
            fprintf(stderr, "Instrucao ainda nao implementada para o processo %d\n", process->pid);
            return EXEC_ERROR;
    }
}
