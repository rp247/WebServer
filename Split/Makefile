CC = clang
CFLAGS = -Wall -Werror -Wextra -pedantic

split: split.o
	$(CC) -o split split.o

split.o:
	$(CC) $(CFLAGS) -c split.c

clean:
	rm -f split split.o

format:
	clang-format -i -style=file split.c

valgrind: split
	valgrind --leak-check=full ./split - a