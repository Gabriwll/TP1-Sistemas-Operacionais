#ifndef QUEUE_H
#define QUEUE_H

#include "process.h"

typedef struct QueueNode {
    PCB *process;
    struct QueueNode *next;
} QueueNode;

typedef struct {
    QueueNode *front;
    QueueNode *rear;
    int size;
    char *name;             // nome da fila para debug
} Queue;

void initialize_queue(Queue *queue, const char *name);
int is_queue_empty(Queue *queue);
void enqueue(Queue *queue, PCB *process);
PCB *dequeue(Queue *queue);
PCB *peek_queue(Queue *queue);
void print_queue(Queue *queue, int current_time, int modo_detalhado);
void clear_queue(Queue *queue);

#endif