CC=gcc
CFLAGS=-c -Wall -Og
OUTPUT=-o interpreter.out

all: interpreter.out

interpreter.out: main.o parser.o cpu.o
	$(CC) obj/main.o obj/parser.o obj/cpu.o $(OUTPUT)

main.o: src/main.c
	mkdir -p obj/
	$(CC) $(CFLAGS) src/main.c -o obj/main.o

parser.o: src/parser.c
	mkdir -p obj/
	$(CC) $(CFLAGS) src/parser.c -o obj/parser.o

cpu.o: src/cpu.c
	mkdir -p obj/
	$(CC) $(CFLAGS) src/cpu.c -o obj/cpu.o

clean: 
	rm interpreter.out
	rm -r obj
