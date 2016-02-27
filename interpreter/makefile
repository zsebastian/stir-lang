CC=gcc
CFLAGS=-c -Wall
OUTPUT=-o interpreter.out


all: interpreter.out
	mkdir -p obj/

interpreter.out: main.o token.o
	$(CC) obj/main.o obj/token.o $(OUTPUT)

token.o: src/token.cpp
	$(CC) $(CFLAGS) src/token.cpp -o obj/token.o

main.o: src/main.cpp
	$(CC) $(CFLAGS) src/main.cpp -o obj/main.o

clean: 
	rm *o interpreter.out
