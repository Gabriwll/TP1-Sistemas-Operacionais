#include "printer.h"
#include "interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void spawn_print_process(pid_t *print_pid, int current_time, CPU *cpu,
                          Scheduler *scheduler, Queue *blocked_queue,
                         int modo_detalhado) {
    // verifica se o processo anterior ainda está vivo
    if (*print_pid > 0) {
        int status;
        // WNOHANG impede que o pai fique parado esperando
        if (waitpid(*print_pid, &status, WNOHANG) == 0) {
            printf("[GERENCIADOR] Aviso: Impressao em curso, aguarde...\n");
            return;
        }
    }

    *print_pid = fork();

    if (*print_pid < 0) {
        perror("Erro ao criar processo de impressao");
    } else if (*print_pid == 0) {
        // filho chama a interface e morre
        print_dashboard(current_time, cpu, scheduler, blocked_queue,
                    modo_detalhado);
        exit(0);
    }
    // pai retorna para continuar a simulação
}