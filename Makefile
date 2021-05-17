COMPILER=g++
FLAGS=-O3 -pedantic

all:
	$(COMPILER) $(FLAGS) main.cpp -o main

test:
	$(COMPILER) $(FLAGS) tests.cpp -o tests