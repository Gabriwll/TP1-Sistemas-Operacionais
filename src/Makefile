CC = gcc
CFLAGS = -Wall -Wextra

SRC = \
    src/main.c \
    src/input/parser.c \
    src/input/input.c

TARGET = simulator

all:
	$(CC) $(SRC) $(CFLAGS) -o $(TARGET)

run:
	./$(TARGET)

clean:
	rm -f $(TARGET)