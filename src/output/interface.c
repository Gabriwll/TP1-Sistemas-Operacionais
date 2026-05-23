#include <stdio.h>
#include <stdlib.h>

#include "interface.h"

void print_memory_dump(PCB *process) {
    if (!process || process->memory_size == 0) {
        printf("  Memoria: [ vazia ou nao alocada ]\n");
        return;
    }

    printf("  Memoria [%d posicoes]: ", process->memory_size);
    printf("[ ");
    for (int i = 0; i < process->memory_size; i++) {
        printf("%d ", process->memory[i]);
    }
    printf("]\n");
}

void print_dashboard(int current_time, CPU *cpu, Scheduler *scheduler, Queue *blocked_queue) {
    system("clear"); 

    printf("\n==================================================\n");
    printf("              SISTEMA OPERACIONAL                 \n");
    printf("==================================================\n");
    printf(" Tempo atual: %d\n", current_time);
    printf(" Status CPU: %s\n", cpu->current_process ? "OCUPADA" : "OCIOSA");
    printf("--------------------------------------------------\n");

    if (cpu->current_process) {
        printf("[ PROCESSO EM EXECUCAO ]\n");
        print_process(cpu->current_process);
        print_memory_dump(cpu->current_process); 
    }

    printf("--------------------------------------------------\n");
    printf("[ FILAS DE PROCESSOS ]\n");

    if (scheduler->type == SCHED_MLFQ) {
        for (int i = 0; i < 4; i++) {
            print_queue(&scheduler->readyQueues[i]);
        }
    } else {
        print_queue(&scheduler->rrQueue);
    }
    
    print_queue(blocked_queue);
    
    printf("==================================================\n\n");
}