#ifndef PARSER_H
#define PARSER_H

#define MAX_LINE 256
#define MAX_FILENAME 128

typedef enum {
    INST_N,
    INST_D,
    INST_V,
    INST_A,
    INST_S,
    INST_B,
    INST_T,
    INST_F,
    INST_R
} InstructionType;

typedef struct {
    InstructionType type;

    int arg1;
    int arg2;

    char filename[MAX_FILENAME];
} Instruction;

typedef struct {
    Instruction *instructions;
    int size;
} Program;

Program *load_program(const char *filename);
void free_program(Program *program);
void print_program(Program *program);

#endif