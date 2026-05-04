CC = gcc
APP_SRC = src/cpu.c src/cycle.c include/cpu.h
CFLAGS = -fsanitize=address,undefined -fno-sanitize=leak -fno-omit-frame-pointer -g -std=c99 -Wall -Wextra -Wconversion -pedantic -Iinclude

test: $(APP_SRC)
	$(CC) $(CFLAGS) $(APP_SRC) -o test
clean: 
	rm -r test

