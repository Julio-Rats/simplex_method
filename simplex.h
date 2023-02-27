#ifndef _SIMPLEX_
#define _SIMPLEX_

#include <stdio.h>
#include <stdlib.h>
#include "sistema_linear.h"

typedef struct
{
    double valor;
    size_t variavel;
} custo_t;

void simplex();

#endif
