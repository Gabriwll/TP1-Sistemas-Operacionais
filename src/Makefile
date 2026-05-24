CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = simulator

SRC = \
    main.c \
    input/input.c \
    input/parser.c \
    process/process.c \
    process/executor.c \
    process/cpu.c \
    process/queue.c \
    process/scheduler.c \
    output/interface.c \
    output/printer.c

all:
	$(CC) $(SRC) $(CFLAGS) -o $(TARGET)

run:
	./$(TARGET)

clean:
	rm -f $(TARGET)

clean-all:
	rm -f $(TARGET)
	find . -name "*.o" -delete
