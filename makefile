CC = gcc

MYFLAGS = -O2 -Wall

TARGET = simplex

OBJFILES = input.o sistema_linear.o simplex.o main.o

default: all

all:
	$(MAKE) $(TARGET)

# Rules for generating the executable
$(TARGET): $(OBJFILES)
	$(CC) $(OBJFILES) -o $(TARGET)

# Compilation rules
input.o: input.c structs.h
	$(CC) -c input.c $(MYFLAGS) 

sistema_linear.o: sistema_linear.c sistema_linear.h structs.h
	$(CC) -c sistema_linear.c $(MYFLAGS)

simplex.o: simplex.c sistema_linear.c sistema_linear.h
	$(CC) -c simplex.c $(MYFLAGS)

main.o: main.c input.c simplex.c
	$(CC) -c main.c $(MYFLAGS) 

clean:
	rm -f *.o
	rm $(TARGET)

clear:
	rm -f *.o