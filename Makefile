SOURCES = $(wildcard *.c)
PROGRAM = comp

CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -std=c99

$(PROGRAM):
	$(CC) $(CFLAGS) $(SOURCES) -o $(PROGRAM)

run: $(PROGRAM)
	./$(PROGRAM)