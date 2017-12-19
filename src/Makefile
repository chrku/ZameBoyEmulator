
CC = gcc
CFLAGS = -g -Wall -Wextra -lSDL2

all: main.c cpu.o rom.o decode.o execute.o util.o opcode.h
	$(CC) $(CFLAGS) -o emu main.c cpu.o rom.o decode.o execute.o util.o opcode.h

cpu.o: cpu.c cpu.h
	$(CC) $(CFLAGS) -c cpu.c

rom.o: rom.c rom.h
	$(CC) $(CFLAGS) -c rom.c

decode.o: decode.c decode.h
	$(CC) $(CFLAGS) -c decode.c

execute.o: execute.c execute.h
	$(CC) $(CFLAGS) -c execute.c

util.o: util.c util.h
	$(CC) $(CFLAGS) -c util.c

clean:
	rm *.o
	rm emu
