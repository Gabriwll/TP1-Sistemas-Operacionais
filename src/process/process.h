#ifndef PROCESS_H
#define PROCESS_H

#include "../input/parser.h"

#define MAX_PROCESSES 256
#define MAX_PRIORITY 3
#define UNBLOCKED -1

typedef enum {
    READY,
    RUNNING,
    BLOCKED,
    TERMINATED
} ProcessState;

typedef struct PCB {
    int pid;
    int ppid;
    
    int pc;                         // contador de programa
    
    int *memory;                    // vetor de variáveis inteiras
    int memory_size;                // tamanho do vetor
    
    Program *program;               // programa do processo
    
    int priority;                   // 0 = mais alta, 3 = mais baixa
    int quantum_used;               // tempo usado na fatia atual
    
    ProcessState state;
    
    int start_time;                 // tempo de criação
    int cpu_time;                   // tempo total de CPU usado
    
    int blocked_until;              // tempo em que desbloqueia (UNBLOCKED se não bloqueado)
} PCB;

typedef struct {
    PCB *table[MAX_PROCESSES];
    int size;
    int next_pid;
    int free_pids[MAX_PROCESSES];   // bitmap de PIDs livres
    int free_count;
} ProcessTable;

// Funções de processo
PCB *create_process(int pid, int ppid, Program *program, int start_time);
void destroy_process(PCB *process, int should_free_program);
void print_process(PCB *process);

// Funções da tabela de processos
void initialize_process_table(ProcessTable *table);
int allocate_pid(ProcessTable *table);
void free_pid(ProcessTable *table, int pid);
int add_process(ProcessTable *table, PCB *process);
PCB *get_process(ProcessTable *table, int pid);
void remove_process(ProcessTable *table, int pid, int should_free_program);

// Função auxiliar para quantum por prioridade
int get_quantum_by_priority(int priority);

#endif