#include "threadList.h"

/* ------------------------------------------------------------------
 * Inicializa uma lista de threads vazia.
 *
 * begin -> início da lista
 * last  -> último elemento
 * size  -> quantidade de elementos
 * ------------------------------------------------------------------ */

ThreadsList tl_init(void) {
    ThreadsList list;
    list.begin = NULL;
    list.last = NULL;
    list.size = 0;
    return list;
}

/* ------------------------------------------------------------------
 * Libera toda a memória utilizada pela lista.
 *
 * Percorre célula por célula liberando os nós.
 * ------------------------------------------------------------------ */

void tl_destroy(ThreadsList *list) {
    //Ponteiro auxiliar para percorrer lista
    ThreadCell *cur = list->begin;
    while (cur) {
        //Guarda proximo elemento antes do free
        ThreadCell *next = cur->next;
        //Libera celula atual
        free(cur);
        //Avança na lista
        cur = next;
    }
    list->begin = NULL;
    list->last = NULL;
    list->size = 0;
}

/* ------------------------------------------------------------------
 * Insere uma thread na lista de forma ordenada por prioridade.
 *
 * MENOR valor de prioridade = MAIOR prioridade
 *
 * A inserção é estável:
 * threads com mesma prioridade mantêm ordem FIFO.
 * ------------------------------------------------------------------ */

void tl_insert(ThreadsList *list, ThreadID id, int priority) {
    ThreadCell *cell = malloc(sizeof(ThreadCell));
    if (!cell) { perror("malloc ThreadCell"); return; }
    cell->id = id;
    cell->priority = priority;
    cell->next = NULL;

    if (!list->begin) {
        list->begin = cell;
        list->last = cell;
        list->size++;
        return;
    }

    /* --------------------------------------------------------------
     * Inserção no início da lista
     *
     * Ocorre quando a nova prioridade é maior
     * (numericamente menor).
     * -------------------------------------------------------------- */

    if (priority < list->begin->priority) {
        cell->next  = list->begin;
        list->begin = cell;
        list->size++;
        return;
    }

    /* --------------------------------------------------------------
     * Busca posição correta da inserção
     * -------------------------------------------------------------- */
    ThreadCell *prev = list->begin;
    ThreadCell *cur = list->begin->next;

    while (cur && cur->priority <= priority) {
        prev = cur;
        cur = cur->next;
    }

    /* --------------------------------------------------------------
     * Insere entre prev e cur
     * -------------------------------------------------------------- */
    cell->next = cur;
    prev->next = cell;

    if (!cur)   /* inseriu no final */
        list->last = cell;

    list->size++;
}

/* ------------------------------------------------------------------
 * Busca uma thread pelo ID.
 *
 * Complexidade:
 * O(n)
 * ------------------------------------------------------------------ */
ThreadCell *tl_get(ThreadsList *list, ThreadID id) {
    ThreadCell *cur = list->begin;
    while (cur) {
        if (cur->id == id) return cur;
        cur = cur->next;
    }
    return NULL;
}

/* ------------------------------------------------------------------
 * Retorna o ID da thread de maior prioridade
 * sem remover da lista.
 *
 * Como a lista é ordenada, o primeiro elemento
 * sempre possui a maior prioridade.
 * ------------------------------------------------------------------ */
ThreadID tl_peek_highest(ThreadsList *list) {
    if (!list->begin) return -1;
    return list->begin->id;
}

/* ------------------------------------------------------------------
 * Remove e retorna a thread de maior prioridade.
 *
 * Remove sempre o primeiro elemento da lista.
 * ------------------------------------------------------------------ */
ThreadID tl_pop_highest(ThreadsList *list) {
    if (!list->begin) return -1;

    ThreadCell *cell = list->begin;
    ThreadID  id = cell->id;

    list->begin = cell->next;
    if (!list->begin) list->last = NULL;

    free(cell);
    list->size--;
    return id;
}

/* ------------------------------------------------------------------
 * Remove uma thread pelo ID.
 *
 * Retorna:
 *   1 -> removeu
 *   0 -> não encontrou
 * ------------------------------------------------------------------ */

int tl_remove(ThreadsList *list, ThreadID id) {
    ThreadCell *prev = NULL;
    ThreadCell *cur = list->begin;

    while (cur) {
        if (cur->id == id) {
            if (prev)
                prev->next = cur->next;
            else
                list->begin = cur->next;

            if (cur == list->last)
                list->last = prev;

            free(cur);
            list->size--;
            return 1;
        }
        prev = cur;
        cur = cur->next;
    }
    return 0;
}


/* ------------------------------------------------------------------
 * Verifica se a lista está vazia.
 *
 * Retorna:
 *   1 -> vazia
 *   0 -> possui elementos
 * ------------------------------------------------------------------ */

int tl_is_empty(ThreadsList *list) {
    return list->size == 0;
}

/* ------------------------------------------------------------------
 * Imprime lista no terminal.
 *
 * Utilizado para debug.
 * ------------------------------------------------------------------ */
void tl_print(ThreadsList *list) {
    printf("[ThreadsList size=%d]: ", list->size);
    ThreadCell *cur = list->begin;
    while (cur) {
        printf("(id=%d, prio=%d) -> ", cur->id, cur->priority);
        cur = cur->next;
    }
    printf("NULL\n");
}