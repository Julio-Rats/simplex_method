#ifndef _STRUCT_
#define _STRUCT_

#include <stdlib.h>

#define true 1
#define false 0

typedef char bool;
typedef char *string;
typedef double *vector_t;
typedef double **matrix_t;

typedef enum
{
    ORIGINAL = 0,
    FOLGA,
    ARTIFICIAL
} vartype_e;

typedef struct
{
    double cost;    // Cost in objective function (C)
    string name;    // Variable name, ex: "x1", "x2".
    size_t index;   // Unique numeric ID for each variable
    vartype_e type; // Variable type (Original, Slack or Artificial).
    double **aj;    // Column with its restrictions.
} variavel_t;

// input.c
extern size_t number_base;    // Dimension of the basic matrix (B).
extern size_t number_Nbase;   // Non-basic matrix dimension (N).
extern double **vetor_b;      // Vector b, from the system Ax=b.
extern variavel_t *var_base;  // Basic variables.
extern variavel_t *var_Nbase; // Non-basic variables.

#endif
