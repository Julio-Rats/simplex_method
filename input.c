#include "input.h"

char types_token[][15] = {"NUM", "VAR", "MULT", "SOMA", "SUB", "NL", "EF", "MENOR", "MENORIGUAL",
"MAIOR", "MAIORIGUAL", "IGUAL"};

variavel_t*   list_var = NULL;
int           num_var  = 0;

string* split(string line, char token, int* length)
{
    string* tokens = NULL;
    int pos_i = 0, pos_f = 0, num = 0;
    *length = 0;

    for(int i=0; line[i] != '\0'; i++)
    {
        if (line[i] == token)
        {
            if (pos_i == i)
            {
                pos_i = i+1;
                continue;
            }

            pos_f = i;
            if (!tokens)
              tokens = (string*) malloc(sizeof(string)*(++num));
            else
              tokens = (string*) realloc(tokens, sizeof(string)*(++num));

            tokens[num-1] = (string) malloc(sizeof(char)*64);
            memcpy(tokens[num-1], line+pos_i, pos_f-pos_i);
            tokens[num-1][pos_f-pos_i+1] = '\0';
            (*length)++;
            pos_i = i+1;
        }
        if (line[i+1] == '\0')
        {
            if (pos_f == i)
                continue;

            pos_f = i+1;
            if (!tokens)
              tokens = (string*) malloc(sizeof(string)*(++num));
            else
              tokens = (string*) realloc(tokens, sizeof(string)*(++num));

            tokens[num-1] = (string) malloc(sizeof(char)*64);
            memcpy(tokens[num-1], line+pos_i, pos_f-pos_i);
            tokens[num-1][pos_f-pos_i+1] = '\0';
            (*length)++;
        }
    }
    return tokens;
}

string trim(string line, char token)
{
    int pos_i = 0, pos_f = 0;

    for (int i = 0; line[i] != '\0'; i++)
    {
        if ((pos_i == 0 )&&(line[i] != token))
            pos_i = i;

        if (line[i] != token)
            pos_f = i;
    }
    string line_trim = (string) malloc(sizeof(char)*(pos_f-pos_i+2));
    memcpy(line_trim, line+pos_i, pos_f-pos_i+1);
    line_trim[pos_f-pos_i+1] = '\0';

    return line_trim;
}


token_t get_token()
{
    states   state_atual = INICIO;
    token_t  token;
    char     char_atual;
    int      length_char = 0;

    while (1)
    {
        char_atual = fgetc(arq);

        if (state_atual == INICIO)
        {
            if (((char_atual >= 'a')&&(char_atual <= 'z'))||((char_atual >= 'A')&&(char_atual <= 'Z')))
            {
                token.value[length_char++] = char_atual;
                state_atual = SVAR;
            }else if ((char_atual >= '0')&&(char_atual <= '9'))
            {
                token.value[length_char++] = char_atual;
                state_atual = SNUM;
            }else if (char_atual == '<')
            {
                token.value[length_char++] = '<';
                state_atual = SMENORIGUAL;
            }else if (char_atual == '>')
            {
                token.value[length_char++] = '>';
                state_atual = SMAIORIGUAL;
            }else if (char_atual == '=')
            {
                // token.value = "=";
                strcpy(token.value, "=");
                token.type = IGUAL;
                break;
            }else if (char_atual == '+')
            {
                strcpy(token.value, "+");
                token.type = SOMA;
                break;
            }else if (char_atual == '-')
            {
                strcpy(token.value, "-");
                token.type = SUB;
                break;
            }else if (char_atual == '*')
            {
                strcpy(token.value, "*");
                token.type = MULT;
                break;
            }else if (char_atual == '\n')
            {
                strcpy(token.value, "NL");
                token.type = NL;
                break;
            }
        }else if (state_atual == SVAR)
        {
            if (((char_atual >= 'a')&&(char_atual <= 'z'))||((char_atual >= 'A')&&(char_atual <= 'Z'))||((char_atual >= '0')&&(char_atual <= '9')))
            {
                token.value[length_char++] = char_atual;
            }else
            {
                token.value[length_char] = '\0';
                token.type = VAR;
                ungetc(char_atual, arq);
                break;
            }
        }else if (state_atual == SNUM)
        {
            if ((char_atual >= '0')&&(char_atual <= '9'))
            {
                token.value[length_char++] = char_atual;
            }else
            {
                token.value[length_char] = '\0';
                token.type = NUM;
                ungetc(char_atual, arq);
                break;
            }
        }else if (state_atual == SMENORIGUAL)
        {
            if (char_atual == '=')
            {
                token.value[length_char++] = '=';
                token.value[length_char] = '\0';
                token.type = MENORIGUAL;
                break;
            }else
            {
                token.value[length_char] = '\0';
                token.type = MENOR;
                ungetc(char_atual, arq);
                break;
            }
        }else if (state_atual == SMAIORIGUAL)
        {
            if (char_atual == '=')
            {
                token.value[length_char++] = '=';
                token.value[length_char] = '\0';
                token.type = MAIORIGUAL;
                break;
            }else
            {
                token.value[length_char] = '\0';
                token.type = MAIOR;
                ungetc(char_atual, arq);
                break;
            }
        }
        if (feof(arq))
        {
            token.type = EF;
            break;
        }
    }
    return token;
}

void  consome_tokens(string path_file)
{
    arq = fopen(path_file, "rt");
    if (!arq)
    {
        printf("\n[ERRO] Falha ao tentar abrir o arquivo: %s\n\n", path_file);
        exit(1);
    }
    rewind(arq);
    objetivo();
    funcao();
    restricao();
    fclose(arq);
}

int sinal;

void objetivo()
{
    token = get_token();
    if (token.type == VAR)
        if (strcmp(token.value, "min")==0)
        {
            sinal = 1;
            return;
        }
        else if (strcmp(token.value, "max")==0)
        {
            sinal = -1;
            return;
        }
    printf("\n[ERRO] Sintaxe errada, esperava min ou max, porém foi recebido %s\n\n", token.value);
    exit(1);
}

void funcao()
{
    token = get_token();
    if (token.type == SOMA)
    {
          escalar(1);
    }else if (token.type == SUB)
    {
          escalar(-1);
    }else if (token.type == NUM)
    {
          variavel(atof(token.value));
    }else if (token.type == VAR)
    {
          // printf("1VAR: %.2lf*%s\n", (double)sinal, token.value);
          variavel_t aux;
          aux.custo = (double)sinal;
          strcmp(aux.name, token.value);
          aux.type  = ORIGINAL;
          if (num_var == 0)
              list_var = (variavel_t*) malloc(sizeof(variavel_t)*(++num_var));
          else
              list_var = (variavel_t*) realloc(list_var, sizeof(variavel_t)*(++num_var));

          list_var[num_var-1] = aux;
          proxima();
    }else{
        printf("\n[ERRO] Sintaxe errada, esperava um escalar ou uma variavel, porém foi recebido %s\n\n", token.value);
        exit(1);
    }
}

void proxima()
{
    token = get_token();
    if (token.type == SOMA)
    {
          escalar(1);
    }else if (token.type == SUB)
    {
          escalar(-1);
    }else if (token.type == MULT)
    {
          printf("\n[ERRO] Sintaxe errada, esperava um escalar ou uma variavel, porém foi recebido %s\n\n", token.value);
          exit(1);
    }else if (token.type == VAR)
    {
          printf("\n[ERRO] Sintaxe errada, esperava um escalar ou uma variavel, porém foi recebido %s\n\n", token.value);
          exit(1);
    }
}

void escalar(int sinal_var)
{
    token = get_token();
    if (token.type == NUM)
        variavel((double)(atof(token.value)*sinal_var));
    else if (token.type == VAR)
    {
        // printf("2VAR: %.2lf*%s\n", (double)(sinal*sinal_var), token.value);
        variavel_t aux;
        aux.custo = (double)(sinal*sinal_var);
        strcmp(aux.name, token.value);
        aux.type  = ORIGINAL;
        if (num_var == 0)
            list_var = (variavel_t*) malloc(sizeof(variavel_t)*(++num_var));
        else
            list_var = (variavel_t*) realloc(list_var, sizeof(variavel_t)*(++num_var));

        list_var[num_var-1] = aux;
        proxima();
    }else{
        printf("\n[ERRO] Sintaxe errada, esperava um escalar ou uma variavel, porém foi recebido %s\n\n", token.value);
        exit(1);
    }
}

void variavel(double escalar)
{
    token = get_token();
    if (token.type == MULT)
        token = get_token();

    if (token.type == VAR)
    {
        // printf("3VAR: %.2lf*%s\n", (double)(sinal*escalar), token.value);
        variavel_t aux;
        aux.custo = (double)(sinal*escalar);
        strcmp(aux.name, token.value);
        aux.type  = ORIGINAL;
        if (num_var == 0)
            list_var = (variavel_t*) malloc(sizeof(variavel_t)*(++num_var));
        else
            list_var = (variavel_t*) realloc(list_var, sizeof(variavel_t)*(++num_var));

        list_var[num_var-1] = aux;
        proxima();
    }else{
        printf("\n[ERRO] Sintaxe errada, esperava uma variavel, porém foi recebido %s\n\n", token.value);
        exit(1);
    }
}

void restricao()
{
      if (token.type != NL)
      {
          printf("\n[ERRO] Sintaxe errada, esperava conjunto de restrições, porém foi recebido %s\n\n", token.value);
          exit(1);
      }
      // printf("restrições\n");
}
