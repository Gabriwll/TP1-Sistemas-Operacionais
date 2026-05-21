#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

static Instruction parse_line(const char *line) {
    Instruction inst;
    memset(&inst, 0, sizeof(Instruction));
    
    char op;
    
    if (sscanf(line, " %c", &op) != 1) {
        fprintf(stderr, "Erro ao ler instrução: %s\n", line);
        exit(EXIT_FAILURE);
    }
    
    switch (op) {
        case 'N':
            inst.type = INST_N;
            if (sscanf(line, "N %d", &inst.arg1) != 1) {
                fprintf(stderr, "Erro na instrução N\n");
                exit(EXIT_FAILURE);
            }
            break;
            
        case 'D':
            inst.type = INST_D;
            if (sscanf(line, "D %d", &inst.arg1) != 1) {
                fprintf(stderr, "Erro na instrução D\n");
                exit(EXIT_FAILURE);
            }
            break;
            
        case 'V':
            inst.type = INST_V;
            if (sscanf(line, "V %d %d", &inst.arg1, &inst.arg2) != 2) {
                fprintf(stderr, "Erro na instrução V\n");
                exit(EXIT_FAILURE);
            }
            break;
            
        case 'A':
            inst.type = INST_A;
            if (sscanf(line, "A %d %d", &inst.arg1, &inst.arg2) != 2) {
                fprintf(stderr, "Erro na instrução A\n");
                exit(EXIT_FAILURE);
            }
            break;
            
        case 'S':
            inst.type = INST_S;
            if (sscanf(line, "S %d %d", &inst.arg1, &inst.arg2) != 2) {
                fprintf(stderr, "Erro na instrução S\n");
                exit(EXIT_FAILURE);
            }
            break;
            
        case 'B':
            inst.type = INST_B;
            if (sscanf(line, "B %d", &inst.arg1) != 1) {
                fprintf(stderr, "Erro na instrução B\n");
                exit(EXIT_FAILURE);
            }
            break;
            
        case 'T':
            inst.type = INST_T;
            break;
            
        case 'F':
            inst.type = INST_F;
            if (sscanf(line, "F %d", &inst.arg1) != 1) {
                fprintf(stderr, "Erro na instrução F\n");
                exit(EXIT_FAILURE);
            }
            break;
            
        case 'R':
            inst.type = INST_R;
            if (sscanf(line, "R %127s", inst.filename) != 1) {
                fprintf(stderr, "Erro na instrução R\n");
                exit(EXIT_FAILURE);
            }
            break;
            
        default:
            fprintf(stderr, "Instrução inválida: %c\n", op);
            exit(EXIT_FAILURE);
    }
    
    return inst;
}

Program *load_program(const char *filename) {
    FILE *file = fopen(filename, "r");
    
    if (!file) {
        perror("Erro ao abrir programa");
        return NULL;
    }
    
    Program *program = malloc(sizeof(Program));
    
    if (!program) {
        perror("malloc");
        fclose(file);
        return NULL;
    }
    
    program->instructions = NULL;
    program->size = 0;
    program->refcount = 1;        // inicializa contador de referências
    
    char line[MAX_LINE];
    
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';
        
        if (strlen(line) == 0)
            continue;
        
        Instruction *new_buffer = realloc(program->instructions,
                                          sizeof(Instruction) * (program->size + 1));
        
        if (!new_buffer) {
            perror("realloc");
            free(program->instructions);
            free(program);
            fclose(file);
            return NULL;
        }
        
        program->instructions = new_buffer;
        program->instructions[program->size] = parse_line(line);
        program->size++;
    }
    
    fclose(file);
    return program;
}

void free_program(Program *program) {
    if (!program)
        return;
    
    free(program->instructions);
    free(program);
}

void print_program(Program *program) {
    if (!program)
        return;
    
    printf("Programa possui %d instrucoes (refcount=%d)\n", program->size, program->refcount);
    
    for (int i = 0; i < program->size; i++) {
        Instruction *inst = &program->instructions[i];
        printf("[%d] ", i);
        
        switch (inst->type) {
            case INST_N: printf("N %d", inst->arg1); break;
            case INST_D: printf("D %d", inst->arg1); break;
            case INST_V: printf("V %d %d", inst->arg1, inst->arg2); break;
            case INST_A: printf("A %d %d", inst->arg1, inst->arg2); break;
            case INST_S: printf("S %d %d", inst->arg1, inst->arg2); break;
            case INST_B: printf("B %d", inst->arg1); break;
            case INST_T: printf("T"); break;
            case INST_F: printf("F %d", inst->arg1); break;
            case INST_R: printf("R %s", inst->filename); break;
        }
        
        printf("\n");
    }
}

void increment_program_refcount(Program *program) {
    if (program) {
        program->refcount++;
    }
}