#ifndef _INPUT_
#define _INPUT_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "structs.h"

#define STR_LEN 128

typedef enum
{
	INICIO = 0,
	SVAR,
	SNUM,
	DNUM,
	DNUM_O,
	SMENORIGUAL,
	SMAIORIGUAL
} states;

typedef enum
{
	NUM = 0,
	VAR,
	MULT,
	SOMA,
	SUB,
	NL,
	EF,
	MENORIGUAL,
	MAIORIGUAL,
	IGUAL,
	ERROR
} type_token;

typedef struct
{
	char value[STR_LEN];
	type_token type;
} token_t;

extern variavel_t *list_var;
extern size_t len_var;

void input_file(string path_file);
void strlower(string str);

#endif
