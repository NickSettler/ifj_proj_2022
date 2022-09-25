SOURCES = $(wildcard *.c)
PROGRAM = comp

CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -std=c99

$(PROGRAM):
	$(CC) $(CFLAGS) $(SOURCES) -o $(PROGRAM)

clean:
	rm -f $(PROGRAM)

run: clean $(PROGRAM)
	./$(PROGRAM)