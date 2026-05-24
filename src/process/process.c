#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "process.h"

#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define WHITE   "\033[37m"

int get_quantum_by_priority(int priority) {
    switch (priority) {
        case 0: return 1;
        case 1: return 2;
        case 2: return 4;
        case 3: return 8;
        default: return 1;
    }
}

PCB *create_process(int pid, int ppid, Program *program, int start_time) {
    PCB *process = malloc(sizeof(PCB));
    
    if (!process)
        return NULL;
    
    process->pid = pid;
    process->ppid = ppid;
    
    process->pc = 0;
    
    process->memory = NULL;
    process->memory_size = 0;
    
    process->program = program;
    
    process->priority = 0;
    process->quantum_used = 0;
    
    process->state = READY;
    
    process->start_time = start_time;
    process->cpu_time = 0;
    
    process->blocked_until = UNBLOCKED;
    process->first_run_time = -1;
    
    return process;
}

void destroy_process(PCB *process, int should_free_program) {
    if (!process)
        return;
    
    if (process->memory) {
        free(process->memory);
        process->memory = NULL;
    }
    
    if (should_free_program && process->program) {
        process->program->refcount--;
        if (process->program->refcount == 0) {
            free_program(process->program);
        }
    }
    
    free(process);
}

void print_process(PCB *process, int modo_detalhado) {
    if (!process)
        return;
    
    printf(BOLD GREEN "PID: " RESET "%d | " BOLD GREEN "PPID: " RESET "%d | " BOLD GREEN "PC: " RESET "%d | " BOLD GREEN "Prioridade: " RESET "%d\n", 
            process->pid, process->ppid, process->pc, process->priority);
            
    printf(BOLD GREEN "Estado: " RESET);
    switch (process->state) {
        case READY:     printf(BOLD YELLOW "PRONTO\n" RESET); break;
        case RUNNING:   printf(BOLD GREEN "EXECUTANDO\n" RESET); break;
        case BLOCKED:   printf(BOLD RED "BLOQUEADO"); 
            if (process->blocked_until != UNBLOCKED) {
                printf(BOLD RED " (ate tempo %d)" RESET, process->blocked_until);
            }
            printf("\n");
            break;
        case TERMINATED: printf(BOLD WHITE "TERMINADO\n" RESET); break;
    }
    
    int total_quantum = get_quantum_by_priority(process->priority);
    printf(BOLD GREEN "Tempo CPU: " RESET "%d | " BOLD GREEN "Quantum restante: " RESET "%d (Usado: %d / %d)\n", 
           process->cpu_time, total_quantum - process->quantum_used, process->quantum_used, total_quantum);
           
    if (modo_detalhado) {
        if (process->memory_size == 0) {
            printf(BOLD WHITE "  Memoria: [ vazia ou nao alocada ]\n" RESET);
        } else {
            printf(BOLD WHITE "  Memoria [%d posicoes]: [ " RESET, process->memory_size);
            for (int i = 0; i < process->memory_size; i++) {
                printf(WHITE "%d " RESET, process->memory[i]);
            }
            printf(BOLD WHITE "]\n" RESET);
        }
    }
}

void initialize_process_table(ProcessTable *table) {
    table->size = 0;
    table->next_pid = 0;
    table->free_count = 0;
    table->total_response_time = 0;
    table->response_time_count = 0;
    
    for (int i = 0; i < MAX_PROCESSES; i++) {
        table->table[i] = NULL;
        table->free_pids[i] = 0;
    }
}

int allocate_pid(ProcessTable *table) {
    // Reutiliza PIDs livres primeiro
    if (table->free_count > 0) {
        for (int i = 0; i < MAX_PROCESSES; i++) {
            if (table->free_pids[i]) {
                table->free_pids[i] = 0;
                table->free_count--;
                return i;
            }
        }
    }
    
    // Se não há livres, usa o próximo
    if (table->next_pid < MAX_PROCESSES) {
        return table->next_pid++;
    }
    
    // Sem PIDs disponíveis
    return -1;
}

void free_pid(ProcessTable *table, int pid) {
    if (pid >= 0 && pid < MAX_PROCESSES) {
        table->free_pids[pid] = 1;
        table->free_count++;
    }
}

int add_process(ProcessTable *table, PCB *process) {
    if (!process)
        return 0;
    
    if (table->size >= MAX_PROCESSES)
        return 0;
    
    int pid = process->pid;
    if (pid < 0 || pid >= MAX_PROCESSES)
        return 0;
    
    table->table[pid] = process;
    table->size++;
    
    return 1;
}

PCB *get_process(ProcessTable *table, int pid) {
    if (pid < 0 || pid >= MAX_PROCESSES)
        return NULL;
    
    return table->table[pid];
}

void remove_process(ProcessTable *table, int pid, int should_free_program) {
    if (pid < 0 || pid >= MAX_PROCESSES)
        return;
    
    PCB *process = table->table[pid];
    
    if (!process)
        return;
    
    destroy_process(process, should_free_program);
    
    table->table[pid] = NULL;
    table->size--;
    
    free_pid(table, pid);
}