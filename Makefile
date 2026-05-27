CC = gcc
APP_SRC = src/cpu.c src/cycle.c src/exec.c include/cpu.h include/qencode.h include/exec.h
CFLAGS = -fsanitize=address,undefined -fno-sanitize=leak -fno-omit-frame-pointer -g -std=c99 -Wall -Wextra  -pedantic -Iinclude

final: $(APP_SRC)
	$(CC) $(CFLAGS) $(APP_SRC) -o bisc-vm
clean: 
	rm -r bisc-vm

