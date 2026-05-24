#ifndef THREADS_LIST_H
#define THREADS_LIST_H

/*
 * ThreadsList — lista encadeada ordenada por prioridade de thread.
 *
 * O Supervisor usa esta lista para manter as threads ordenadas e decidir
 * qual acorda a seguir.  A prioridade é um inteiro: MENOR valor = MAIOR
 * prioridade (espelha o MLFQ dos processos simulados).
 *
 * Para quebrar a dependência circular (supervisor ↔ threadsList), esta
 * lista armazena apenas o ID (int) de cada thread, não a struct Thread
 * completa.  O Supervisor mapeia IDs para pthread_t / pthread_cond_t
 * internamente.
 */

#include <stdio.h>
#include <stdlib.h>

/* Tipo armazenado na lista: identificador de thread (THREAD_EXEC=0, etc.) */
typedef int ThreadID;

typedef struct ThreadCell {
    ThreadID id;
    int priority;  /* cópia da prioridade para navegação rápida */
    struct ThreadCell *next;
} ThreadCell;

typedef struct {
    ThreadCell *begin;
    ThreadCell *last;
    int size;
} ThreadsList;

/* --- ciclo de vida --- */
ThreadsList tl_init(void);
void tl_destroy(ThreadsList *list);

/* --- inserção ordenada por prioridade (estável: empate mantém ordem FIFO) --- */
void tl_insert(ThreadsList *list, ThreadID id, int priority);

/* --- consulta / remoção --- */
ThreadCell *tl_get(ThreadsList *list, ThreadID id);
ThreadID tl_peek_highest(ThreadsList *list);   /* não remove */
ThreadID tl_pop_highest(ThreadsList *list);    /* remove e retorna */
int tl_remove(ThreadsList *list, ThreadID id);

/* --- utilitários --- */
int tl_is_empty(ThreadsList *list);
void tl_print(ThreadsList *list);

#endif /* THREADS_LIST_H */