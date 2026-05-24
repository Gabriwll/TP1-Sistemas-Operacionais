#include <stdio.h>
#include <stdlib.h>

#include "interface.h"

#define RESET "\033[0m"
#define BOLD "\033[1m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define WHITE "\033[37m"

void print_dashboard(int current_time, CPU *cpu, Scheduler *scheduler,
                     Queue *blocked_queue, int modo_detalhado) {
    system("clear");

    printf(BOLD WHITE
           "\n==================================================\n" RESET);
    printf(BOLD WHITE
           "              SISTEMA OPERACIONAL                 \n" RESET);
    printf(BOLD WHITE
           "==================================================\n" RESET);
    printf(BOLD WHITE " Tempo atual: " BOLD YELLOW "%d\n" RESET, current_time);
    if (cpu->current_process) {
      printf(BOLD WHITE " Status CPU: " BOLD GREEN "OCUPADA\n" RESET);
    } else {
      printf(BOLD WHITE " Status CPU: " BOLD RED "OCIOSA\n" RESET);
    }
    printf(BOLD WHITE
           "--------------------------------------------------\n" RESET);

    if (cpu->current_process) {
      printf(BOLD GREEN "[ PROCESSO EM EXECUCAO ]\n" RESET);
      print_process(cpu->current_process, modo_detalhado);
    }

    printf(BOLD WHITE
           "--------------------------------------------------\n" RESET);
    printf(BOLD BLUE "[ FILAS DE PROCESSOS ]\n" RESET);

    if (scheduler->type == SCHED_MLFQ) {
        for (int i = 0; i < 4; i++) {
            print_queue(&scheduler->readyQueues[i], current_time, modo_detalhado);
        }
    } else {
        print_queue(&scheduler->rrQueue, current_time, modo_detalhado);
    }

    print_queue(blocked_queue, current_time, modo_detalhado);

    printf(BOLD WHITE
           "==================================================\n\n" RESET);
}