#ifndef PRINTER_H
#define PRINTER_H

#include <sys/types.h>
#include "../process/cpu.h"
#include "../process/scheduler.h"
#include "../process/queue.h"

// Função que gerencia o fork e garante que apenas um processo de impressão rode
void spawn_print_process(pid_t *print_pid, int current_time, CPU *cpu, Scheduler *scheduler, Queue *blocked_queue, int modo_detalhado);

#endif