all: sistema struct simpl input main
	gcc *.o -o simplex

sistema:
	gcc -c sistema_linear.h sistema_linear.c

struct:
	gcc -c structs.h

simpl:
	gcc -c simplex.h simplex.c

input:
	gcc -c input.h input.c
main:
	gcc -c main.c

clear:
	rm *.o *.gch
