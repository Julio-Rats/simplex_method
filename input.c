#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "input.h"

#define STR_LEN 128

/*
	Global Variables
*/
variavel_t* list_var  = NULL; // Vetor auxiliar para ler restri��es
size_t len_var        = 0;    // Length do vetor list_var.
size_t number_base    = 0;    // Dimens�o da matriz basica (B).
size_t number_Nbase   = 0;    // Dimens�o da matriz n?o basica (N).
double** vetor_b      = NULL; // Vetor B.
variavel_t* var_base  = NULL; // Variveis basicas.
variavel_t* var_Nbase = NULL; // Variveis n�o basicas.
int sinal;

/*
	Local Variables
*/
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

FILE *arq;
token_t token;
size_t number_artif = 1;
size_t number_folga = 1;
size_t number_artifN = 1;

/*
	Local Functions
*/
string random_var(string prefixo, size_t index);
token_t get_token();
void strlower(string str);
void uniao_var();
void tipo_otimizacao();
void funcao_objetivo();
void restricao();
void proxima();
void operacao(int sinal_var);
void variavel(double escalar);
void continua_restrict();
void nova_restricao();
void escalar_restrict(int sinal_var);
void variavel_restrict(double escalar);
void menor_igual();
void maior_igual();
void igual();

string random_var(string prefixo, size_t index)
{
	string var = (string)malloc(sizeof(char) * (strlen(prefixo) + 6));
	sprintf(var, "%s%d", prefixo, index);
	return var;
}

void strlower(string str)
{
	for (size_t i = 0; i < str[i]; i++)
		if ((str[i] >= 'A') && (str[i] <= 'Z'))
			str[i] += 'a' - 'A';
}

token_t get_token()
{
	states state_atual = INICIO;
	token_t token;
	char char_atual;
	size_t length_char = 0;

	while (true)
	{
		char_atual = fgetc(arq);

		if (state_atual == INICIO)
		{
			if (((char_atual >= 'a') && (char_atual <= 'z')) || ((char_atual >= 'A') && (char_atual <= 'Z')))
			{
				token.value[length_char++] = char_atual;
				state_atual = SVAR;
			}
			else if ((char_atual >= '0') && (char_atual <= '9'))
			{
				token.value[length_char++] = char_atual;
				state_atual = SNUM;
			}
			else if (char_atual == '<')
			{
				token.value[length_char++] = '<';
				state_atual = SMENORIGUAL;
			}
			else if (char_atual == '>')
			{
				token.value[length_char++] = '>';
				state_atual = SMAIORIGUAL;
			}
			else if (char_atual == '=')
			{
				strcpy(token.value, "=");
				token.type = IGUAL;
				break;
			}
			else if (char_atual == '+')
			{
				strcpy(token.value, "+");
				token.type = SOMA;
				break;
			}
			else if (char_atual == '-')
			{
				strcpy(token.value, "-");
				token.type = SUB;
				break;
			}
			else if (char_atual == '*')
			{
				strcpy(token.value, "*");
				token.type = MULT;
				break;
			}
			else if (char_atual == '\n')
			{
				strcpy(token.value, "(Nova Linha)");
				token.type = NL;
				break;
			}
		}
		else if (state_atual == SVAR)
		{
			if (((char_atual >= 'a') && (char_atual <= 'z')) || ((char_atual >= 'A') && (char_atual <= 'Z')) || ((char_atual >= '0') && (char_atual <= '9')))
			{
				token.value[length_char++] = char_atual;
			}
			else
			{
				token.value[length_char] = '\0';
				token.type = VAR;
				ungetc(char_atual, arq);
				break;
			}
		}
		else if (state_atual == SNUM)
		{
			if ((char_atual >= '0') && (char_atual <= '9'))
			{
				token.value[length_char++] = char_atual;
			}
			else if ((char_atual == '.') || (char_atual == ','))
			{
				token.value[length_char++] = ',';
				state_atual = DNUM_O;
			}
			else
			{
				token.value[length_char] = '\0';
				token.type = NUM;
				ungetc(char_atual, arq);
				break;
			}
		}
		else if (state_atual == DNUM_O)
		{
			if ((char_atual >= '0') && (char_atual <= '9'))
			{
				token.value[length_char++] = char_atual;
				state_atual = DNUM;
			}
			else
			{
				token.value[length_char++] = char_atual;
				token.value[length_char] = '\0';
				token.type = ERROR;
				break;
			}
		}
		else if (state_atual == DNUM)
		{
			if ((char_atual >= '0') && (char_atual <= '9'))
			{
				token.value[length_char++] = char_atual;
			}
			else if ((char_atual == '.') || (char_atual == ','))
			{
				token.value[length_char++] = char_atual;
				token.value[length_char] = '\0';
				token.type = ERROR;
				break;
			}
			else
			{
				token.value[length_char] = '\0';
				token.type = NUM;
				ungetc(char_atual, arq);
				break;
			}
		}
		else if (state_atual == SMENORIGUAL)
		{
			if (char_atual == '=')
			{
				token.value[length_char++] = '=';
				token.value[length_char] = '\0';
				token.type = MENORIGUAL;
				break;
			}
			else
			{
				token.value[length_char] = '\0';
				token.type = ERROR;
				ungetc(char_atual, arq);
				break;
			}
		}
		else if (state_atual == SMAIORIGUAL)
		{
			if (char_atual == '=')
			{
				token.value[length_char++] = '=';
				token.value[length_char] = '\0';
				token.type = MAIORIGUAL;
				break;
			}
			else
			{
				token.value[length_char] = '\0';
				token.type = ERROR;
				ungetc(char_atual, arq);
				break;
			}
		}
		if (feof(arq))
		{
			token.type = EF;
			strcpy(token.value,"(Final de Arquivo)");
			break;
		}
	}
	return token;
}

void input_file(string path_file)
{
	if ((arq = fopen(path_file, "rt")) == NULL)
	{
		printf("\n[ERRO] Falha ao tentar abrir o arquivo: %s\n\n", path_file);
		exit(EXIT_FAILURE);
	}
	tipo_otimizacao();
	funcao_objetivo();
	restricao();
	for (size_t i = 0; i < len_var; i++)
		if (list_var[i].aj == NULL)
		{
			list_var[i].aj = (double**)malloc(sizeof(double*));
			list_var[i].aj[0] = (double*)calloc(number_base, sizeof(double));
		}
	uniao_var();
	fclose(arq);
}

void tipo_otimizacao()
{
	token = get_token();
	if (token.type == VAR)
	{
		strlower(token.value);
		if ((strcmp(token.value, "min") == 0) || (strcmp(token.value, "minimize") == 0))
		{
			sinal = 1;
			return;
		}
		else if ((strcmp(token.value, "max") == 0) || (strcmp(token.value, "maximize") == 0))
		{
			sinal = -1;
			return;
		}
	}
	else if (token.type == EF)
	{
		printf("\n[ERRO] Sintaxe errada, esperava declara��o da fun��o objetiva\n\n", token.value);
		exit(EXIT_FAILURE);
	}
	else if (token.type == NL)
	{
		tipo_otimizacao();
		return;
	}
	printf("\n[ERRO] Sintaxe errada, esperava min ou max, por�m foi recebido '%s'\n\n", token.value);
	exit(EXIT_FAILURE);
}

void funcao_objetivo()
{
	token = get_token();
	if (token.type == SOMA)
		operacao(1);
	else if (token.type == SUB)
		operacao(-1);
	else if (token.type == NUM)
		variavel(atof(token.value));
	else if (token.type == VAR)
	{
		variavel_t aux;
		aux.cost = (double)sinal;
		aux.name = (string)malloc(sizeof(char) * (strlen(token.value) + 1));
		aux.aj = NULL;
		strcpy(aux.name, token.value);
		aux.type = ORIGINAL;
		if (len_var == 0)
			list_var = (variavel_t*)malloc(sizeof(variavel_t) * (++len_var));
		else
			list_var = (variavel_t*)realloc(list_var, sizeof(variavel_t) * (++len_var));
		list_var[len_var - 1] = aux;
		proxima();
	}
	else
	{
		printf("\n[ERRO] Sintaxe errada, esperava um escalar ou uma variavel, por�m foi recebido %s\n\n", token.value);
		exit(EXIT_FAILURE);
	}
}

void proxima()
{
	token = get_token();
	if (token.type == SOMA)
		operacao(1);
	else if (token.type == SUB)
		operacao(-1);
	else if (token.type != NL)
	{
		printf("\n[ERRO] Sintaxe errada, esperava um escalar ou uma variavel, por�m foi recebido %s\n\n", token.value);
		exit(EXIT_FAILURE);
	}
}

void operacao(int sinal_var)
{
	token = get_token();
	if (token.type == NUM)
		variavel((double)(atof(token.value) * sinal_var));
	else if (token.type == VAR)
	{
		variavel_t aux;
		aux.cost = (double)(sinal * sinal_var);
		aux.name = (string)malloc(sizeof(char) * (strlen(token.value) + 1));
		aux.aj = NULL;
		strcpy(aux.name, token.value);
		aux.type = ORIGINAL;
		if (len_var == 0)
			list_var = (variavel_t*)malloc(sizeof(variavel_t) * (++len_var));
		else
			list_var = (variavel_t*)realloc(list_var, sizeof(variavel_t) * (++len_var));
		list_var[len_var - 1] = aux;
		proxima();
	}
	else
	{
		printf("\n[ERRO] Sintaxe errada, esperava um escalar ou uma variavel, por�m foi recebido %s\n\n", token.value);
		exit(EXIT_FAILURE);
	}
}

void variavel(double escalar)
{
	token = get_token();
	if (token.type == MULT)
		token = get_token();
	if (token.type == VAR)
	{
		variavel_t aux;
		aux.cost = (double)(sinal * escalar);
		aux.name = (string)malloc(sizeof(char) * (strlen(token.value) + 1));
		aux.aj = NULL;
		strcpy(aux.name, token.value);
		aux.type = ORIGINAL;
		if (len_var == 0)
			list_var = (variavel_t*)malloc(sizeof(variavel_t) * (++len_var));
		else
			list_var = (variavel_t*)realloc(list_var, sizeof(variavel_t) * (++len_var));
		list_var[len_var - 1] = aux;
		proxima();
	}
	else
	{
		printf("\n[ERRO] Sintaxe errada, esperava uma variavel, por�m foi recebido %s\n\n", token.value);
		exit(EXIT_FAILURE);
	}
}

void restricao()
{
	while (token.type == NL)
		token = get_token();
	if (token.type == EF)
	{
		printf("\n[ERRO] Sintaxe errada, esperava conjunto de restri��o\n\n");
		exit(EXIT_FAILURE);
	}
	if (token.type == SOMA)
		escalar_restrict(1);
	else if (token.type == SUB)
		escalar_restrict(-1);
	else if (token.type == NUM)
		variavel_restrict((double)atof(token.value));
	else if (token.type == VAR)
	{
		bool exist = false;
		for (size_t i = 0; i < len_var; i++)
			if (strcmp(list_var[i].name, token.value) == 0)
			{
				exist = true;
				if (list_var[i].aj == NULL)
				{
					list_var[i].aj = (double**)malloc(sizeof(double*));
					list_var[i].aj[0] = (double*)calloc((number_base + 1), sizeof(double));
				}
				else
				{
					list_var[i].aj[0] = (double*)realloc(list_var[i].aj[0], sizeof(double) * (number_base + 1));
				}
				list_var[i].aj[0][number_base] = 1;
				break;
			}
		if (!exist)
		{
			if (len_var == 0)
				list_var = (variavel_t*)malloc(sizeof(variavel_t) * (++len_var));
			else
				list_var = (variavel_t*)realloc(list_var, sizeof(variavel_t) * (++len_var));

			double**vetor_aux = (double**)malloc(sizeof(double*));
			vetor_aux[0] = (double*)calloc(number_base + 1, sizeof(double));
			vetor_aux[0][number_base] = 1;

			variavel_t aux = {0, NULL, ORIGINAL, vetor_aux};
			aux.name = (string)malloc(sizeof(char) * (strlen(token.value) + 1));
			strcpy(aux.name, token.value);

			list_var[len_var - 1] = aux;
		}
		continua_restrict();
	}
	else
	{
		printf("\n[ERRO] Sintaxe errada, esperava uma variavel, por�m foi recebido %s\n\n", token.value);
		exit(EXIT_FAILURE);
	}
}

void nova_restricao()
{
	token = get_token();
	if (token.type != EF)
		if (token.type == SOMA)
			escalar_restrict(1);
		else if (token.type == SUB)
			escalar_restrict(-1);
		else if (token.type == NUM)
			variavel_restrict((double)atof(token.value));
		else if (token.type == VAR)
		{
			bool exist = false;
			for (size_t i = 0; i < len_var; i++)
				if (strcmp(list_var[i].name, token.value) == 0)
				{
					exist = true;
					if (list_var[i].aj == NULL)
					{
						list_var[i].aj = (double**)malloc(sizeof(double*));
						list_var[i].aj[0] = (double*)calloc((number_base + 1), sizeof(double));
					}
					else
					{
						list_var[i].aj[0] = (double*)realloc(list_var[i].aj[0], sizeof(double) * (number_base + 1));
					}
					list_var[i].aj[0][number_base] = 1;
					break;
				}
			if (!exist)
			{
				if (len_var == 0)
					list_var = (variavel_t*)malloc(sizeof(variavel_t) * (++len_var));
				else
					list_var = (variavel_t*)realloc(list_var, sizeof(variavel_t) * (++len_var));

				double**vetor_aux = (double**)malloc(sizeof(double*));
				vetor_aux[0] = (double*)calloc(number_base + 1, sizeof(double));
				vetor_aux[0][number_base] = 1;

				variavel_t aux = {0, NULL, ORIGINAL, vetor_aux};
				aux.name = (string)malloc(sizeof(char) * (strlen(token.value) + 1));
				strcpy(aux.name, token.value);

				list_var[len_var - 1] = aux;
			}
			continua_restrict();
		}
		else if (token.type == NL)
		{
			nova_restricao();
		}
		else 
		{
			printf("\n[ERRO] Sintaxe errada, esperava uma variavel, por�m foi recebido %s\n\n", token.value);
			exit(EXIT_FAILURE);
		}
}

void continua_restrict()
{
	token = get_token();
	if (token.type == SOMA)
		escalar_restrict(1);
	else if (token.type == SUB)
		escalar_restrict(-1);
	else if (token.type == MENORIGUAL)
		menor_igual();
	else if (token.type == MAIORIGUAL)
		maior_igual();
	else if (token.type == IGUAL)
		igual();
	else if (token.type == MULT)
	{
		printf("\n[ERRO] Sintaxe errada, esperava um escalar ou uma variavel, por�m foi recebido %s\n\n", token.value);
		exit(EXIT_FAILURE);
	}
	else if (token.type == VAR)
	{
		printf("\n[ERRO] Sintaxe errada, esperava um escalar ou uma variavel, por�m foi recebido %s\n\n", token.value);
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("\n[ERRO] Sintaxe errada, esperava uma variavel ou sinal ou simbolo de equa��o ou inequa��o, por�m foi recebido '%s'\n\n", token.value);
		exit(EXIT_FAILURE);
	}
}

void escalar_restrict(int sinal_var)
{
	token = get_token();
	if (token.type == NUM)
		variavel_restrict((double)(atof(token.value) * sinal_var));
	else if (token.type == VAR)
	{
		bool exist = false;
		for (size_t i = 0; i < len_var; i++)
			if (strcmp(list_var[i].name, token.value) == 0)
			{
				exist = true;
				if (list_var[i].aj == NULL)
				{
					list_var[i].aj = (double**)malloc(sizeof(double*));
					list_var[i].aj[0] = (double*)calloc((number_base + 1), sizeof(double));
				}
				else
				{
					list_var[i].aj[0] = (double*)realloc(list_var[i].aj[0], sizeof(double) * (number_base + 1));
				}
				list_var[i].aj[0][number_base] = (double)sinal_var;
				break;
			}
		if (!exist)
		{
			if (len_var == 0)
				list_var = (variavel_t*)malloc(sizeof(variavel_t) * (++len_var));
			else
				list_var = (variavel_t*)realloc(list_var, sizeof(variavel_t) * (++len_var));

			double**vetor_aux = (double**)malloc(sizeof(double*));
			vetor_aux[0] = (double*)calloc(number_base + 1, sizeof(double));
			vetor_aux[0][number_base] = (double)sinal_var;

			variavel_t aux = {0, NULL, ORIGINAL, vetor_aux};
			aux.name = (string)malloc(sizeof(char) * (strlen(token.value) + 1));
			strcpy(aux.name, token.value);
			list_var[len_var - 1] = aux;
		}
		continua_restrict();
	}
	else
	{
		printf("\n[ERRO] Sintaxe errada, esperava um escalar ou uma variavel, por�m foi recebido %s\n\n", token.value);
		exit(EXIT_FAILURE);
	}
}

void variavel_restrict(double escalar)
{
	token = get_token();
	if (token.type == MULT)
		token = get_token();

	if (token.type == VAR)
	{
		bool exist = false;
		for (size_t i = 0; i < len_var; i++)
			if (strcmp(list_var[i].name, token.value) == 0)
			{
				exist = true;
				if (list_var[i].aj == NULL)
				{
					list_var[i].aj = (double**)malloc(sizeof(double*));
					list_var[i].aj[0] = (double*)calloc((number_base + 1), sizeof(double));
				}
				else
				{
					list_var[i].aj[0] = (double*)realloc(list_var[i].aj[0], sizeof(double) * (number_base + 1));
				}
				list_var[i].aj[0][number_base] = escalar;
				break;
			}
		if (!exist)
		{
			if (len_var == 0)
				list_var = (variavel_t*)malloc(sizeof(variavel_t) * (++len_var));
			else
				list_var = (variavel_t*)realloc(list_var, sizeof(variavel_t) * (++len_var));

			double**vetor_aux = (double**)malloc(sizeof(double*));
			vetor_aux[0] = (double*)calloc(number_base + 1, sizeof(double));
			vetor_aux[0][number_base] = escalar;

			variavel_t aux = {0, NULL, ORIGINAL, vetor_aux};
			aux.name = (string)malloc(sizeof(char) * (strlen(token.value) + 1));
			strcpy(aux.name, token.value);

			list_var[len_var - 1] = aux;
		}
		continua_restrict();
	}
	else
	{
		printf("\n[ERRO] Sintaxe errada, esperava uma variavel, por�m foi recebido %s\n\n", token.value);
		exit(EXIT_FAILURE);
	}
}

void menor_igual()
{
	token = get_token();
	if (token.type == NUM)
	{
		for (size_t i = 0; i < number_base; i++)
		{
			var_base[i].aj[0] = (double*)realloc(var_base[i].aj[0], sizeof(double) * (number_base + 1));
			var_base[i].aj[0][number_base] = 0;
		}

		if (number_base == 0)
		{
			var_base = (variavel_t*)malloc(sizeof(variavel_t) * (++number_base));
			vetor_b = (double**)malloc(sizeof(double*) * (number_base));
			vetor_b[0] = (double*)malloc(sizeof(double));
		}
		else
		{
			var_base = (variavel_t*)realloc(var_base, sizeof(variavel_t) * (++number_base));
			vetor_b = (double**)realloc(vetor_b, sizeof(double*) * (number_base));
			vetor_b[number_base - 1] = (double*)malloc(sizeof(double));
		}
		vetor_b[number_base - 1][0] = (double)atof(token.value);

		double**vetor_aux = (double**)malloc(sizeof(double*));
		vetor_aux[0] = (double*)calloc(number_base, sizeof(double));
		vetor_aux[0][number_base - 1] = 1;

		variavel_t aux = {0, random_var("folga", number_folga++), FOLGA, vetor_aux};
		var_base[number_base - 1] = aux;
	}
	else
	{
		printf("\n[ERRO] Sintaxe errada, esperava um valor apos o menor igual, por�m foi recebido %s\n\n", token.value);
		exit(EXIT_FAILURE);
	}

	token = get_token();
	if (token.type != NL && token.type != EF)
	{
		printf("\n[ERRO] Sintaxe errada, esperava uma nova linha de restri��o, por�m foi recebido %s\n\n", token.value);
		exit(EXIT_FAILURE);
	}
	nova_restricao();
}

void maior_igual()
{
	token = get_token();
	if (token.type == NUM)
	{
		for (size_t i = 0; i < number_base; i++)
		{
			var_base[i].aj[0] = (double*)realloc(var_base[i].aj[0], sizeof(double) * (number_base + 1));
			var_base[i].aj[0][number_base] = 0;
		}

		for (size_t i = 0; i < number_Nbase; i++)
		{
			var_Nbase[i].aj[0] = (double*)realloc(var_Nbase[i].aj[0], sizeof(double) * (number_base + 1));
			var_Nbase[i].aj[0][number_base] = 0;
		}

		if (number_base == 0)
		{
			var_base = (variavel_t*)malloc(sizeof(variavel_t) * (++number_base));
			vetor_b = (double**)malloc(sizeof(double*) * (number_base));
			vetor_b[0] = (double*)malloc(sizeof(double));
		}
		else
		{
			var_base = (variavel_t*)realloc(var_base, sizeof(variavel_t) * (++number_base));
			vetor_b = (double**)realloc(vetor_b, sizeof(double*) * (number_base));
			vetor_b[number_base - 1] = (double*)malloc(sizeof(double));
		}
		vetor_b[number_base - 1][0] = (double)atof(token.value);

		double**vetor_aux = (double**)malloc(sizeof(double*));
		vetor_aux[0] = (double*)calloc(number_base, sizeof(double));
		vetor_aux[0][number_base - 1] = 1;

		variavel_t aux = {(double)abs(BIGM), random_var("artif", number_artif++), ARTIFICIAL, vetor_aux};
		var_base[number_base - 1] = aux;

		if (number_Nbase == 0)
			var_Nbase = (variavel_t*)malloc(sizeof(variavel_t) * (++number_Nbase));
		else
			var_Nbase = (variavel_t*)realloc(var_Nbase, sizeof(variavel_t) * (++number_Nbase));

		vetor_aux = (double**)malloc(sizeof(double*));
		vetor_aux[0] = (double*)calloc(number_base, sizeof(double));
		vetor_aux[0][number_base - 1] = -1;

		variavel_t aux1 = {0, random_var("folgaN", number_artifN++), FOLGA, vetor_aux};
		var_Nbase[number_Nbase - 1] = aux1;
	}
	else
	{
		printf("\n[ERRO] Sintaxe errada, esperava um valor apos o maior igual, por�m foi recebido %s\n\n", token.value);
		exit(EXIT_FAILURE);
	}

	token = get_token();
	if (token.type != NL && token.type != EF)
	{
		printf("\n[ERRO] Sintaxe errada, esperava uma nova linha de restri��o, por�m foi recebido %s\n\n", token.value);
		exit(EXIT_FAILURE);
	}
	nova_restricao();
}

void igual()
{
	token = get_token();
	if (token.type == NUM)
	{
		for (size_t i = 0; i < number_base; i++)
		{
			var_base[i].aj[0] = (double*)realloc(var_base[i].aj[0], sizeof(double) * (number_base + 1));
			var_base[i].aj[0][number_base] = 0;
		}

		for (size_t i = 0; i < number_Nbase; i++)
		{
			var_Nbase[i].aj[0] = (double*)realloc(var_Nbase[i].aj[0], sizeof(double) * (number_base + 1));
			var_Nbase[i].aj[0][number_base] = 0;
		}

		if (number_base == 0)
		{
			var_base = (variavel_t*)malloc(sizeof(variavel_t) * (++number_base));
			vetor_b = (double**)malloc(sizeof(double*) * (number_base));
			vetor_b[0] = (double*)malloc(sizeof(double));
		}
		else
		{
			var_base = (variavel_t*)realloc(var_base, sizeof(variavel_t) * (++number_base));
			vetor_b = (double**)realloc(vetor_b, sizeof(double*) * (number_base));
			vetor_b[number_base - 1] = (double*)malloc(sizeof(double));
		}
		vetor_b[number_base - 1][0] = (double)atof(token.value);

		double**vetor_aux = (double**)malloc(sizeof(double*));
		vetor_aux[0] = (double*)calloc(number_base, sizeof(double));
		vetor_aux[0][number_base - 1] = 1;

		variavel_t aux = {(double)abs(BIGM), random_var("artif", number_artif++), ARTIFICIAL, vetor_aux};
		var_base[number_base - 1] = aux;
	}
	else
	{
		printf("\n[ERRO] Sintaxe errada, esperava um valor apos a igualdade, por�m foi recebido %s\n\n", token.value);
		exit(EXIT_FAILURE);
	}

	token = get_token();
	if (token.type != NL && token.type != EF)
	{
		printf("\n[ERRO] Sintaxe errada, esperava uma nova linha de restri��o, por�m foi recebido %s\n\n", token.value);
		exit(EXIT_FAILURE);
	}
	nova_restricao();
}

void uniao_var()
{
	var_Nbase = (variavel_t*)realloc(var_Nbase, sizeof(variavel_t) * (len_var + number_Nbase));
	for (size_t i = number_Nbase, j = 0; i < (len_var + number_Nbase); j++, i++)
		var_Nbase[i] = list_var[j];

	number_Nbase += len_var;
	free(list_var);
	len_var = 0;
}
