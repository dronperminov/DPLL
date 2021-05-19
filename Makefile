COMPILER=g++
FLAGS=-O3 -pedantic

all: dpll test

dpll:
	$(COMPILER) $(FLAGS) main.cpp -o dpll

test:
	$(COMPILER) $(FLAGS) tests.cpp -o tests