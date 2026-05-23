#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

void initialize_queue(Queue *queue, const char *name) {
    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
    
    if (name) {
        queue->name = malloc(strlen(name) + 1);
        if (queue->name)
            strcpy(queue->name, name);
    } else {
        queue->name = NULL;
    }
}

int is_queue_empty(Queue *queue) {
    return queue->size == 0;
}

void enqueue(Queue *queue, PCB *process) {
    QueueNode *node = malloc(sizeof(QueueNode));
    
    node->process = process;
    node->next = NULL;
    
    if (queue->rear)
        queue->rear->next = node;
    
    queue->rear = node;
    
    if (!queue->front)
        queue->front = node;
    
    queue->size++;
}

PCB *dequeue(Queue *queue) {
    if (is_queue_empty(queue))
        return NULL;
    
    QueueNode *node = queue->front;
    PCB *process = node->process;
    
    queue->front = node->next;
    
    if (!queue->front)
        queue->rear = NULL;
    
    free(node);
    queue->size--;
    
    return process;
}

PCB *peek_queue(Queue *queue) {
    if (is_queue_empty(queue))
        return NULL;
    
    return queue->front->process;
}

void print_queue(Queue *queue, int current_time, int modo_detalhado) {
    if (queue->name) {
        printf("[%s]\n", queue->name);
    } else {
        printf("[Fila]\n");
    }
    
    if (is_queue_empty(queue)) {
        printf("  (vazia)\n");
        return;
    }
    
    QueueNode *current = queue->front;
    
    while (current) {
        PCB *p = current->process;
        if (p->state == BLOCKED) {
            int restante = p->blocked_until - current_time;
            if (restante < 0) restante = 0;
            printf("  - PID: %d | Tempo restante: %d", p->pid, restante);
        } else {
            printf("  - PID: %d | Prioridade: %d", p->pid, p->priority);
        }
        
        if (modo_detalhado) {
            printf(" | PC: %d", p->pc);
            if (p->memory_size > 0) {
                printf(" | Vars: [");
                for (int i=0; i<p->memory_size; i++) printf(" %d", p->memory[i]);
                printf(" ]");
            }
        }
        printf("\n");
        
        current = current->next;
    }
}

void clear_queue(Queue *queue) {
    while (!is_queue_empty(queue)) {
        PCB *p = dequeue(queue);
        (void)p;  // não libera o processo, só remove da fila
    }
    
    if (queue->name) {
        free(queue->name);
        queue->name = NULL;
    }
}