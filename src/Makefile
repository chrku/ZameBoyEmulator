
CC = gcc
CFLAGS = -g -Wall -Wextra -std=c11
LIBS = $(shell sdl2-config --cflags --libs)
 
all: main.c cpu.o rom.o decode.o execute.o util.o opcode.h graphics.o
	$(CC) main.c cpu.o rom.o decode.o execute.o util.o opcode.h graphics.o $(CFLAGS) $(LIBS) -o emu  

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

graphics.o: graphics.c graphics.h
	$(CC) $(CFLAGS) -c graphics.c  

clean:
	rm *.o
	rm emu
