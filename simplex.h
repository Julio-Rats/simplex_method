#ifndef _SIMPLEX_
#define _SIMPLEX_

#include "sistema_linear.h"

typedef struct
{
    double valor;
    int    variavel;
}custo_t;

static int  sort = 0;
double**    matriz_Base;   //  Matriz com as aj colunas de A, das j variaveis basicas.

void      simplex();
void      calc_menor_custo(double valor, int var);
int       var_menor_custo();

#endif
