#include <stdio.h>
#include <stdlib.h>

#include "scheduler.h"

// MLFQ scheduler implementation

void initialize_scheduler(Scheduler *scheduler){
    if(scheduler == NULL)
        return;
    
    initialize_queue(&scheduler->readyQueues[0], "PRIORIDADE 0");
    initialize_queue(&scheduler->readyQueues[1], "PRIORIDADE 1");
    initialize_queue(&scheduler->readyQueues[2], "PRIORIDADE 2");
    initialize_queue(&scheduler->readyQueues[3], "PRIORIDADE 3");
}

void schedule_process(Scheduler *scheduler, PCB *process){
    // responsável por colocar um processo na fila correta de acordo com sua prioridade
    if(scheduler == NULL || process == NULL)
        return;
    
    // garante limites na prioridade dos processos
    if(process->priority < 0){
        process->priority = 0;
    }
    if(process->priority > 3){
        process->priority = 3;
    }

    enqueue(&scheduler->readyQueues[process->priority], process);
}

/* FUNÇÕES COMPLEMENTARES */

PCB* get_next_ready_process(Scheduler *scheduler){
    // é feita a busca na fila de maior prioridade para a menor, retornando o primeiro processo encontrado
    for(int i=0; i<4; i++){
        if(!is_queue_empty(&scheduler->readyQueues[i])){
            return dequeue(&scheduler->readyQueues[i]);
        }
    }
    return NULL;
}

void scheduler_tick(Scheduler *scheduler, CPU *cpu){
    // aqui é feita a lógica principal do escalonamento, que representa o avanço do tempo. 
    // A cada tick, o escalonador verifica se o processo atual excedeu seu quantum e, se sim, 
    // move-o para a fila de menor prioridade e seleciona o próximo processo para execução.
    if(scheduler == NULL || cpu == NULL)
        return;
    
    // Com a cpu ociosa fazemos a busca pelo próximo processo a ser executado
    if(cpu->current_process == NULL){
        PCB *next = get_next_ready_process(scheduler);
        if(next != NULL){
            // troca de contexto para o próximo processo
            execute_context_switch(cpu, next, RUNNING);
        }
        return;
    }

    // existe processo rodando na cpu
    PCB *current = cpu->current_process;
    //current->quantum_used++; //deve ser retirada na junção com a execução das instruções, mas para fins de teste é feita aqui

    if(current->quantum_used >= get_quantum_by_priority(current->priority)){ // verifica se o processo excedeu seu quantum
        // Base da MLFQ: se excedeu, é movido para a fila de menor prioridade (a menos que já esteja na mais baixa)
        // e o próximo processo é selecionado para execução
        if(current->priority < 3){
            current->priority++;
        }

        PCB *next = get_next_ready_process(scheduler);

        if(next != NULL){
            execute_context_switch(cpu, next, READY);
            schedule_process(scheduler, current);
        } else{
            // Se não tem outro processo para rodar, o processo atual continua, mas seu quantum é resetado
            current->quantum_used = 0;
        }
    }
}

/*void block_current_process(Scheduler *scheduler, CPU *cpu){
    if(scheduler == NULL || cpu == NULL || cpu->current_process == NULL)
        return;
    
    PCB *current = cpu->current_process;

    // quando o processo bloqueia antes do fim do quantum ele nã está monopolizando CPU,
    // então ele ganha prioridade para ter resposta mais rápida quando for desbloqueado, ou seja, ele é colocado na fila de maior prioridade
    if(current->priority > 0){
        current->priority--;
    }

    current->quantum_used = 0;

    PCB *next = get_next_ready_process(scheduler);
    if(next != NULL){
        execute_context_switch(cpu, next, BLOCKED);
    } else{
        // CPU fica ociosa, mas o processo atual precisa sair
        cpu->current_process->state = BLOCKED;
        cpu->current_process = NULL;
    }
}*/
// verificar se essa vai ser mesmo necessária na junçãi com a execução de instruções

void destroy_scheduler(Scheduler *scheduler){
    if(scheduler == NULL)
        return;
    
    for(int i=0; i<4; i++){
        clear_queue(&scheduler->readyQueues[i]);
    }
}

