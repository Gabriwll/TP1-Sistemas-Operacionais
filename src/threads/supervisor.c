#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "supervisor.h"
#include "../process/cpu.h"
#include "../process/executor.h"
#include "../process/process.h"
#include "../process/queue.h"
#include "../process/scheduler.h"
#include "../output/printer.h"

/* ------------------------------------------------------------------
 * Variáveis globais compartilhadas pelo simulador
 * ------------------------------------------------------------------ */

extern int current_time;
extern ProcessTable process_table;
extern CPU cpu;
extern Scheduler scheduler;
extern Queue blocked_queue;

/* ------------------------------------------------------------------
 * Finaliza o processamento de um comando.
 *
 * Funções:
 *   - marca o comando como concluído
 *   - limpa pending_cmd
 *   - acorda a thread_input
 *
 * IMPORTANTE:
 * quando chamar o mutex deve estar travado.
 * ------------------------------------------------------------------ */

static void notify_done(Supervisor *sv) {
    /* Marca que o comando terminou */
    sv->cmd_done = 1;
    /* Remove comando pendente */
    sv->pending_cmd = CMD_NONE;
    /* Acorda thread_input */
    pthread_cond_signal(&sv->cond_done);
    /* Sinaliza novamente usando supervisor_signal */  
    supervisor_signal(sv);               
}

/* ------------------------------------------------------------------
 * Decide qual thread deve acordar dependendo do comando atual.
 *
 * CMD_U -> thread_execucao
 * CMD_I -> thread_impressao
 * CMD_M -> thread_impressao
 * NONE  -> thread_input
 * ------------------------------------------------------------------ */

void supervisor_signal(Supervisor *sv) {

    /* Não sinaliza nada se o sistema estiver encerrando */
    if (!sv->running)
        return;

    /* Percorre lista de threads prontas */    
    ThreadCell *cell = sv->ready_threads.begin;

    while (cell) {

        int id = cell->id;
        /* Comando U -> executar instrução */
        if (id == THREAD_EXEC  && sv->pending_cmd == CMD_U) {
            pthread_cond_signal(&sv->cond[THREAD_EXEC]);
            return;
        }

        /* Comandos I ou M -> impressão */
        if (id == THREAD_PRINT && (sv->pending_cmd == CMD_I || sv->pending_cmd == CMD_M)) {
            pthread_cond_signal(&sv->cond[THREAD_PRINT]);
            return;
        }

        /* Sem comando pendente -> libera input */
        if (id == THREAD_INPUT && sv->pending_cmd == CMD_NONE) {
            pthread_cond_signal(&sv->cond[THREAD_INPUT]);
            return;
        }

        cell = cell->next;
    }
}

/* ------------------------------------------------------------------
 * THREAD DE EXECUÇÃO
 *
 * Responsável por:
 *   - executar instruções
 *   - avançar tempo
 *   - mover processos desbloqueados
 *   - atualizar scheduler
 * ------------------------------------------------------------------ */

static void *thread_execucao(void *arg) {

    ThreadArg *ta = (ThreadArg *)arg;
    Supervisor *sv = ta->sv;

    /* Entra na região crítica */
    pthread_mutex_lock(&sv->mutex);

    while (sv->running) {

        /* Espera comando U */
        while (sv->running && sv->pending_cmd != CMD_U)
            pthread_cond_wait(&sv->cond[THREAD_EXEC], &sv->mutex);

        if (!sv->running) break;

        printf("[EXEC] instrucao (tempo=%d)\n", current_time);
        fflush(stdout);

        /* Executa próxima instrução */
        ExecutionResult result = execute_next_instruction(&cpu, &process_table, &scheduler, &blocked_queue, current_time);

        /* Tratamento de erro */
        if (result == EXEC_ERROR) {
            fprintf(stderr, "[EXEC] erro ao executar instrucao\n");
            sv->running = 0;
            pthread_cond_signal(&sv->cond_done);

            /* Libera todas as threads */
            for (int i = 0; i < NUM_THREADS; i++)
                pthread_cond_signal(&sv->cond[i]);
            break;
        }

        /* Avança tempo global */
        current_time++;

        /* Move processos desbloqueados */
        move_unblocked_processes(&blocked_queue, &scheduler, current_time);
        /* Atualiza scheduler */
        scheduler_tick(&scheduler, &cpu, &process_table);

        printf("[EXEC] tempo agora: %d | PID em execucao: %d\n", current_time, cpu.current_process ? cpu.current_process->pid : -1);
        fflush(stdout);

        notify_done(sv);
    }

    pthread_mutex_unlock(&sv->mutex);
    return NULL;
}

/* ------------------------------------------------------------------
 * THREAD DE IMPRESSÃO
 *
 * Responsável por:
 *   - imprimir dashboard
 *   - mostrar estatísticas
 *   - finalizar simulação
 * ------------------------------------------------------------------ */

static void *thread_impressao(void *arg) {

    ThreadArg *ta = (ThreadArg *)arg;
    Supervisor *sv = ta->sv;

    pthread_mutex_lock(&sv->mutex);

    while (sv->running) {

        /* Espera comandos I ou M */
        while (sv->running && sv->pending_cmd != CMD_I && sv->pending_cmd != CMD_M)
            pthread_cond_wait(&sv->cond[THREAD_PRINT], &sv->mutex);

        if (!sv->running) break;

        /* Guarda comando atual */
        char cmd = sv->pending_cmd;

        /* ----------------------------------------------------------
         * Libera mutex durante impressão
         *
         * Isso evita bloquear a thread_input enquanto o processo
         * de impressão executa.
         * ---------------------------------------------------------- */
        pthread_mutex_unlock(&sv->mutex);
        spawn_print_process(&sv->print_pid, current_time, &cpu, &scheduler, &blocked_queue, sv->modo_detalhado);

        /* Aguarda processo filho de impressão terminar */
        if (sv->print_pid > 0) {
            waitpid(sv->print_pid, NULL, 0);
            sv->print_pid = 0;
        }
        pthread_mutex_lock(&sv->mutex);

         /* ----------------------------------------------------------
         * Comando M -> mostra métricas e encerra simulação
         * ---------------------------------------------------------- */
        if (cmd == CMD_M) {

            float avg = 0.0f;

            /* Calcula tempo médio de resposta */
            if (process_table.response_time_count > 0) 
                avg = (float)process_table.total_response_time /process_table.response_time_count;

            printf("\n[ESTATISTICAS] Tempo medio de resposta: %.2f ut\n", avg);
            printf("[GERENCIADOR] encerrando simulacao\n");
            fflush(stdout);

            /* Finaliza sistema */
            sv->running = 0;
            sv->pending_cmd = CMD_NONE;
            pthread_cond_signal(&sv->cond_done);

            /* Libera todas as threads */
            for (int i = 0; i < NUM_THREADS; i++)
                pthread_cond_signal(&sv->cond[i]);
            break;
        }

        notify_done(sv);
    }

    pthread_mutex_unlock(&sv->mutex);
    return NULL;
}

/* ------------------------------------------------------------------
 * THREAD DE INPUT
 *
 * Fluxo:
 *   1. Espera comando anterior terminar
 *   2. Mostra prompt
 *   3. Lê pipe
 *   4. Atualiza pending_cmd
 *   5. Sinaliza thread correspondente
 *   6. Espera processamento terminar
 * ------------------------------------------------------------------ */

static void *thread_input(void *arg) {

    ThreadArg *ta = (ThreadArg *)arg;
    Supervisor *sv = ta->sv;

    pthread_mutex_lock(&sv->mutex);

    while (sv->running) {
        
        /* Espera término do comando anterior */
        while (sv->running && sv->pending_cmd != CMD_NONE)
            pthread_cond_wait(&sv->cond[THREAD_INPUT], &sv->mutex);

        if (!sv->running) break;

        /* ----------------------------------------------------------
         * Leitura sem mutex
         *
         * Evita bloquear outras threads enquanto espera entrada.
         * ---------------------------------------------------------- */

        pthread_mutex_unlock(&sv->mutex);
        printf("\nDigite comando (U/I/M): ");
        fflush(stdout);

        char cmd = CMD_NONE;
        ssize_t n = read(sv->pipe_read_fd, &cmd, sizeof(char));

        pthread_mutex_lock(&sv->mutex);

        if (n <= 0) {
            sv->running = 0;
            pthread_cond_signal(&sv->cond_done);
            for (int i = 0; i < NUM_THREADS; i++)
                pthread_cond_signal(&sv->cond[i]);
            break;
        }

        printf("[INPUT] comando: %c (tempo=%d)\n", cmd, current_time);
        fflush(stdout);

        /* Define novo comando pendente */
        sv->cmd_done = 0;
        sv->pending_cmd = cmd;
        supervisor_signal(sv);

        /* Espera processamento terminar */
        while (sv->running && !sv->cmd_done)
            pthread_cond_wait(&sv->cond_done, &sv->mutex);

        sv->cmd_done = 0;
    }

    pthread_mutex_unlock(&sv->mutex);
    return NULL;
}

/* ------------------------------------------------------------------
 * Inicializa supervisor e cria threads
 * ------------------------------------------------------------------ */

void supervisor_init(Supervisor *sv, int pipe_read_fd, int modo_detalhado) {
    /* Inicializa atributos básicos */
    sv->pipe_read_fd = pipe_read_fd;
    sv->modo_detalhado = modo_detalhado;
    sv->pending_cmd = CMD_NONE;
    sv->cmd_done = 0;
    sv->running = 1;
    sv->print_pid = 0;

    pthread_mutex_init(&sv->mutex, NULL);
    pthread_cond_init(&sv->cond_done, NULL);

    for (int i = 0; i < NUM_THREADS; i++)
        pthread_cond_init(&sv->cond[i], NULL);

    /* ----------------------------------------------------------
     * Lista de prioridades:
     *
     * EXEC  -> prioridade 0
     * PRINT -> prioridade 1
     * INPUT -> prioridade 2
     * ---------------------------------------------------------- */

    sv->ready_threads = tl_init();
    tl_insert(&sv->ready_threads, THREAD_EXEC,  THREAD_EXEC);
    tl_insert(&sv->ready_threads, THREAD_PRINT, THREAD_PRINT);
    tl_insert(&sv->ready_threads, THREAD_INPUT, THREAD_INPUT);

    /* Inicializa argumentos das threads */
    for (int i = 0; i < NUM_THREADS; i++) {
        sv->args[i].sv = sv;
        sv->args[i].id = i;
    }

    /* Vetor de funções executadas pelas threads */
    static void *(*fns[NUM_THREADS])(void *) = {
        thread_execucao,
        thread_impressao,
        thread_input
    };

    /* Cria threads */
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&sv->threads[i], NULL, fns[i], &sv->args[i]) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    /* Libera thread_input para iniciar */
    pthread_mutex_lock(&sv->mutex);
    supervisor_signal(sv);
    pthread_mutex_unlock(&sv->mutex);
}

/* ------------------------------------------------------------------
 * Aguarda todas as threads terminarem
 * ------------------------------------------------------------------ */

void supervisor_join(Supervisor *sv) {
    for (int i = 0; i < NUM_THREADS; i++)
        pthread_join(sv->threads[i], NULL);
}

/* ------------------------------------------------------------------
 * Libera recursos do supervisor
 * ------------------------------------------------------------------ */

void supervisor_destroy(Supervisor *sv) {

    /* Libera lista de threads */
    tl_destroy(&sv->ready_threads);
    /* Destroi mutex */
    pthread_mutex_destroy(&sv->mutex);
    /* Destroi condição principal */
    pthread_cond_destroy(&sv->cond_done);
    /* Destroi condições individuais */
    for (int i = 0; i < NUM_THREADS; i++)
        pthread_cond_destroy(&sv->cond[i]);
}