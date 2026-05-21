#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#include "input/input.h"
#include "process/process.h"
#include "process/cpu.h"
#include "process/queue.h"

// Variáveis globais do simulador (pra Issue 3)
static int current_time = 0;
static ProcessTable process_table;
static CPU cpu;
static Queue ready_queue;
static Queue blocked_queue;

int main() {
    int fd[2];
    
    if (pipe(fd) == -1) {
        perror("pipe");
        exit(1);
    }
    
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("fork");
        exit(1);
    }
    
    if (pid == 0) {
        // Processo GERENCIADOR
        close(fd[1]);
        
        // Inicializa estruturas
        initialize_process_table(&process_table);
        initialize_cpu(&cpu);
        initialize_queue(&ready_queue, "PRONTO");
        initialize_queue(&blocked_queue, "BLOQUEADO");
        
        // TODO: Carregar programa init e criar primeiro processo (PID 0)
        // Isso será feito na Parte 3
        
        char command;
        
        while (read(fd[0], &command, sizeof(char)) > 0) {
            printf("[GERENCIADOR] recebeu comando: %c (tempo=%d)\n", command, current_time);
            
            // TODO: Processar comando U, I, M
            // Isso será feito na Parte 3
            
            if (command == 'M')
                break;
        }
        
        close(fd[0]);
        exit(0);
    }
    
    else {
        // Processo CONTROLE
        close(fd[0]);
        
        int option;
        
        printf("1 - Terminal\n");
        printf("2 - Arquivo\n");
        printf("Escolha: ");
        scanf("%d", &option);
        
        if (option == 1) {
            run_input_loop(fd[1]);
        }
        else if (option == 2) {
            char filename[128];
            printf("Arquivo de comandos: ");
            scanf("%s", filename);
            run_input_file(filename, fd[1]);
        }
        else {
            printf("Opcao invalida\n");
        }
        
        close(fd[1]);
        wait(NULL);
    }
    
    return 0;
}