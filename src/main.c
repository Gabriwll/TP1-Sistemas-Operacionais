#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "input/input.h"
#include "output/interface.h"
#include "output/printer.h"
#include "process/cpu.h"
#include "process/executor.h"
#include "process/process.h"
#include "process/queue.h"
#include "process/scheduler.h"

static int current_time = 0;
static ProcessTable process_table;
static CPU cpu;
// static Queue ready_queue;
static Scheduler scheduler;
static Queue blocked_queue;

static int initialize_first_process(int pipe_fd) {
  Program *init_program = load_program("init");
  if (!init_program) {
    fprintf(stderr,
            "[GERENCIADOR] nao foi possivel carregar o programa init\n");
    close(pipe_fd);
    return 0;
  }

  int init_pid = allocate_pid(&process_table);
  PCB *init_process = create_process(init_pid, -1, init_program, current_time);
  if (!init_process || !add_process(&process_table, init_process)) {
    fprintf(stderr,
            "[GERENCIADOR] nao foi possivel criar o processo inicial\n");
    destroy_process(init_process, 1);
    close(pipe_fd);
    return 0;
  }

  load_process_into_cpu(&cpu, init_process);
  return 1;
}

static SchedulerType choose_scheduler(int *out_quantum) {
  int choice;
  printf("\nEscolha a politica de escalonamento:\n");
  printf("  1 - MLFQ (Multi-Level Feedback Queue)\n");
  printf("  2 - Round Robin\n");
  printf("Opcao: ");
  scanf("%d", &choice);

  if (choice == 2) {
    printf("Quantum do Round Robin (0 = usar padrao %d): ", RR_DEFAULT_QUANTUM);
    scanf("%d", out_quantum);
    return SCHED_RR;
  }

  *out_quantum = 0;
  return SCHED_MLFQ;
}

int main() {
  int fd[2];

  if (pipe(fd) == -1) {
    perror("pipe");
    exit(1);
  }

  int rr_quantum = 0;
  SchedulerType type = choose_scheduler(&rr_quantum);

  pid_t print_pid = 0; // guarda o ID do processo de impressao atual
  int modo_detalhado = 1;

  pid_t pid = fork();

  if (pid < 0) {
    perror("fork");
    exit(1);
  }

  if (pid == 0) {
    close(fd[1]);

    initialize_process_table(&process_table);
    initialize_cpu(&cpu);
    // initialize_queue(&ready_queue, "PRONTO");
    initialize_scheduler(&scheduler, type, rr_quantum);
    initialize_queue(&blocked_queue, "BLOQUEADO");

    if (!initialize_first_process(fd[0])) {
      exit(1);
    }

    char command;

    while (read(fd[0], &command, sizeof(char)) > 0) {
      printf("[GERENCIADOR] recebeu comando: %c (tempo=%d)\n", command,
             current_time);

      if (command == 'U') {
        ExecutionResult result = execute_next_instruction(
            &cpu, &process_table, &scheduler, &blocked_queue, current_time);
        if (result == EXEC_ERROR) {
          fprintf(stderr, "[GERENCIADOR] erro ao executar instrucao\n");
          break;
        }

        char command;

        while (read(fd[0], &command, sizeof(char)) > 0) {
            printf("[GERENCIADOR] recebeu comando: %c (tempo=%d)\n", command, current_time);

            if (command == 'U') {
                ExecutionResult result = execute_next_instruction(&cpu,
                                                                 &process_table,
                                                                 &scheduler,
                                                                 &blocked_queue,
                                                                 current_time);
                if (result == EXEC_ERROR) {
                    fprintf(stderr, "[GERENCIADOR] erro ao executar instrucao\n");
                    break;
                }

                // Depois do U o relogio anda, entao ja da para acordar bloqueados.
                current_time++;
                move_unblocked_processes(&blocked_queue, &scheduler, current_time);
                scheduler_tick(&scheduler, &cpu, &process_table);
            } else if (command == 'I') {
                print_system_state();
            } else if (command == 'M') {
                print_system_state();
                printf("[GERENCIADOR] encerrando simulacao\n");
                break;
            }
        }
        
        float avg_response = 0.0;
        if (process_table.response_time_count > 0) {
            avg_response = (float)process_table.total_response_time / process_table.response_time_count;
        }
        printf("\n[ESTATISTICAS]\nTempo medio de resposta: %.2f unidades de tempo\n", avg_response);
        
        printf("[GERENCIADOR] encerrando simulacao\n");
        break;
      }
    }

    close(fd[0]);
  } else {
    close(fd[0]);

    int option;

    printf("1 - Terminal\n");
    printf("2 - Arquivo\n");
    printf("Escolha: ");
    scanf("%d", &option);

    if (option == 1) {
      run_input_loop(fd[1]);
    } else if (option == 2) {
      char filename[128];
      printf("Arquivo de comandos: ");
      scanf("%s", filename);
      run_input_file(filename, fd[1]);
    } else {
      printf("Opcao invalida\n");
    }

    close(fd[1]);
    wait(NULL);
  }

  return 0;
}
