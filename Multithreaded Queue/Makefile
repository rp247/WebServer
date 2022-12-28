CC = clang
CFLAGS = -Wall -Werror -Wextra -pedantic

queue.o:
	$(CC) $(CFLAGS) -c queue.c -o queue.o

clean:
	rm -f queue.o

format:
	clang-format -i -style=file queue.c