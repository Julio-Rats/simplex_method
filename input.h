#ifndef _INPUT_
#define _INPUT_

#include "simplex.h"
#include <string.h>
#include <float.h>

#define  BIGM   10000

typedef enum
{
    INICIO = 0, SVAR, SNUM, DNUM, DNUM_O, SMENORIGUAL, SMAIORIGUAL
}states;

typedef enum
{
    NUM = 0, VAR, MULT, SOMA, SUB, NL, EF, MENORIGUAL,
    MAIORIGUAL, IGUAL, ERROR
}type_token;

typedef struct
{
    char        value[64];
    type_token  type;
}token_t;

FILE*         arq;
token_t       token;
variavel_t*   list_var;
int           num_var;


string*    split(string line, char token, int* length);
string     trim(string line, char token);
string     random_var(string prefixo, int index);
token_t    get_token();
void       input_file(string path_file);
void       uniao_var();

void       objetivo();
void       funcao();
void       proxima();
void       escalar(int sinal_var);
void       variavel(double escalar);
void       restricao();
void       line();
void       proxima_restrict();
void       escalar_restrict(int sinal_var);
void       variavel_restrict(double escalar);
void       menor_igual();
void       maior_igual();
void       igual();

#endif
