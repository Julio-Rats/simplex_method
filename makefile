CC = gcc

MYFLAGS = -O2 -W

TARGET = simplex

OBJFILES = input.o sistema_linear.o simplex.o main.o

default: all

all:
	$(MAKE) $(TARGET)

# Regras para gerar o executável
$(TARGET): $(OBJFILES)
	$(CC) $(OBJFILES) -o $(TARGET)

# Regras de compilação
input.o: input.c structs.h
	$(CC) $(MYFLAGS) -c input.c

sistema_linear.o: sistema_linear.c sistema_linear.h structs.h
	$(CC) $(MYFLAGS) -c sistema_linear.c

simplex.o: simplex.c sistema_linear.c sistema_linear.h
	$(CC) $(MYFLAGS) -c simplex.c

main.o: main.c input.c simplex.c
	$(CC) $(MYFLAGS) -c main.c

clean:
	rm -f *.o
	rm $(TARGET)

clear:
	rm -f *.o