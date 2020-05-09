all: sistema struct simplex main
	gcc *.o -o simplex

sistema:
	gcc -c sistema_linear.h sistema_linear.c

struct:
	gcc -c structs.h

simplex:
	gcc -c simplex.h simplex.c

main:
	gcc -c main.c

clear:
	rm *.o
