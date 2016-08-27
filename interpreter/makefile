CC=gcc
CFLAGS=-c -Wall -Og
OUTPUT=-o interpreter.out

all: interpreter.out

interpreter.out: main.o parser.o cpu.o parg.o code_generator.o
	$(CC) obj/main.o obj/parser.o obj/cpu.o obj/parg.o obj/code_generator.o $(OUTPUT)

code_generator.o: src/code_generator.c
	mkdir -p obj/
	$(CC) $(CFLAGS) src/code_generator.c -o obj/code_generator.o

main.o: src/main.c
	mkdir -p obj/
	$(CC) $(CFLAGS) src/main.c -o obj/main.o

parser.o: src/parser.c
	mkdir -p obj/
	$(CC) $(CFLAGS) src/parser.c -o obj/parser.o

cpu.o: src/cpu.c
	mkdir -p obj/
	$(CC) $(CFLAGS) src/cpu.c -o obj/cpu.o

parg.o: modules/parg/parg.c
	mkdir -p obj/
	$(CC) $(CFLAGS) modules/parg/parg.c -o obj/parg.o

clean: 
	rm interpreter.out
	rm -r obj
