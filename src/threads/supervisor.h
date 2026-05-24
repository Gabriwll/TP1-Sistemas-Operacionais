#ifndef SUPERVISOR_H
#define SUPERVISOR_H

#include <pthread.h>
#include <sys/types.h>
#include "../List/threadList.h"

#define THREAD_EXEC   0
#define THREAD_PRINT  1
#define THREAD_INPUT  2
#define NUM_THREADS   3

#define CMD_NONE  '\0'
#define CMD_U     'U'
#define CMD_I     'I'
#define CMD_M     'M'

typedef struct Supervisor Supervisor;

typedef struct {
    Supervisor *sv;
    int id;
} ThreadArg;

struct Supervisor {
    pthread_mutex_t mutex;
    pthread_cond_t cond[NUM_THREADS];
    pthread_cond_t cond_done;   /* sinaliza que o comando foi processado */

    ThreadsList ready_threads;

    char pending_cmd;
    int cmd_done;    /* 1 = comando processado, thread_input pode pedir o próximo */
    int running;

    int pipe_read_fd;
    int modo_detalhado;

    pid_t print_pid;

    ThreadArg args[NUM_THREADS];
    pthread_t threads[NUM_THREADS];
};

void supervisor_init(Supervisor *sv, int pipe_read_fd, int modo_detalhado);
void supervisor_join(Supervisor *sv);
void supervisor_destroy(Supervisor *sv);
void supervisor_signal(Supervisor *sv);

#endif /* SUPERVISOR_H */

//ARQUITETURA ESCOLHIDA
/*
Processo Controle (pai)
  └─ lê stdin → escreve no pipe

Processo Gerenciador (filho)
  ├─ thread_input     (prioridade 2)
  ├─ thread_execucao  (prioridade 0)
  └─ thread_impressao (prioridade 1)
       └─ fork() → Processo Impressão (neto) → termina
*/