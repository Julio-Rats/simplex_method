# Implementation of the Simplex Method

This repository provides a ``C`` implementation of the ``Simplex Method``, a widely-used algorithm for solving linear programming problems. The Simplex Method optimizes an objective function, either ``maximizing`` or ``minimizing`` it, while adhering to a set of linear constraints on the decision variables.

To use the program, provide a text file containing the objective function along with the constraints, and pass it as a parameter to the program.
The file must follow this format:
~~~sh
max 5x1 + 3x2   # Objective function

2x1 + x2 <= 10
x1 + 3x2 <= 12
# Use '#' for comments. Anything following this symbol,
#  up to the end of the line, will be treated as a comment.
~~~
The objective function can be declared using **max**, **maximize**, **min**, or **minimize**. Note that it is not case-sensitive.
#### All variables are subject to `>= 0`, so adding non-negativity constraints in the constraints file is redundant.


## Compile and run the program:

The program can be compiled using the **Makefile** with the `make` command, or manually by navigating to the folder via the terminal and using the compiler.
Example of manual compilation:
~~~sh
gcc -c *.c
gcc *.o -o simplex.exe
~~~

These commands will generate the ``simplex.exe`` executable, which requires the file containing the objective function and its constraints to be passed as a parameter.
~~~sh
simplex.exe restrictions.txt
~~~

The program will run, displaying the execution steps in matrix notation and providing the solution at the end, if one exists. Note that the method does NOT handle cycling cases automatically. To address this, use the ``-b`` parameter to activate Bland's pivoting rule, which ensures that only one solution is displayed in the case of multiple solutions

## Parameters:
```txt
Use:
    simplex [options] <file.txt>

Options:

-b: Enable Bland's pivot rule
-d: Disable verbose mode
-e <value>: Set manual value to Epsilon (Default: 1e-9)
-i: Activate iterative mode (incompatible with disable verbose, -d)
-m <value>: Set manual value to BIGM (Default: 1e9)
```
