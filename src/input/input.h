#ifndef INPUT_H
#define INPUT_H

void run_input_loop(int pipe_fd);
void run_input_file(const char *filename, int pipe_fd);

#endif