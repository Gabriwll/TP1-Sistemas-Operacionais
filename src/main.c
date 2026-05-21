#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "input/input.h"

int main() {

    int fd[2];

    if (pipe(fd) == -1) {
        perror("pipe");
        exit(1);
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {

        close(fd[1]);

        char command;

        while (read(fd[0], &command, sizeof(char)) > 0) {

            printf("[GERENCIADOR] recebeu comando: %c\n", command);

            if (command == 'M')
                break;
        }

        close(fd[0]);

        exit(0);
    }

    else {

        close(fd[0]);

        int option;

        printf("1 - Terminal\n");
        printf("2 - Arquivo\n");
        printf("Escolha: ");

        scanf("%d", &option);

        if (option == 1) {
            run_input_loop(fd[1]);
        }

        else if (option == 2) {

            char filename[128];

            printf("Arquivo de comandos: ");
            scanf("%s", filename);

            run_input_file(filename, fd[1]);
        }

        else {
            printf("Opção inválida\n");
        }

        close(fd[1]);

        wait(NULL);
    }

    return 0;
}