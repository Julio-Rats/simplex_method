#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>

#include "simplex.h"

/*  Global Variable  */
extern char sinal; // input.c

/*  Local Variables  */
typedef struct
{
    double valor;
    size_t variavel;
} custo_t;  // Tuple (value, index)

custo_t* custos      = NULL;
size_t len_custos    = 0;
size_t sort          = 0;

/*  Local Functions  */
void calc_menor_custo(double valor, size_t var);
int var_menor_custo();
matriz_t init_matriz(size_t m, size_t n);

void simplex()
{
    matriz_t matriz_Base    = (matriz_t)malloc(sizeof(vetor_t) * number_base); // (B) Matriz com as j colunas de A, das j variáveis básicas.
    matriz_t matriz_Base_tr = init_matriz(number_base, number_base);           // (B^t) Matriz Básica transposta.

    matriz_t lambda         = init_matriz(number_base, 1); // cb^t*B^-1  
    matriz_t lambda_tr      = init_matriz(1, number_base); // lambda^t  
    matriz_t var_Nbase_tr   = init_matriz(number_base, 1); // (Aj)^t; Coluna j de A da variáveis não básicas.
    matriz_t custo_basico_A = init_matriz(1, 1); // ci^t*B^-1*Aj, j var não básica, i variável básica.

    matriz_t Xb      = init_matriz(number_base, 1); // x = B^-1*b
    matriz_t vetor_y = init_matriz(number_base, 1); // y = B^-1*Aj ; j não básicos.
    matriz_t coef    = init_matriz(number_base, 1); // c ; min c^t*x

    bool mult_sol = false;

    size_t iter = 1;
    double fx = 0.f; // valor função objetivo

    while (true)
    {
        printf("\t\t%zuª ITERAÇÃO\n\n", iter++);
        printf("Variáveis Básicas \t B[");
        for (size_t i = 0; i < number_base - 1; i++)
            printf("%s, ", var_base[i].name);

        printf("%s]\n", var_base[number_base - 1].name);
        printf("Variáveis não Básicas \t N[");
        for (size_t i = 0; i < number_Nbase - 1; i++)
            printf("%s, ", var_Nbase[i].name);

        printf("%s]\n\n", var_Nbase[number_Nbase - 1].name);

        for (size_t i = 0; i < number_base; i++)
        {
            coef[i][0] = var_base[i].cost;
            matriz_Base[i] = var_base[i].aj[0];
        }

        transposta(matriz_Base, matriz_Base_tr, number_base, number_base);
        decomposicao_LU(matriz_Base, lambda, coef, number_base);

        printf("Coeficientes Básicos:\n");
        for (size_t i = 0; i < number_base; i++)
            printf("%.4lf\t", coef[i][0]);
        printf("\n\n");

        printf("Matriz Básica:");
        for (size_t i = 0; i < number_base; i++)
        {
            printf("\n");
            for (size_t j = 0; j < number_base; j++)
                printf("%.4lf\t", matriz_Base_tr[i][j]);
        }
        printf("\n\n");

        printf("Lambda:\n");
        for (size_t i = 0; i < number_base; i++)
            printf("%.4lf\n", lambda[i][0]);
        printf("\n");

        double custo, menor_custo = INT_MAX;
        int variavel_entra;

        if (custos)
        {
            free(custos);
            custos = NULL;
            len_custos = 0;
        }

        sort = 0;

        bool otimo = true;

        for (size_t i = 0; i < number_Nbase; i++)
        {
            transposta(var_Nbase[i].aj, var_Nbase_tr, 1, number_base);
            multi_matriz((transposta(lambda, lambda_tr, number_base, 1)), var_Nbase_tr, custo_basico_A, 1, number_base, 1);
            custo = var_Nbase[i].cost - custo_basico_A[0][0];
            printf("Custo Var: %s -> %.4lf\n", var_Nbase[i].name, custo);
            if (custo < 0)
            {
                if (custo <= menor_custo)
                {
                    menor_custo = custo;
                    calc_menor_custo(custo, i);
                }
                if (otimo)
                    otimo = false;
            }else if (!mult_sol && custo == 0)
                mult_sol = true;
        }
    
        decomposicao_LU(matriz_Base_tr, Xb, vetor_b, number_base); // vetor_b -> input.h

        printf("\nVetor X:\n");
        for (size_t i = 0; i < number_base; i++)
            printf("%.4lf\n", Xb[i][0]);
        printf("\n\n");

        if (otimo)
            break;

        bool ilimitada = true;
        double passo, menor_passo;
        long int variavel_sai;
        do
        {
            if ((variavel_entra = var_menor_custo()) == -1)
            {
                printf("\n\tSolução ILIMITADA\n\n");
                exit(EXIT_SUCCESS);
            }

            decomposicao_LU(matriz_Base_tr, vetor_y, transposta(var_Nbase[variavel_entra].aj, var_Nbase_tr, 1, number_base), number_base);

            for (size_t i = 0; i < number_base; i++)
            {
                if (vetor_y[i][0] <= 1e-6)
                    continue;
                
                passo = (Xb[i][0]) / (vetor_y[i][0]);
                if (passo < menor_passo || ilimitada) // primeira verificação
                {
                    menor_passo  = passo;
                    variavel_sai = i;
                    ilimitada = false;
                }
            }
            printf("Vetor Y:\n");
            for (size_t i = 0; i < number_base; i++)
                printf("%.4lf\n", vetor_y[i][0]);
            printf("\n");
            if (ilimitada)
                printf("não achou nenhum Y positivo\n\n");
        } while (ilimitada);

        printf("Resumo da Interação:\n\n");
        printf("Menor custo %.4lf, variável %s\n", menor_custo, var_Nbase[variavel_entra].name);
        printf("Menor passo %.4lf, variável %s\n", menor_passo, var_base[variavel_sai].name);
        printf("Nessa iteração entra variável %s e sai variável %s da base\n\n", var_Nbase[variavel_entra].name, var_base[variavel_sai].name);

        variavel_t aux = var_Nbase[variavel_entra];
        var_Nbase[variavel_entra] = var_base[variavel_sai];
        var_base[variavel_sai] = aux;
        // char a = getc(stdin);
    }

    for (size_t i = 0; i < number_base; i++)
        if (var_base[i].type == ARTIFICIAL && Xb[i][0])
        {
            printf("\nVariável Artificial (não nula) no fim da otimização, problema INFACTÍVEL !!!\n\n");
            exit(EXIT_FAILURE);
        }


    if (mult_sol)
        printf("MÚLTIPLAS SOLUÇÕES POSSÍVEIS ! (Uma delas apresentada)\n");
    else
        printf("Ótimo encontrado !\n");
    printf("\n\tSolução:\n\n");
    for (size_t i = 0; i < number_base; i++)
        if (var_base[i].type == ORIGINAL)
        {
            fx += Xb[i][0] * var_base[i].cost;
            printf("\tVariável %s -> %.4lf\n", var_base[i].name, Xb[i][0]);
        }
    for (size_t i = 0; i < number_Nbase; i++)
        if (var_Nbase[i].type == ORIGINAL)
            printf("\tVariável %s -> 0\n", var_Nbase[i].name);

    printf("\n\tFunção objetivo: %.4lf\n\n", sinal*fx);
}

void calc_menor_custo(double valor, size_t var)
{
    if (len_custos == 0)
    {
        custos = (custo_t *)malloc(sizeof(custo_t));
        if (!custos)
        {
            printf("Erro Alocação de memoria\n");
            exit(EXIT_FAILURE);
        }
        len_custos = 1;
        custos[0].valor = valor;
        custos[0].variavel = var;
    }
    else if (custos[0].valor > valor)
    {
        len_custos = 1;
        custos = (custo_t *)realloc(custos, sizeof(custo_t));
        if (!custos)
        {
            printf("Erro Realocação de memoria\n");
            exit(EXIT_FAILURE);
        }
        custos[0].valor = valor;
        custos[0].variavel = var;
    }
    else if (custos[0].valor == valor)
    {
        custos = (custo_t *)realloc(custos, sizeof(custo_t) * (++len_custos));
        if (!custos)
        {
            printf("Erro Realocação de memoria\n");
            exit(EXIT_FAILURE);
        }
        custos[len_custos - 1].valor = valor;
        custos[len_custos - 1].variavel = var;
    }
}

int var_menor_custo()
{
    if (sort < len_custos)
        return custos[sort++].variavel;
    else
        return -1;
}

matriz_t init_matriz(size_t m, size_t n)
{   
    matriz_t A = (matriz_t)malloc(sizeof(vetor_t)*m);
    if (!A)
    {
        printf("[ERRO] Falha de alocação de memoria para matriz, init_matriz()\n\n");
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < m; i++)
        if (!(A[i] = (vetor_t)malloc(sizeof(double)*n)))
        {
            printf("[ERRO] Falha de alocação de memoria para matriz, init_matriz()\n\n");
            exit(EXIT_FAILURE);
        }

    return A;
}
