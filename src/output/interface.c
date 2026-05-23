#include <stdio.h>
#include <stdlib.h>

#include "interface.h"



void print_dashboard(int current_time, CPU *cpu, Scheduler *scheduler, Queue *blocked_queue, int modo_detalhado) {
    system("clear"); 

    printf("\n==================================================\n");
    printf("              SISTEMA OPERACIONAL                 \n");
    printf("==================================================\n");
    printf(" Tempo atual: %d\n", current_time);
    printf(" Status CPU: %s\n", cpu->current_process ? "OCUPADA" : "OCIOSA");
    printf("--------------------------------------------------\n");

    if (cpu->current_process) {
        printf("[ PROCESSO EM EXECUCAO ]\n");
        print_process(cpu->current_process, modo_detalhado);
    }

    printf("--------------------------------------------------\n");
    printf("[ FILAS DE PROCESSOS ]\n");

    if (scheduler->type == SCHED_MLFQ) {
        for (int i = 0; i < 4; i++) {
            print_queue(&scheduler->readyQueues[i], current_time, modo_detalhado);
        }
    } else {
        print_queue(&scheduler->rrQueue, current_time, modo_detalhado);
    }
    
    print_queue(blocked_queue, current_time, modo_detalhado);
    
    printf("==================================================\n\n");
}