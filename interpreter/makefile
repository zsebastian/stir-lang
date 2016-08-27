CC=gcc
CFLAGS=-c -Wall
OUTPUT=-o interpreter.out

all: interpreter.out

interpreter.out: main.o parser.o
	$(CC) obj/main.o obj/parser.o $(OUTPUT)

main.o: src/main.c
	mkdir -p obj/
	$(CC) $(CFLAGS) src/main.c -o obj/main.o

parser.o: src/parser.c
	mkdir -p obj/
	$(CC) $(CFLAGS) src/parser.c -o obj/parser.o

clean: 
	rm interpreter.out
	rm -r obj
