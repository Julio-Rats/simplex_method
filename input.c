#include <stdio.h>
#include <string.h>
#include <math.h>

#include "structs.h"

#define STR_LEN 128

/*  Global Function  */
matrix_t init_matriz(size_t m, size_t n); // sistema_linear.c

/*  Global local Variables  */
size_t number_base = 0;       // Basic matrix dimensions (B).
size_t number_Nbase = 0;      // Non-Basic Array Dimensions (N).
matrix_t vector_b = NULL;     // Vector b, from the system Ax=b.
variavel_t *var_base = NULL;  // Basic variables.
variavel_t *var_Nbase = NULL; // Non-basic variables.
char signal;                  // External variable, simplex.c
size_t var_index = 0;         // Unique numeric ID for each variable
long double BIGM = 1e9;       // Number associated with the cost of artificial variables, BIGM method

/*  Local Enum  */
typedef enum
{
    START = 0,
    SVAR,
    SNUM,
    DNUM,
    DNUM_O,
    SLEQUAL,
    SGEQUAL,
    COMMENT
} states_t;

typedef enum
{
    NUM = 0,
    VAR,
    MULT,
    DIV,
    SUM,
    SUB,
    NL,
    EF,
    LEQUAL,
    GEQUAL,
    EQUAL,
    OPAREN,
    CPAREN,
    ERROR
} type_token;

/*  Local Struct  */
typedef struct
{
    char value[STR_LEN];
    type_token type;
} token_t;

/*  Local Variables  */
FILE *arq;
token_t token;
size_t number_rest = 0;
size_t number_artif = 1;
size_t number_folga = 1;
size_t number_folgaN = 1;

/*  Local Functions  */
string random_var(string prefixo, size_t index);
token_t get_token();
void strlower(string str);
void cria_var(double escalar, string name_var);
void add_restricao(double escalar, string name_var);
void add_coluna_aj(variavel_t *list, size_t len_list, size_t number_col);

void tipo_otimizacao();
void funcao_objetivo(int oper);
void resto_Fx();
double expr();
double resto_add(double escalar);
double mult();
double uno();
double resto_mult(double escalar);
double fator();

void restricao();
void var_restr(int oper);
void resto_eq();
size_t tipo_des();
void resto_rest();
void add_vet_b(double escalar);
void menor_igual_rest();
void igual_rest();
void maior_igual_rest();

void input_file(string path_file)
{
    if ((arq = fopen(path_file, "rt")) == NULL)
    {
        printf("\n[ERROR]: Failed to open the file: '%s'\n\n", path_file);
        exit(EXIT_FAILURE);
    }
    tipo_otimizacao();
    while (token.type == NL)
        token = get_token();
    restricao();
    fclose(arq);
}

string random_var(string prefixo, size_t index)
{
    string var = (string)malloc(sizeof(char) * (strlen(prefixo) + 7));
    if (!var)
    {
        printf("\n[ERROR]: Memory allocation failure, function random_var()\n\n");
        fclose(arq);
        exit(EXIT_FAILURE);
    }
    sprintf(var, "%s%zu", prefixo, index);
    return var;
}

void strlower(string str)
{
    for (size_t i = 0; i < strlen(str); i++)
        if ((str[i] >= 'A') && (str[i] <= 'Z'))
            str[i] += 'a' - 'A';
}

token_t get_token()
{
    states_t state_atual = START;
    token_t token;
    char char_atual;
    size_t length_char = 0;

    while (true)
    {
        char_atual = fgetc(arq);

        if (state_atual == START)
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
                state_atual = SLEQUAL;
            }
            else if (char_atual == '>')
            {
                token.value[length_char++] = '>';
                state_atual = SGEQUAL;
            }
            else if (char_atual == '#')
            {
                state_atual = COMMENT;
            }
            else if (char_atual == '=')
            {
                strcpy(token.value, "=");
                token.type = EQUAL;
                break;
            }
            else if (char_atual == '+')
            {
                strcpy(token.value, "+");
                token.type = SUM;
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
            else if (char_atual == '/')
            {
                strcpy(token.value, "/");
                token.type = DIV;
                break;
            }
            else if (char_atual == '(')
            {
                strcpy(token.value, "(");
                token.type = OPAREN;
                break;
            }
            else if (char_atual == ')')
            {
                strcpy(token.value, ")");
                token.type = CPAREN;
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
                token.value[length_char++] = '.';
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
        else if (state_atual == SLEQUAL)
        {
            if (char_atual == '=')
            {
                token.value[length_char++] = '=';
                token.value[length_char] = '\0';
                token.type = LEQUAL;
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
        else if (state_atual == SGEQUAL)
        {
            if (char_atual == '=')
            {
                token.value[length_char++] = '=';
                token.value[length_char] = '\0';
                token.type = GEQUAL;
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
        else if (state_atual == COMMENT)
        {
            if (char_atual == '\n')
            {
                ungetc(char_atual, arq);
                token = get_token();
                break;
            }
        }
        if (feof(arq))
        {
            token.type = EF;
            strcpy(token.value, "(Final de Arquivo)");
            break;
        }
    }
    return token;
}

/*
    Grammar for expressions used

<expr> -> <mult> <restoAdd> ;
<restoAdd> -> '+' <mult> <restoAdd>
            | '-' <mult> <restoAdd> | & ;
<mult> -> <uno> <restoMult> ;
<restoMult> -> '*' <uno> <restoMult>
            |  '/' <uno> <restoMult>
            |  '%' <uno> <restoMult> | & ;
<uno> -> '+' <uno> | '-' <uno> | <fator> ;
<fator> -> 'NUMBER'| 'VARIABLE'
            | '(' <atrib> ')' ;

& = end of chain.

OBS: Variable is transformed to value double 1.0
    posteriori variable takes the value of the
    scalar number that precedes it
*/

void tipo_otimizacao()
{
    /*  <max|min|minimize|maximize> [ - ] <funcao_objetivo>  */
    token = get_token();
    if (token.type == NL)
        tipo_otimizacao();
    else if (token.type == VAR)
    {
        strlower(token.value);
        if ((strcmp(token.value, "min") == 0) || (strcmp(token.value, "minimize") == 0))
            signal = 1;
        else if ((strcmp(token.value, "max") == 0) || (strcmp(token.value, "maximize") == 0))
            signal = -1;
        else
        {
            printf("\n[ERROR]: Wrong syntax, expected min or max, but was received '%s'\n\n", token.value);
            fclose(arq);
            exit(EXIT_FAILURE);
        }
        token = get_token();
        if (token.type == SUB)
        {
            token = get_token();
            funcao_objetivo(-1);
        }
        else
            funcao_objetivo(1);
    }
    else if (token.type == EF)
    {
        printf("\n[ERROR]: Wrong syntax, expected objective function declaration, but received '%s'\n\n", token.value);
        fclose(arq);
        exit(EXIT_FAILURE);
    }
}

void funcao_objetivo(int oper) // need
{
    /*  [ <expr> [ * ] ] <var> <resto_Fx>  */
    double escalar = 1.0;

    if (token.type != VAR)
        escalar = expr();

    if (token.type == VAR)
        cria_var(escalar * oper, token.value);
    else
    {
        printf("\n[ERROR]: Wrong syntax, expected mathematical operator or variable, but was received '%s'\n\n", token.value);
        fclose(arq);
        exit(EXIT_FAILURE);
    }

    resto_Fx();
}

double expr() // need
{
    /*  <mult> <resto_add>  */
    double escalar = mult();
    return resto_add(escalar);
}

void resto_Fx()
{
    /*  + <funcao_objetivo> | - <funcao_objetivo> | NL  */
    token = get_token();
    if (token.type == SUM)
    {
        token = get_token();
        funcao_objetivo(1);
    }
    else if (token.type == SUB)
    {
        token = get_token();
        funcao_objetivo(-1);
    }
    else if (token.type != NL)
    {
        printf("\n[ERROR]: Wrong syntax, expected end of objective function or new variable, but was received '%s'\n\n", token.value);
        fclose(arq);
        exit(EXIT_FAILURE);
    }
}

double resto_add(double escalar)
{
    /*  [ + <mult> <resto_add> ] | [ - <mult> <resto_add> ] | &  */
    if (token.type == SUM)
    {
        token = get_token();
        escalar = resto_add(escalar + mult());
    }
    else if (token.type == SUB)
    {
        token = get_token();
        escalar = resto_add(escalar - mult());
    }
    return escalar;
}

double mult() // need
{
    /*  <uno> <resto_mult>  */
    double escalar = uno();
    return resto_mult(escalar);
}

double uno() // need
{
    /*  +<uno> | -<uno> | fator */
    double escalar;
    if (token.type == SUM)
    {
        token = get_token();
        escalar = uno();
    }
    else if (token.type == SUB)
    {
        token = get_token();
        escalar = -uno();
    }
    else
        escalar = fator();

    return escalar;
}

double resto_mult(double escalar)
{
    /*  [ * <uno> <resto_mult> ] | [ / <uno> <resto_mult> ] | &  */
    token = get_token();
    if (token.type == MULT)
    {
        token = get_token();
        escalar = resto_mult(escalar * uno());
    }
    else if (token.type == DIV)
    {
        token = get_token();
        double aux = uno();
        if (aux == 0)
        {
            printf("\n[ERROR]: Division by zero in restrictions file\n\n");
            fclose(arq);
            exit(EXIT_FAILURE);
        }
        escalar = resto_mult(escalar / aux);
    }
    return escalar;
}

double fator() // nedd
{
    /*  NUM | VAR | (  */
    if (token.type == NUM)
        return atof(token.value);
    else if (token.type == VAR)
        return (double)1.0;
    else if (token.type == OPAREN)
    {
        token = get_token();
        double aux = expr();
        if (token.type != CPAREN)
        {
            printf("\n[ERROR]: Wrong syntax, expected to close parenthesis, but was received '%s'\n\n", token.value);
            fclose(arq);
            exit(EXIT_FAILURE);
        }
        return aux;
    }

    printf("\n[ERROR]: Wrong syntax, expected an expression, but was received '%s'\n\n", token.value);
    fclose(arq);
    exit(EXIT_FAILURE);
}

void restricao()
{
    /*  [ - ] <var_restr> <tipo_des> <expr> <resto_rest>  */

    number_rest++;
    add_coluna_aj(var_base, number_base, number_rest);
    add_coluna_aj(var_Nbase, number_Nbase, number_rest);

    if (token.type == SUB)
    {
        token = get_token();
        var_restr(-1.0);
    }
    else if (token.type == SUM)
    {
        token = get_token();
        var_restr(1.0);
    }
    else
        var_restr(1.0);

    switch (tipo_des())
    {
    case LEQUAL:
        menor_igual_rest();
        break;

    case EQUAL:
        igual_rest();
        break;

    case GEQUAL:
        maior_igual_rest();
        break;
    }

    token = get_token();
    add_vet_b(expr());
    resto_rest();
}

void var_restr(int oper) // need
{
    /*  [ <expr> [ * ] ] <var> <resto_eq>  */
    double escalar = 1.0;

    if (token.type != VAR)
        escalar = expr();

    if (token.type == VAR)
        add_restricao(escalar * oper, token.value);
    else
    {
        printf("\n[ERROR]: Wrong syntax, expected mathematical operator or variable, but was received '%s'\n\n", token.value);
        fclose(arq);
        exit(EXIT_FAILURE);
    }

    resto_eq();
}

void resto_eq()
{
    /*  + <var_restr> | - <var_restr> | &  */
    token = get_token();
    if (token.type == SUM)
    {
        token = get_token();
        var_restr(1);
    }
    else if (token.type == SUB)
    {
        token = get_token();
        var_restr(-1);
    }
}

size_t tipo_des() // need
{
    /*  <= | = | >=  */
    if (token.type == LEQUAL)
        return LEQUAL;
    else if (token.type == EQUAL)
        return EQUAL;
    else if (token.type == GEQUAL)
        return GEQUAL;
    else
    {
        printf("\n[ERROR]: Wrong syntax, expected mathematical operator or variable, but was received '%s'\n\n", token.value);
        fclose(arq);
        exit(EXIT_FAILURE);
    }
}

void resto_rest()
{
    /* !EF <restricao> | EF */
    while (token.type == NL)
        token = get_token();

    if (token.type == EF)
        return;
    restricao();
}

void add_vet_b(double escalar)
{
    if (number_rest == 1)
        vector_b = (matrix_t)malloc(sizeof(vector_t));
    else
        vector_b = (matrix_t)realloc(vector_b, sizeof(vector_t) * number_rest);

    if (!vector_b)
    {
        printf("\n[ERROR]: Memory allocation failure, function add_vet_b()\n\n");
        fclose(arq);
        exit(EXIT_FAILURE);
    }

    vector_b[number_rest - 1] = (vector_t)malloc(sizeof(double));
    vector_b[number_rest - 1][0] = escalar;
}

void menor_igual_rest()
{
    matrix_t vetor_aux = init_matriz(1, number_rest);
    vetor_aux[0][number_rest - 1] = 1.0;
    variavel_t var_folga = {0.0, random_var("Slack_", number_folga++), var_index++, FOLGA, vetor_aux};

    if (++number_base == 1)
        var_base = (variavel_t *)malloc(sizeof(variavel_t));
    else
        var_base = (variavel_t *)realloc(var_base, sizeof(variavel_t) * number_base);

    if (!var_base)
    {
        printf("\n[ERROR]: Memory allocation failure, function menor_igual_rest()\n\n");
        fclose(arq);
        exit(EXIT_FAILURE);
    }
    var_base[number_base - 1] = var_folga;
}

void igual_rest()
{
    matrix_t vetor_aux = init_matriz(1, number_rest);
    vetor_aux[0][number_rest - 1] = 1.0;
    variavel_t var_folga = {fabsl(BIGM), random_var("Artif_", number_artif++), var_index++, ARTIFICIAL, vetor_aux};

    if (++number_base == 1)
        var_base = (variavel_t *)malloc(sizeof(variavel_t));
    else
        var_base = (variavel_t *)realloc(var_base, sizeof(variavel_t) * number_base);

    if (!var_base)
    {
        printf("\n[ERROR]: Memory allocation failure, function igual_rest()\n\n");
        fclose(arq);
        exit(EXIT_FAILURE);
    }
    var_base[number_base - 1] = var_folga;
}

void maior_igual_rest()
{
    matrix_t vetor_aux = init_matriz(1, number_rest);
    vetor_aux[0][number_rest - 1] = 1.0;
    variavel_t var_folga = {fabsl(BIGM), random_var("Artif_", number_artif++), var_index++, ARTIFICIAL, vetor_aux};

    if (++number_base == 1)
        var_base = (variavel_t *)malloc(sizeof(variavel_t));
    else
        var_base = (variavel_t *)realloc(var_base, sizeof(variavel_t) * number_base);

    if (!var_base)
    {
        printf("\n[ERROR]: Memory allocation failure, function maior_igual_rest()\n\n");
        fclose(arq);
        exit(EXIT_FAILURE);
    }
    var_base[number_base - 1] = var_folga;

    vetor_aux = init_matriz(1, number_rest);
    vetor_aux[0][number_rest - 1] = -1.0;
    variavel_t var_folgaN = {0.0, random_var("NegSlack_", number_folgaN++), var_index++, FOLGA, vetor_aux};

    if (++number_Nbase == 1)
        var_Nbase = (variavel_t *)malloc(sizeof(variavel_t));
    else
        var_Nbase = (variavel_t *)realloc(var_Nbase, sizeof(variavel_t) * number_Nbase);

    if (!var_Nbase)
    {
        printf("\n[ERROR]: Memory allocation failure, function maior_igual_rest()\n\n");
        fclose(arq);
        exit(EXIT_FAILURE);
    }
    var_Nbase[number_Nbase - 1] = var_folgaN;
}

void cria_var(double escalar, string name_var)
{
    for (size_t i = 0; i < number_Nbase; i++)
        if (strcmp(name_var, var_Nbase[i].name) == 0)
        {
            printf("\n[ERROR]: Variable used more than once in the objective function, variable '%s'\n\n", name_var);
            fclose(arq);
            exit(EXIT_FAILURE);
        }

    variavel_t var;
    var.cost = (double)(signal * escalar);
    var.name = (string)malloc(sizeof(char) * (strlen(token.value) + 1));
    var.index = var_index++;
    var.type = ORIGINAL;
    var.aj = (matrix_t)malloc(sizeof(vector_t));

    if (!var.name || !var.aj)
    {
        printf("\n[ERROR]: Memory allocation failure, function cria_var()\n\n");
        fclose(arq);
        exit(EXIT_FAILURE);
    }
    strcpy(var.name, token.value);

    if (++number_Nbase == 1)
        var_Nbase = (variavel_t *)malloc(sizeof(variavel_t));
    else
        var_Nbase = (variavel_t *)realloc(var_Nbase, sizeof(variavel_t) * number_Nbase);

    if (!var_Nbase)
    {
        printf("\n[ERROR]: Memory allocation failure, function cria_var()\n\n");
        fclose(arq);
        exit(EXIT_FAILURE);
    }
    var_Nbase[number_Nbase - 1] = var;
}

void add_restricao(double escalar, string name_var)
{
    bool existe = false;
    size_t var_id;
    for (size_t i = 0; i < number_Nbase; i++)
        if (strcmp(name_var, var_Nbase[i].name) == 0)
        {
            existe = true;
            var_id = i;
            break;
        }
    if (!existe)
    {
        printf("\n[ERROR]: Unknown variable used in restrictions, variable '%s'\n\n", name_var);
        fclose(arq);
        exit(EXIT_FAILURE);
    }
    var_Nbase[var_id].aj[0][number_rest - 1] += escalar;
}

void add_coluna_aj(variavel_t *list, size_t len_list, size_t number_col)
{
    for (size_t i = 0; i < len_list; i++)
    {
        if (number_col == 1)
            list[i].aj[0] = (vector_t)malloc(sizeof(double));
        else
            list[i].aj[0] = (vector_t)realloc(list[i].aj[0], sizeof(double) * number_col);

        if (!list[i].aj[0])
        {
            printf("\n[ERROR]: Memory allocation failure, function add_coluna_aj()\n\n");
            fclose(arq);
            exit(EXIT_FAILURE);
        }
        list[i].aj[0][number_col - 1] = 0;
    }
}
