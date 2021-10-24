CC=g++
FLAGS=-lsfml-graphics -lsfml-window -lsfml-system

all: main.o snake clean

snake: main.o
	$(CC) -o snake main.o $(FLAGS)

main.o: main.cpp
	$(CC) -c -std=c++20 -o main.o main.cpp $(FLAGS)

clean:
	rm -f main.o
