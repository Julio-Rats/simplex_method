#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "sistema_linear.h"


matriz_t decomposicao_LU(matriz_t A, matriz_t x, matriz_t b, size_t n)
{
/*
    Ax = b, solver using LU decomposition  

    obs: Using x and b as matrix, convenience in creating transposes of the vector.
*/

    size_t vpermut[n];

    double** lu = (double**) malloc(n * sizeof(double*));
    if (!lu)
    {
        printf("[ERRO] Falha alocação de memoria para matriz lu, função decomposicao_LU()\n\n");
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < n; i++)
    {
        lu[i] = (double*) calloc(n, sizeof(double));
        if (!lu[i])
        {
            printf("[ERRO] Falha alocação de memoria para matriz lu[i], função decomposicao_LU()\n\n");
            exit(EXIT_FAILURE);
        }
        memcpy(lu[i], A[i], n * sizeof(double));
        vpermut[i] = i;
    }


    for (size_t k = 0; k < (n-1); k++)
    {
        double pivo = lu[k][k];
        size_t l_pivo = k;
        for (size_t i = (k+1); i < n; i++)
            if(fabs(lu[i][k]) > fabs(pivo))
            {
                pivo = lu[i][k];
                l_pivo = i;
            }

        if (fabs(pivo) < 1e-6)
        {
            printf("[ERRO] Matriz de restrições A é singular, sistema não possível !\n");
            exit(EXIT_FAILURE);
        }

        if (l_pivo != k)
        {
            size_t aux      = vpermut[k];
            vpermut[k]      = vpermut[l_pivo];
            vpermut[l_pivo] = aux;

            for (size_t j = 0; j < n; j++)
            {
                double aux    = lu[k][j];
                lu[k][j]      = lu[l_pivo][j];
                lu[l_pivo][j] = aux;
            }
        }

        for (size_t i = (k+1); i < n; i++)
        {
            double escalar = lu[i][k]/lu[k][k];
            lu[i][k]       = escalar;
            for (size_t j  = (k+1); j < n; j++)
                lu[i][j]  -= (escalar*lu[k][j]);
        }
    }

    double b_permut[n];

    for (size_t i = 0; i < n; i++)
        b_permut[i] = b[vpermut[i]][0];

    double y[n];

    for (size_t i = 0; i < n; i++)
    {
        double soma = 0;
        for (int j = 0; j <= (int)i-1; j++)
            soma += lu[i][j]*y[j];

        y[i] = b_permut[i] - soma;
    }

    for (size_t i = 0; i < n ; i++)
        x[i][0] = 0;


    for (int i = (n-1); i >= 0; i--)
    {
        double soma = 0;
        for (size_t j = (i+1); j < n; j++)
            soma += lu[i][j]*x[j][0];

        x[i][0] = (y[i]-soma)/lu[i][i];
    }

    for (size_t i = 0; i < n ; i++)
        free(lu[i]);
    free(lu);

    return x;
}

matriz_t multi_matriz(matriz_t A, matriz_t B, matriz_t dest, size_t m, size_t n, size_t r)
{
    for (size_t i = 0; i < m; i++)
        for (size_t k = 0; k < r; k++)
        {
            dest[i][k] = 0;
            for (size_t j = 0; j < n; j++)
                dest[i][k] += A[i][j]*B[j][k];
        }

    return dest;
}

matriz_t multi_escalar(matriz_t src, double escalar, matriz_t dest, size_t m, size_t n)
{
    for (size_t i = 0; i < m; i++)
        for (size_t j = 0; j < n; j++)
            dest[i][j] = escalar*src[i][j];

    return dest;
}

matriz_t transposta(matriz_t src, matriz_t dest, size_t m, size_t n)
{
    for (size_t i = 0; i < m; i++)
        for (size_t j = 0; j < n; j++)
            dest[j][i] = src[i][j];

    return dest;
}
