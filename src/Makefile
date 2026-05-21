CC = gcc
CFLAGS = -Wall -Wextra

SRC = \
    main.c \
    input/parser.c \
    input/input.c

TARGET = simulator

all:
	$(CC) $(SRC) $(CFLAGS) -o $(TARGET)

run:
	./$(TARGET)

clean:
	rm -f $(TARGET)