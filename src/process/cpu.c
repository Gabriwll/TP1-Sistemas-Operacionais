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

void execute_context_switch(CPU *cpu, PCB *nextProcess, ProcessState reasonState, ProcessTable *table){
    // Função independente para utilização de troca de contexto para o escalonador
    // Realiza a troca de contexto, utilizando a tabela de processos para atualizar os estados dos processos envolvidos.

    if(cpu == NULL)
        return;
    
    // Salva o estado do processo atual, se houver, e atualiza seu estado de acordo com a razão da troca.
    if(cpu->current_process != NULL){
        int pid = cpu->current_process->pid;
        PCB *entry = get_process(table, pid); // Busca o processo na tabela para atualizar seu estado

        if(entry != NULL){
            if(reasonState != TERMINATED){
                // Se o processo não terminou, atualiza seu estado e reseta o quantum usado.
                entry->quantum_used = 0;
                entry->state = reasonState;
            } else{
                // Se o processo terminou marca como TERMINATED.    
                entry->state = TERMINATED;
            }
        }
        cpu->current_process = NULL;
    }

    // Se houver um próximo processo para rodar, atualiza o estado e aponta a CPU para ele.
    if(nextProcess != NULL){
        PCB *entry = get_process(table, nextProcess->pid);

        if(entry != NULL){
            entry->state = RUNNING;
            cpu->current_process = entry;
        }
    }
}