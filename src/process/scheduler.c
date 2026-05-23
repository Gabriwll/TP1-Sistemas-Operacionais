#include <stdio.h>
#include <stdlib.h>

#include "scheduler.h"

/* FUNÇÕES COMPLEMENTARES */

PCB* get_next_ready_process(Scheduler *scheduler){
    if(scheduler->type == SCHED_MLFQ){
        // é feita a busca na fila de maior prioridade para a menor, retornando o primeiro processo encontrado
        for(int i=0; i<4; i++){
            if(!is_queue_empty(&scheduler->readyQueues[i])){
                return dequeue(&scheduler->readyQueues[i]);
            }
        }
    } else{
        return dequeue(&scheduler->rrQueue);
    }

    return NULL;
}

// implementação da lógica base do MLFQ
void mlfq_tick(Scheduler *scheduler, CPU *cpu){
    // aqui é feita a lógica principal do escalonamento, que representa o avanço do tempo. 
    // A cada tick, o escalonador verifica se o processo atual excedeu seu quantum e, se sim, 
    // move-o para a fila de menor prioridade e seleciona o próximo processo para execução.
    

    // existe processo rodando na cpu
    PCB *current = cpu->current_process;

    if(current->quantum_used < get_quantum_by_priority(current->priority)){
        // verifica se o processo ainda não excedeu seu quantum
        // se não tiver excedido não acontece nada
        return;
    }

    // caso chegou aqui significa que o quantum foi excedido
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

// implementação da lógica base do RR
void rr_tick(Scheduler *scheduler, CPU *cpu){
    // existe processo rodando na cpu
    PCB *current = cpu->current_process;

    if(current->quantum_used < scheduler->rrQuantum){
        // verifica se o processo ainda não excedeu seu quantum
        // se não tiver excedido não acontece nada
        return;
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

/* MLFQ and RR implementations */

void initialize_scheduler(Scheduler *scheduler, SchedulerType type, int rr_quantum){
    if(scheduler == NULL)
        return;
    
    scheduler->type = type;

    if(scheduler->type == SCHED_MLFQ){
        initialize_queue(&scheduler->readyQueues[0], "PRIORIDADE 0");
        initialize_queue(&scheduler->readyQueues[1], "PRIORIDADE 1");
        initialize_queue(&scheduler->readyQueues[2], "PRIORIDADE 2");
        initialize_queue(&scheduler->readyQueues[3], "PRIORIDADE 3");

        // evitando erro ou lixo de memória
        initialize_queue(&scheduler->rrQueue, "RR inativo");
        scheduler->rrQuantum = 0;
    } else{
        initialize_queue(&scheduler->rrQueue, "ROUND ROBIN");
        // seleciona valor do quantum, se for válido aceita, senão usa o padrão (evita erros)
        scheduler->rrQuantum = (rr_quantum > 0) ? rr_quantum : RR_DEFAULT_QUANTUM;

        // evitando erro ou lixo de memória
        for (int i = 0; i < MLFQ_LEVELS; i++)
            initialize_queue(&scheduler->readyQueues[i], NULL);
    }
    
}

void schedule_process(Scheduler *scheduler, PCB *process){
    // responsável por colocar um processo na fila correta de acordo com sua prioridade
    if(scheduler == NULL || process == NULL)
        return;
    
    if(scheduler->type == SCHED_MLFQ){
        // garante limites na prioridade dos processos
        if(process->priority < 0)
            process->priority = 0;
        if(process->priority > 3)
            process->priority = 3;

        enqueue(&scheduler->readyQueues[process->priority], process);
    } else{
        // RR
        enqueue(&scheduler->rrQueue, process);
    }
    
}

void scheduler_tick(Scheduler *scheduler, CPU *cpu){
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

    if(scheduler->type == SCHED_MLFQ)
        mlfq_tick(scheduler, cpu);
    else
        rr_tick(scheduler, cpu);

}

void destroy_scheduler(Scheduler *scheduler){
    if(scheduler == NULL)
        return;
    
    if(scheduler->type == SCHED_MLFQ){
        for(int i=0; i<4; i++){
            clear_queue(&scheduler->readyQueues[i]);
        }

        // limpa também fila inativa
        clear_queue(&scheduler->rrQueue);
    } else{
        clear_queue(&scheduler->rrQueue);

        // limpa também filas inativas
        for (int i = 0; i < MLFQ_LEVELS; i++)
            clear_queue(&scheduler->readyQueues[i]);
    }    
}