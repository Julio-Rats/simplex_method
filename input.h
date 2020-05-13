#ifndef _INPUT_
#define _INPUT_

#include "simplex.h"
#include <string.h>

typedef enum
{
    INICIO = 0, SVAR, SNUM, SMENORIGUAL, SMAIORIGUAL
}states;

typedef enum
{
    NUM = 0, VAR, MULT, SOMA, SUB, NL, EF, MENOR, MENORIGUAL,
    MAIOR, MAIORIGUAL, IGUAL
}type_token;

typedef struct
{
    char        value[64];
    type_token  type;
}token_t;

FILE*                arq;
token_t              token;
extern variavel_t*   list_var;
extern int           num_var;


string* split(string line, char token, int* length);
string  trim(string line, char token);
token_t get_token();
void    consome_tokens(string path_file);

void    objetivo();
void    funcao();
void    proxima();
void    escalar(int sinal_var);
void    variavel(double escalar);
void    restricao();
#endif
