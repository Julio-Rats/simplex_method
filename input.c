#include "input.h"

variavel_t*  list_var     = NULL;  // Vetor auxiliar para ler restrições
int          num_var      = 0;     // Length do vetor list_var.

u_int16_t    number_base  = 0;      // Dimensão da matriz basica (B).
u_int16_t    number_Nbase = 0;      // Dimensão da matriz não basica (N).
double**     vetor_b      = NULL;   // Vetor B.
variavel_t*  var_base     = NULL;   // Variveis basicas.
variavel_t*  var_Nbase    = NULL;   // Variveis não basicas.

string random_var(string prefixo, int index)
{
    string var = (string) malloc(sizeof(char)*(strlen(prefixo)+6));
    sprintf(var, "%s%d", prefixo, index);
    return var;
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
            }else if (char_atual == '.')
            {
                token.value[length_char++] = char_atual;
                state_atual = DNUM_O;
            }else
            {
                token.value[length_char] = '\0';
                token.type = NUM;
                ungetc(char_atual, arq);
                break;
            }
        }else if (state_atual == DNUM_O)
        {
            if ((char_atual >= '0')&&(char_atual <= '9'))
            {
                token.value[length_char++] = char_atual;
                state_atual = DNUM;
            }else
            {
                token.value[length_char++] = char_atual;
                token.value[length_char] = '\0';
                token.type = ERROR;
                break;
            }
        }else if (state_atual == DNUM)
        {
            if ((char_atual >= '0')&&(char_atual <= '9'))
            {
                token.value[length_char++] = char_atual;
            }else if (char_atual == '.')
            {
                token.value[length_char++] = char_atual;
                token.value[length_char] = '\0';
                token.type = ERROR;
                break;
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
                token.type = ERROR;
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
                token.type = ERROR;
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

void  input_file(string path_file)
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
    for (int i = 0; i < num_var; i++)
        if (list_var[i].aj == NULL)
        {
            list_var[i].aj    = (double**) malloc(sizeof(double*));
            list_var[i].aj[0] = (double*)  calloc(number_base, sizeof(double));
        }
    uniao_var();
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
          escalar(1);
    else if (token.type == SUB)
          escalar(-1);
    else if (token.type == NUM)
          variavel(atof(token.value));
    else if (token.type == VAR)
    {
          variavel_t aux;
          aux.custo = (double)sinal;
          aux.name  = (string) malloc(sizeof(char)*(strlen(token.value)+1));
          aux.aj    = NULL;
          strcpy(aux.name, token.value);
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
          escalar(1);
    else if (token.type == SUB)
          escalar(-1);
    else if ((token.type == MULT)||(token.type == VAR))
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
        variavel_t aux;
        aux.custo = (double)(sinal*sinal_var);
        aux.name  = (string) malloc(sizeof(char)*(strlen(token.value)+1));
        aux.aj    = NULL;
        strcpy(aux.name, token.value);
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
        variavel_t aux;
        aux.custo = (double)(sinal*escalar);
        aux.name  = (string) malloc(sizeof(char)*(strlen(token.value)+1));
        aux.aj    = NULL;
        strcpy(aux.name, token.value);
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
    line();
}

void line()
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
            for (int i = 0; i < num_var; i++)
                if (strcmp(list_var[i].name, token.value)==0)
                {
                    exist = true;
                    if (list_var[i].aj == NULL)
                    {
                        list_var[i].aj    = (double**) malloc(sizeof(double*));
                        list_var[i].aj[0] = (double*)  calloc((number_base+1), sizeof(double));
                    }else{
                        list_var[i].aj[0] = (double*) realloc(list_var[i].aj[0], sizeof(double)*(number_base+1));
                    }
                    list_var[i].aj[0][number_base] = 1;
                    break;
                }
            if (!exist)
            {
                if (num_var == 0)
                    list_var = (variavel_t*) malloc(sizeof(variavel_t)*(++num_var));
                else
                    list_var = (variavel_t*) realloc(list_var, sizeof(variavel_t)*(++num_var));

                double** vetor_aux = (double**) malloc(sizeof(double*));
                vetor_aux[0] = (double*) calloc(number_base+1, sizeof(double));
                vetor_aux[0][number_base] = 1;

                variavel_t aux = {0, NULL, ORIGINAL, vetor_aux};
                aux.name  = (string) malloc(sizeof(char)*(strlen(token.value)+1));
                strcpy(aux.name, token.value);

                list_var[num_var-1] = aux;
            }
            proxima_restrict();
        }
        else if (token.type == NL)
            line();
}

void proxima_restrict()
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
          printf("\n[ERRO] Sintaxe errada, esperava um escalar ou uma variavel, porém foi recebido %s\n\n", token.value);
          exit(1);
    }else if (token.type == VAR)
    {
          printf("\n[ERRO] Sintaxe errada, esperava um escalar ou uma variavel, porém foi recebido %s\n\n", token.value);
          exit(1);
    }
}

void escalar_restrict(int sinal_var)
{
    token = get_token();
    if (token.type == NUM)
        variavel_restrict((double)(atof(token.value)*sinal_var));
    else if (token.type == VAR)
    {
        bool exist = false;
        for (int i = 0; i < num_var; i++)
            if (strcmp(list_var[i].name, token.value)==0)
            {
                exist = true;
                if (list_var[i].aj == NULL)
                {
                    list_var[i].aj    = (double**) malloc(sizeof(double*));
                    list_var[i].aj[0] = (double*)  calloc((number_base+1), sizeof(double));
                }else{
                    list_var[i].aj[0] = (double*) realloc(list_var[i].aj[0], sizeof(double)*(number_base+1));
                }
                list_var[i].aj[0][number_base] = (double)sinal_var;
                break;
            }
        if (!exist)
        {
            if (num_var == 0)
                list_var = (variavel_t*) malloc(sizeof(variavel_t)*(++num_var));
            else
                list_var = (variavel_t*) realloc(list_var, sizeof(variavel_t)*(++num_var));

            double** vetor_aux = (double**) malloc(sizeof(double*));
            vetor_aux[0] = (double*) calloc(number_base+1, sizeof(double));
            vetor_aux[0][number_base] = (double)sinal_var;

            variavel_t aux = {0, NULL, ORIGINAL, vetor_aux};
            aux.name  = (string) malloc(sizeof(char)*(strlen(token.value)+1));
            strcpy(aux.name, token.value);
            list_var[num_var-1] = aux;
        }
        proxima_restrict();
    }else{
        printf("\n[ERRO] Sintaxe errada, esperava um escalar ou uma variavel, porém foi recebido %s\n\n", token.value);
        exit(1);
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
        for (int i = 0; i < num_var; i++)
            if (strcmp(list_var[i].name, token.value)==0)
            {
                exist = true;
                if (list_var[i].aj == NULL)
                {
                    list_var[i].aj    = (double**) malloc(sizeof(double*));
                    list_var[i].aj[0] = (double*)  calloc((number_base+1), sizeof(double));
                }else{
                    list_var[i].aj[0] = (double*) realloc(list_var[i].aj[0], sizeof(double)*(number_base+1));
                }
                list_var[i].aj[0][number_base] = escalar;
                break;
            }
        if (!exist)
        {
            if (num_var == 0)
                list_var = (variavel_t*) malloc(sizeof(variavel_t)*(++num_var));
            else
                list_var = (variavel_t*) realloc(list_var, sizeof(variavel_t)*(++num_var));

            double** vetor_aux = (double**) malloc(sizeof(double*));
            vetor_aux[0] = (double*) calloc(number_base+1, sizeof(double));
            vetor_aux[0][number_base] = escalar;

            variavel_t aux = {0, NULL, ORIGINAL, vetor_aux};
            aux.name  = (string) malloc(sizeof(char)*(strlen(token.value)+1));
            strcpy(aux.name, token.value);

            list_var[num_var-1] = aux;
        }
        proxima_restrict();
    }else{
        printf("\n[ERRO] Sintaxe errada, esperava uma variavel, porém foi recebido %s\n\n", token.value);
        exit(1);
    }
}

void menor_igual()
{
    token = get_token();
    if (token.type == NUM)
    {
        for (int i = 0; i < number_base; i++)
        {
            var_base[i].aj[0] = (double*) realloc(var_base[i].aj[0], sizeof(double)*(number_base+1));
            var_base[i].aj[0][number_base] = 0;
        }

        if (number_base == 0)
        {
            var_base       = (variavel_t*) malloc(sizeof(variavel_t)*(++number_base));
            vetor_b        = (double**)    malloc(sizeof(double*)*(number_base));
            vetor_b[0]     = (double*)     malloc(sizeof(double));
        }else{
            var_base       = (variavel_t*) realloc(var_base, sizeof(variavel_t)*(++number_base));
            vetor_b        = (double**)    realloc(vetor_b, sizeof(double*)*(number_base));
            vetor_b[number_base-1]  = (double*)  malloc(sizeof(double));
        }
        vetor_b[number_base-1][0] = (double)atof(token.value);

        double** vetor_aux = (double**) malloc(sizeof(double*));
        vetor_aux[0] = (double*) calloc(number_base, sizeof(double));
        vetor_aux[0][number_base-1] = 1;

        static u_int16_t number_folga = 1;
        variavel_t aux = {0, random_var("folga", number_folga++), FOLGA, vetor_aux};
        var_base[number_base-1]  = aux;
    }
    token = get_token();
    if (token.type != NL)
    {
        printf("\n[ERRO] Sintaxe errada, esperava uma nova linha de restrição, porém foi recebido %s\n\n", token.value);
        exit(1);
    }
    line();
}

void maior_igual()
{
    token = get_token();
    if (token.type == NUM)
    {
        for (int i = 0; i < number_base; i++)
        {
            var_base[i].aj[0] = (double*) realloc(var_base[i].aj[0], sizeof(double)*(number_base+1));
            var_base[i].aj[0][number_base] = 0;
        }

        for (int i = 0; i < number_Nbase; i++)
        {
            var_Nbase[i].aj[0] = (double*) realloc(var_Nbase[i].aj[0], sizeof(double)*(number_base+1));
            var_Nbase[i].aj[0][number_base] = 0;
        }

        if (number_base == 0)
        {
            var_base       = (variavel_t*) malloc(sizeof(variavel_t)*(++number_base));
            vetor_b        = (double**)    malloc(sizeof(double*)*(number_base));
            vetor_b[0]     = (double*)     malloc(sizeof(double));
        }else{
            var_base       = (variavel_t*) realloc(var_base, sizeof(variavel_t)*(++number_base));
            vetor_b        = (double**)    realloc(vetor_b, sizeof(double*)*(number_base));
            vetor_b[number_base-1]  = (double*)  malloc(sizeof(double));
        }
        vetor_b[number_base-1][0] = (double)atof(token.value);

        double** vetor_aux = (double**) malloc(sizeof(double*));
        vetor_aux[0] = (double*) calloc(number_base, sizeof(double));
        vetor_aux[0][number_base-1] = 1;

        static u_int16_t number_artif = 1;
        variavel_t aux = {(double)(sinal*BIGM), random_var("artif", number_artif++), ARTIFICIAL, vetor_aux};
        var_base[number_base-1]   = aux;

        if (number_Nbase == 0)
            var_Nbase = (variavel_t*) malloc(sizeof(variavel_t)*(++number_Nbase));
        else
            var_Nbase = (variavel_t*) realloc(var_Nbase, sizeof(variavel_t)*(++number_Nbase));

        static u_int16_t number_artifN = 1;
        vetor_aux = (double**) malloc(sizeof(double*));
        vetor_aux[0] = (double*) calloc(number_base, sizeof(double));
        vetor_aux[0][number_base-1] = -1;

        variavel_t aux1 = {0, random_var("folgaN", number_artifN++), FOLGA, vetor_aux};
        var_Nbase[number_Nbase-1]   = aux1;

    }
    token = get_token();
    if (token.type != NL)
    {
        printf("\n[ERRO] Sintaxe errada, esperava uma nova linha de restrição, porém foi recebido %s\n\n", token.value);
        exit(1);
    }
    line();
}

void igual()
{
      token = get_token();
      if (token.type == NUM)
      {
          for (int i = 0; i < number_base; i++)
          {
              var_base[i].aj[0] = (double*) realloc(var_base[i].aj[0], sizeof(double)*(number_base+1));
              var_base[i].aj[0][number_base] = 0;
          }

          for (int i = 0; i < number_Nbase; i++)
          {
              var_Nbase[i].aj[0] = (double*) realloc(var_Nbase[i].aj[0], sizeof(double)*(number_base+1));
              var_Nbase[i].aj[0][number_base] = 0;
          }

          if (number_base == 0)
          {
              var_base       = (variavel_t*) malloc(sizeof(variavel_t)*(++number_base));
              vetor_b        = (double**)    malloc(sizeof(double*)*(number_base));
              vetor_b[0]     = (double*)     malloc(sizeof(double));
          }else{
              var_base       = (variavel_t*) realloc(var_base, sizeof(variavel_t)*(++number_base));
              vetor_b        = (double**)    realloc(vetor_b, sizeof(double*)*(number_base));
              vetor_b[number_base-1]  = (double*)  malloc(sizeof(double));
          }
          vetor_b[number_base-1][0] = (double)atof(token.value);

          double** vetor_aux = (double**) malloc(sizeof(double*));
          vetor_aux[0] = (double*) calloc(number_base, sizeof(double));
          vetor_aux[0][number_base-1] = 1;

          static u_int16_t number_artif = 1;
          variavel_t aux = {(double)(sinal*BIGM), random_var("artif", number_artif++), ARTIFICIAL, vetor_aux};
          var_base[number_base-1]   = aux;

      }
      token = get_token();
      if (token.type != NL)
      {
          printf("\n[ERRO] Sintaxe errada, esperava uma nova linha de restrição, porém foi recebido %s\n\n", token.value);
          exit(1);
      }
      line();
}

void uniao_var()
{
    var_Nbase = (variavel_t*) realloc(var_Nbase, sizeof(variavel_t)*(num_var+number_Nbase));
    int i,j;
    for (i = number_Nbase, j = 0; i < (num_var+number_Nbase); j++, i++)
        var_Nbase[i] = list_var[j];

    number_Nbase += num_var;
    free(list_var);
    num_var = 0;
}
