#ifndef CPU_H
#define CPU_H

#include "process.h"

// CPU SIMPLIFICADA: apenas aponta para o processo em execução
typedef struct {
    PCB *current_process;
} CPU;

void initialize_cpu(CPU *cpu);
void load_process_into_cpu(CPU *cpu, PCB *process);
void save_cpu_state(CPU *cpu);
void clear_cpu(CPU *cpu);
void execute_context_switch(CPU *cpu, PCB *nextProcess, ProcessState reasonState, ProcessTable *table);

// Macros auxiliares para acessar "registradores" via CPU
#define CPU_GET_PC(cpu) ((cpu)->current_process ? (cpu)->current_process->pc : -1)
#define CPU_SET_PC(cpu, val) do { if((cpu)->current_process) (cpu)->current_process->pc = (val); } while(0)
#define CPU_GET_MEMORY(cpu, idx) ((cpu)->current_process ? (cpu)->current_process->memory[idx] : 0)
#define CPU_SET_MEMORY(cpu, idx, val) do { if((cpu)->current_process) (cpu)->current_process->memory[idx] = (val); } while(0)
#define CPU_GET_QUANTUM_USED(cpu) ((cpu)->current_process ? (cpu)->current_process->quantum_used : 0)
#define CPU_SET_QUANTUM_USED(cpu, val) do { if((cpu)->current_process) (cpu)->current_process->quantum_used = (val); } while(0)

#endif
