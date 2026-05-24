#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "input/input.h"
#include "process/cpu.h"
#include "process/executor.h"
#include "process/process.h"
#include "process/queue.h"
#include "process/scheduler.h"
#include "threads/supervisor.h"

/* ------------------------------------------------------------------
 * Variáveis globais do simulador
 * ------------------------------------------------------------------
 * current_time  -> representa o tempo global da simulação
 * process_table -> tabela com todos os processos existentes
 * cpu           -> representa a CPU simulada
 * scheduler     -> escalonador utilizado
 * blocked_queue -> fila de processos bloqueados
 * ------------------------------------------------------------------ */

int          current_time = 0;
ProcessTable process_table;
CPU          cpu;
Scheduler    scheduler;
Queue        blocked_queue;

/* ------------------------------------------------------------------
 * Cria o primeiro processo do sistema ("init")
 * ------------------------------------------------------------------
 * Fluxo:
 *   1. Carrega o programa init
 *   2. Aloca um PID
 *   3. Cria o PCB do processo
 *   4. Adiciona o processo na tabela
 *   5. Coloca o processo na CPU
 * ------------------------------------------------------------------ */

static int initialize_first_process(void) {
    Program *init_program = load_program("init");
    if (!init_program) {
        fprintf(stderr, "[GERENCIADOR] nao foi possivel carregar o programa init\n");
        return 0;
    }

    /* Aloca PID para o processo */
    int  init_pid     = allocate_pid(&process_table);

    /* Cria o PCB do processo init */
    PCB *init_process = create_process(init_pid, -1, init_program, current_time);

    /* Verifica falha na criação/inserção */
    if (!init_process || !add_process(&process_table, init_process)) {
        fprintf(stderr, "[GERENCIADOR] nao foi possivel criar o processo inicial\n");
        destroy_process(init_process, 1);
        return 0;
    }

    /* Coloca processo inicial na CPU */
    load_process_into_cpu(&cpu, init_process);
    return 1;
}

int main(void) {
    

     /* ------------------------------------------------------------------
     * Pipe utilizado para comunicação entre:
     *
     *   Processo pai   -> envia comandos
     *   Processo filho -> recebe comandos
     *
     * fd[0] -> leitura
     * fd[1] -> escrita
     * ------------------------------------------------------------------ */
    int fd[2];

    //Cria pipe
    if (pipe(fd) == -1) {
      perror("pipe");
      exit(EXIT_FAILURE);
    }

    /* ------------------------------------------------------------------
     * TODOS os menus são feitos antes do fork.
     *
     * Assim apenas o processo pai interage com o terminal,
     * evitando race condition no stdin.
     * ------------------------------------------------------------------ */

    /* =========================
     * Escolha do escalonador
     * ========================= */

    int choice = 0;

    printf("\nEscolha a politica de escalonamento:\n");
    printf("  1 - MLFQ (Multi-Level Feedback Queue)\n");
    printf("  2 - Round Robin\n");
    printf("Opcao: ");
    scanf("%d", &choice);

    int rr_quantum = 0;
    SchedulerType sched_type = SCHED_MLFQ;
    if (choice == 2) {
        printf("Quantum do Round Robin (0 = usar padrao %d): ", RR_DEFAULT_QUANTUM);
        scanf("%d", &rr_quantum);
        sched_type = SCHED_ROUND_ROBIN;
    }

    /* =========================
      Escolha do modo de entrada
     ========================== */
    int option = 0;
    printf("1 - Terminal\n");
    printf("2 - Arquivo\n");
    printf("Escolha: ");
    scanf("%d", &option);

    //Nome do arquivo de entrada
    char filename[128] = {0};
    if (option == 2) {
      printf("Arquivo de comandos: ");
      scanf("%127s", filename);
    } else if (option != 1) {
      printf("Opcao invalida\n");
      exit(EXIT_FAILURE);
    }

    //Define se dashboard será detalhado
    int modo_detalhado = 1;

    /* ------------------------------------------------------------------
     * Criação do processo filho
     *
     * Pai   -> responsável pela entrada
     * Filho -> responsável pela simulação
     * ------------------------------------------------------------------ */

    pid_t pid = fork();
    if (pid < 0) {
      perror("fork");
      exit(EXIT_FAILURE);
    }

    /* ==============================================================
     * PROCESSO FILHO
     *
     * Responsável pela simulação do sistema operacional
     * ==============================================================
     */

    if (pid == 0) {
      //Filho não escreve no pipe
      close(fd[1]);
      fclose(stdin);

      initialize_process_table(&process_table);
      initialize_cpu(&cpu);
      initialize_scheduler(&scheduler, sched_type, rr_quantum);
      initialize_queue(&blocked_queue, "BLOQUEADO");

      if (!initialize_first_process()) {
        close(fd[0]);
        exit(EXIT_FAILURE);
      }

      /* ----------------------------------------------------------
         * Supervisor controla:
         *   - thread de input
         *   - thread de execução
         *   - thread de impressão
         * ---------------------------------------------------------- */

      Supervisor sv;
      supervisor_init(&sv, fd[0], modo_detalhado);
      //Aguarda término das threads
      supervisor_join(&sv);
      //Libera recursos do supervisor
      supervisor_destroy(&sv);
      //Libera scheduler
      destroy_scheduler(&scheduler);
      //Libera fila de bloqueados
      clear_queue(&blocked_queue);

      close(fd[0]);
      exit(EXIT_SUCCESS);
    }

    /* ==============================================================
     * PROCESSO PAI
     *
     * Responsável pela entrada de comandos
     * ==============================================================
     */
    close(fd[0]);

    if (option == 1) {
        run_input_loop(fd[1]);
    } else {
        run_input_file(filename, fd[1]);
    }

    close(fd[1]);
    wait(NULL);

    return 0;
}