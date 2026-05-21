#include <stdio.h>

#include "input.h"

static int is_valid_command(char c) {

    return c == 'U' || c == 'I' || c == 'M';
}

void run_input_loop(int pipe_fd) {

    char command;

    while (1) {

        printf("Digite comando (U/I/M): ");

        scanf(" %c", &command);

        if (!is_valid_command(command)) {
            printf("Comando inválido\n");
            continue;
        }

        write(pipe_fd, &command, sizeof(char));

        if (command == 'M')
            break;
    }
}

void run_input_file(const char *filename, int pipe_fd) {

    FILE *file = fopen(filename, "r");

    if (!file) {
        perror("Erro ao abrir arquivo de entrada");
        return;
    }

    char line[32];

    while (fgets(line, sizeof(line), file)) {

        char command;

        sscanf(line, " %c", &command);

        if (!is_valid_command(command)) {
            printf("Comando inválido no arquivo: %c\n", command);
            continue;
        }

        write(pipe_fd, &command, sizeof(char));

        if (command == 'M')
            break;
    }

    fclose(file);
}