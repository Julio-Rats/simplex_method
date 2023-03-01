#include <stdio.h>
#include <stdlib.h>

#include "simplex.h"

/*
    Global Variable
*/
extern int sinal; // input.c

/*
    Local Variables
*/
typedef struct
{
    double valor;
    size_t variavel;
} custo_t;

double** matriz_Base = NULL; //  Matriz com as j colunas de A, das j variáveis basicas.
custo_t* custos      = NULL;
size_t len_custos    = 0;
size_t sort          = 0;

/*
    Local Functions
*/
void calc_menor_custo(double valor, size_t var);
int var_menor_custo();

void simplex()
{
    double** lambda  = NULL;
    double** Xb      = NULL;
    double** vetor_y = NULL;
    double** coeficientes = (double**)malloc(sizeof(double*));
    coeficientes[0] = (double*)malloc(sizeof(double) * number_base);

    matriz_Base = (double**)malloc(sizeof(double*) * number_base);

    int iter = 1;

    while (true)
    {
        printf("\t\t%dª ITERAÇÃO\n\n", iter++);
        printf("Variáveis Basicas \t B[");
        for (size_t i = 0; i < number_base - 1; i++)
            printf("%s, ", var_base[i].name);

        printf("%s]\n", var_base[number_base - 1].name);
        printf("Variáveis não Basicas \t N[");
        for (size_t i = 0; i < number_Nbase - 1; i++)
            printf("%s, ", var_Nbase[i].name);

        printf("%s]\n\n", var_Nbase[number_Nbase - 1].name);

        if (lambda)
        {
            free(lambda);
            lambda = NULL;
        }

        for (size_t i = 0; i < number_base; i++)
        {
            coeficientes[0][i] = var_base[i].cost;
            matriz_Base[i] = var_base[i].aj[0];
        }

        matriz_Base = transposta(matriz_Base, number_base, number_base);
        lambda = transposta(decomposicao_LU(transposta(matriz_Base, number_base, number_base), transposta(coeficientes, 1, number_base), number_base), number_base, 1);

        printf("Coeficientes Basicos:\n");
        for (size_t j = 0; j < number_base; j++)
            printf("%.2lf\t", coeficientes[0][j]);
        printf("\n\n");

        printf("Matriz Basica:");
        for (size_t i = 0; i < number_base; i++)
        {
            printf("\n");
            for (size_t j = 0; j < number_base; j++)
                printf("%.2lf\t", matriz_Base[i][j]);
        }
        printf("\n\n");

        printf("Lambda:\n");
        for (size_t i = 0; i < number_base; i++)
            printf("%.2lf\n", lambda[0][i]);
        printf("\n");

        bool otimo = true;
        double custo, menor_custo = 1;
        int variavel_entra;

        if (custos)
        {
            free(custos);
            custos = NULL;
            len_custos = 0;
        }

        sort = 0;

        for (size_t i = 0; i < number_Nbase; i++)
        {
            custo = var_Nbase[i].cost - multi_matriz(lambda, transposta(var_Nbase[i].aj, 1, number_base), 1, number_base, 1)[0][0];
            printf("Custo Var: %s -> %.2lf\n", var_Nbase[i].name, custo);
            if (custo < 0)
            {
                otimo = false;
                if (custo <= menor_custo)
                {
                    menor_custo = custo;
                    calc_menor_custo(custo, i);
                }
            }
        }

        Xb = decomposicao_LU(matriz_Base, vetor_b, number_base);

        printf("\nVetor X:\n");
        for (int i = 0; i < number_base; i++)
            printf("%.2lf\n", Xb[i][0]);
        printf("\n");

        if (otimo)
            break;

        bool ilimitada = true;
        double passo, menor_passo;
        int variavel_sai;
        do
        {
            variavel_entra = var_menor_custo();

            if (variavel_entra == -1)
            {
                printf("\n\tSolução ILIMITADA\n\n");
                exit(EXIT_SUCCESS);
            }

            vetor_y = decomposicao_LU(matriz_Base, transposta(var_Nbase[variavel_entra].aj, 1, number_base), number_base);

            for (size_t i = 0; i < number_base; i++)
            {
                if (vetor_y[i][0] > 0)
                {
                    passo = (Xb[i][0]) / (vetor_y[i][0]);
                    if (ilimitada) // primeira verifica??o
                    {
                        menor_passo = passo;
                        variavel_sai = i;
                        ilimitada = false;
                        continue;
                    }
                    if (passo < menor_passo)
                    {
                        menor_passo = passo;
                        variavel_sai = i;
                    }
                }
                free(matriz_Base[i]);
            }
            printf("Vetor Y:\n");
            for (size_t i = 0; i < number_base; i++)
            {
                printf("%.2lf\n", vetor_y[i][0]);
                free(vetor_y[i]);
            }
            printf("\n");
            if (ilimitada)
                printf("não achou nenhum Y positivo\n\n");
        } while (ilimitada);

        printf("Resumo da Interação:\n\n");
        printf("Menor custo %.2lf, variável %s\n", menor_custo, var_Nbase[variavel_entra].name);
        printf("Menor passo %.2lf, variável %s\n", menor_passo, var_base[variavel_sai].name);
        printf("Entra variável %s e sai variável %s da base, nessa interação\n\n", var_Nbase[variavel_entra].name, var_base[variavel_sai].name);

        variavel_t aux = var_Nbase[variavel_entra];
        var_Nbase[variavel_entra] = var_base[variavel_sai];
        var_base[variavel_sai] = aux;
        // char a = getc(stdin);
    }

    double fx = 0.f; // valor função objetivo

    for (size_t i = 0; i < number_base; i++)
        if (var_base[i].type == ARTIFICIAL)
        {
            printf("\nVariável Artificial no fim da otimização, problema INFACTIVEL!!\n\n");
            exit(EXIT_FAILURE);
        }

    printf("Otimo encontrado !  \n");
    printf("\n\tResultado:\n\n");
    for (size_t i = 0; i < number_base; i++)
        if (var_base[i].type == ORIGINAL)
        {
            fx += Xb[i][0] * var_base[i].cost;
            printf("\tVariável %s -> %.2lf\n", var_base[i].name, Xb[i][0]);
        }

    for (size_t i = 0; i < number_Nbase; i++)
        if (var_Nbase[i].type == ORIGINAL)
            printf("\tVariável %s -> 0\n", var_Nbase[i].name);

    printf("\n\tFunção objetivo: %.2lf\n\n", sinal*fx);
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
