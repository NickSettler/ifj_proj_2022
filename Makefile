SOURCES = $(wildcard src/*.c)
PROGRAM = comp

CC = gcc
CFLAGS = -pedantic -std=c99
CFLAGS_TEST = -std=c++11 -I/opt/homebrew/include -L/opt/homebrew/lib -lgtest -lgtest_main -lpthread

TEST_SOURCES = $(wildcard tests/*.cpp)
TEST_PROGRAM = test

$(PROGRAM):
	@$(CC) $(CFLAGS) $(SOURCES) -o $(PROGRAM)

clean:
	@rm -f $(PROGRAM)

run: clean $(PROGRAM)
	@./$(PROGRAM)

test_clean:
	@rm -f $(TEST_PROGRAM)

test: test_clean
	@g++ $(CFLAGS_TEST) $(TEST_SOURCES) -o $(TEST_PROGRAM)
	@./$(TEST_PROGRAM)