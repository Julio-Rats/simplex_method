# Implementação do Método Simplex

This repository contains a C implementation of the Simplex Method for optimizing linear functions. The Simplex Method is an algorithm used to solve operational research (OP) problems in which it can be used to maximize or minimize an objective function subject to a set of linear restrictions on its variables.

To use the program, simply provide a text file containing the objective function with the restrictions and pass it via parameter.
The file must contain the following format:
~~~txt
max 5x1 + 3x2

2x1 + x2 <= 10
x1 + 3x2 <= 12
~~~
The declaration of the objective function can be used **max**, **maximize**, **min** or **minimize**.
### All variables are subject to '>= 0', so placing non-negativity constraints in the constraints file is redundancy.


## Compile and run the program:

Compilation can be done through the **Makefile** using the **make** program, or manually by accessing
the folder with terminal and using the compiler.
Example of a manual compilation:
~~~sh
gcc -c *.o
gcc *.o -o simplex.exe
~~~

These commands will generate the simplex.exe executable, in which the file must be passed as a parameter
with the objective function and its restrictions.
~~~sh
simplex.exe restrictions.txt
~~~

The program will execute showing the executable steps in matrix notation, giving the solution at the end.
if it exists, it is worth highlighting that the method still DOES NOT treat degenerate cases, not showing all
solutions in case of multiple solutions.
