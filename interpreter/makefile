CC=gcc
CFLAGS=-c -Wall
OUTPUT=-o interpreter


all: interpreter

interpreter: main.o token.o
	mkdir -p obj/
	$(CC) obj/main.o obj/token.o $(OUTPUT)

token.o: src/token.cpp
	$(CC) $(CFLAGS) src/token.cpp -o obj/token.o

main.o: src/main.cpp
	$(CC) $(CFLAGS) src/main.cpp -o obj/main.o

clean: 
	rm *o interpreter
