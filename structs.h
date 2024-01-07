#ifndef _STRUCT_
#define _STRUCT_

#include <stdlib.h>

#define true 1
#define false 0

#define BIGM (1e6)

typedef char bool;
typedef char *string;
typedef double *vetor_t;
typedef double **matriz_t;

typedef enum
{
    ORIGINAL = 0,
    FOLGA,
    ARTIFICIAL
} vartype_e;

typedef struct
{
    double cost;    // Custo na função objetivo (C);
    string name;    // Nome da variável, ex: "x1", "x2".
    vartype_e type; // Tipo da variável (Folga ou Artificial).
    double **aj;    // Coluna com suas restrições.
} variavel_t;

extern size_t number_base;    // Dimensão da matriz básica (B).
extern size_t number_Nbase;   // Dimensão da matriz não básica (N).
extern double **vetor_b;      // Vetor B
extern variavel_t *var_base;  // variáveis básicas.
extern variavel_t *var_Nbase; // variáveis não básicas.

#endif
