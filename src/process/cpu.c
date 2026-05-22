#include <stdio.h>
#include <stdlib.h>

#include "cpu.h"

void initialize_cpu(CPU *cpu) {
    cpu->current_process = NULL;
}

void load_process_into_cpu(CPU *cpu, PCB *process) {
    if (!process)
        return;
    
    // Se já tem um processo diferente, salva estado antes de trocar
    if (cpu->current_process && cpu->current_process != process) {
        save_cpu_state(cpu);
        cpu->current_process->state = READY;
    }
    
    // Apenas aponta para o novo processo
    cpu->current_process = process;
    process->state = RUNNING;
}

void save_cpu_state(CPU *cpu) {
    // Com a CPU simplificada, precisa ser salvo
    // porque a CPU trabalha diretamente sobre o PCB.
    // Mas eu tinha colocado do jeito mais difícil antes, então quero deixar aqui pra caso precise voltar atrás. 
    //E pq eu gosto de ter uma função explícita pra isso, mesmo que seja trivial. Ainda que só pq me deu trabalho antes
    (void)cpu;
}

void clear_cpu(CPU *cpu) {
    if (cpu->current_process) {
        cpu->current_process->state = READY;
        cpu->current_process = NULL;
    }
}

void execute_context_switch(CPU *cpu, PCB *nextProcess, ProcessState reasonState){
    // Função independente para utilização de troca de contexto para o escalonador

    if(cpu == NULL)
        return;
    
    // Salvar estado do processo atual
    if(cpu->current_process != NULL){
        cpu->current_process->quantum_used = 0;

        // Modifica estado para motivo determinado pelo escalonador (Ready OU Blocked)
        cpu->current_process->state = reasonState;

        cpu->current_process = NULL;
    }

    // Copia estado do escalonado para cpu
    if(nextProcess != NULL){
        cpu->current_process = nextProcess;
        nextProcess->state = RUNNING;
    }
}
