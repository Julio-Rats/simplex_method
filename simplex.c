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
void lista_custo_iguais(double valor, size_t var);
long int var_menor_custo();
matriz_t init_matriz(size_t m, size_t n);

void simplex()
{
    matriz_t matriz_Base    = init_matriz(number_base, number_base);           // (B) Matriz com as j colunas de A, das j variáveis básicas.
    matriz_t matriz_Base_tr = (matriz_t)malloc(sizeof(vetor_t) * number_base); // (B^t) Matriz Básica transposta.

    matriz_t lambda         = init_matriz(number_base, 1); // cb^t*B^-1
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
            matriz_Base_tr[i] = var_base[i].aj[0];
        }

        transposta(matriz_Base_tr, matriz_Base, number_base, number_base);
        decomposicao_LU(matriz_Base_tr, lambda, coef, number_base);

        printf("Coeficientes Básicos (Cb):\n");
        for (size_t i = 0; i < number_base; i++)
            printf("%.4lf\n", coef[i][0]);

        printf("\nMatriz B = [A[j]] | Para todo j básico:");
        for (size_t i = 0; i < number_base; i++)
        {
            printf("\n");
            for (size_t j = 0; j < number_base; j++)
                printf("%.4lf\t", matriz_Base[i][j]);
        }

        printf("\n\nLambda: (B^t)^-1 * Cb\n");
        for (size_t i = 0; i < number_base; i++)
            printf("%.4lf\n", lambda[i][0]);

        double custo, menor_custo = INT_MAX;
        long int variavel_entra = -1;
        size_t variavel_sai;

        if (custos)
        {
            free(custos);
            custos = NULL;
            len_custos = 0;
        }

        decomposicao_LU(matriz_Base, Xb, vetor_b, number_base); // vetor_b -> input.h

        printf("\nVetor x = B^-1 * b :\n");
        for (size_t i = 0; i < number_base; i++)
            printf("%.4lf\n", Xb[i][0]);

        fx = 0;
        for (size_t i = 0; i < number_base; i++)
            if (var_base[i].type == ORIGINAL)
                fx += Xb[i][0] * var_base[i].cost;

        printf("\nFunção objetivo: %.4lf\n", sinal*fx);

        sort = 0;
        bool otimo = true;

        printf("\nCustos (Cn[j] - A[j]^t * lambda | j não básicos)\n");
        for (size_t i = 0; i < number_Nbase; i++)
        {
            multi_matriz(var_Nbase[i].aj, lambda, custo_basico_A, 1, number_base, 1);
            custo = var_Nbase[i].cost - custo_basico_A[0][0];
            printf("Custo Var: %s -> %.4lf\n", var_Nbase[i].name, custo);
            if (custo < 0)
            {
                if (custo <= menor_custo)
                {
                    menor_custo = custo;
                    lista_custo_iguais(custo, i);
                }
                if (otimo)
                    otimo = false;
            }else if (!mult_sol && fabs(custo) <= 1e-6)
                mult_sol = true;
        }

        if (otimo)
            break;

        bool ilimitada = true;
        double passo, menor_passo = 0; // warning safe (warning uninitialized)
        do
        {
            if ((variavel_entra = var_menor_custo()) == -1)
            {
                printf("\n\n\tSolução ILIMITADA\n\n");
                exit(EXIT_SUCCESS);
            }


            decomposicao_LU(matriz_Base, vetor_y, transposta(var_Nbase[variavel_entra].aj, var_Nbase_tr, 1, number_base), number_base);

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
            printf("\nVetor Bnj, variável %s\n", var_Nbase[variavel_entra].name);
            for (size_t i = 0; i < number_base; i++)
                printf("%.4lf\n", vetor_y[i][0]);

            if (ilimitada)
                printf("\nNão achou nenhum Y positivo\n");

        } while (ilimitada);

        printf("\nResumo da Interação:\n\n");
        printf("Menor custo %.4lf, variável %s\n", menor_custo, var_Nbase[variavel_entra].name);
        printf("Menor passo %.4lf (min = {x[i]/Aj[i]} | Para todo Aj[i] > 0), variável %s\n", menor_passo, var_base[variavel_sai].name);
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
        printf("\nMÚLTIPLAS SOLUÇÕES POSSÍVEIS ! (Uma delas apresentada)\n");
    else
        printf("\nÓtimo encontrado !\n");

    printf("\n\tSolução:\n\n");
    fx = 0;
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

void lista_custo_iguais(double valor, size_t var)
{
    if (len_custos == 0)
    {
        if (!(custos = (custo_t*)malloc(sizeof(custo_t))))
        {
            printf("[ERRO] Falha alocação de memoria para lista de custo, função lista_custo_iguais()\n\n");
            exit(EXIT_FAILURE);
        }
        len_custos = 1;
        custos[0].valor = valor;
        custos[0].variavel = var;
    }
    else if (custos[0].valor > valor) // reset size to 1
    {
        len_custos = 1;
        if (!(custos = (custo_t *)realloc(custos, sizeof(custo_t))))
        {
            printf("[ERRO] Falha alocação de memoria para lista de custo, função lista_custo_iguais()\n\n");
            exit(EXIT_FAILURE);
        }
        custos[0].valor = valor;
        custos[0].variavel = var;
    }
    else if (custos[0].valor == valor) // make list same values (min value)
    {
        if (!(custos = (custo_t *)realloc(custos, sizeof(custo_t) * (++len_custos))))
        {
            printf("[ERRO] Falha alocação de memoria para lista de custo, função lista_custo_iguais()\n\n");
            exit(EXIT_FAILURE);
        }
        custos[len_custos - 1].valor = valor;
        custos[len_custos - 1].variavel = var;
    }
}

long int var_menor_custo()
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
        if (!(A[i] = (vetor_t)calloc(n, sizeof(double))))
        {
            printf("[ERRO] Falha de alocação de memoria para matriz, init_matriz()\n\n");
            exit(EXIT_FAILURE);
        }

    return A;
}
