CC = clang
CFLAGS = -Wall -Werror -Wextra -pedantic

httpserver: httpserver.o bind.o
	$(CC) -o httpserver httpserver.o bind.o

httpserver.o:
	$(CC) $(CFLAGS) -c httpserver.c

bind.o:
	$(CC) $(CFLAGS) -c bind.c

clean:
	rm -f httpserver httpserver.o bind.o

format:
	clang-format -i -style=file split.c

valgrind: httpserver
	valgrind --leak-check=full ./httpserver 1024
