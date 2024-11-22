CC=gcc
CFLAGS=-Wall -Wextra

main: main.c c_lexer.h
	$(CC) $(CFLAGS) -o main main.c