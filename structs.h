#ifndef _STRUCT_
#define _STRUCT_

#define true 1
#define false 0

#define BIGM 1000000

typedef char  bool;
typedef char* string;

typedef enum
{
    ORIGINAL = 0,
    FOLGA,
    ARTIFICIAL
} vartype_e;

typedef struct
{
    double cost;    //  Custo na função objetivo (C);
    string name;    // Nome da variavel, ex: "x1", "x2".
    vartype_e type; // Tipo da variavel (folga ou artificial).
    double** aj;    // Coluna com suas restrições.
} variavel_t;

extern size_t number_base;    // Dimensão da matriz basica (B).
extern size_t number_Nbase;   // Dimensão da matriz não basica (N).
extern double** vetor_b;      // Vetor B
extern variavel_t* var_base;  // Variveis basicas.
extern variavel_t* var_Nbase; // Variveis não basicas.

#endif
