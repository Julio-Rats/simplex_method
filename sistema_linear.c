#include <stdio.h>
#include <string.h>
#include <math.h>

#include "sistema_linear.h"

matriz_t solver_LU(matriz_t LU, size_t *vpermut, matriz_t x, matriz_t b, size_t n)
{
    /*
        Ax = b, Resolve o sistema usando decomposição LU
        LUx = b (Linhas de b ajustadas com as permutadas durante construção da LU, vetor vpermut)

        obs: Usando x e b como matriz por conveniência de operação entre matrizes, sem necessidade de operação
            entre matrizes e vetores, um vetor de i elementos é uma matriz de índice [k][0] (vetor coluna)
    */
    double b_permut[n];

    for (size_t i = 0; i < n; i++)
        b_permut[i] = b[vpermut[i]][0];

    double y[n];

    for (size_t i = 0; i < n; i++)
    {
        double soma = 0;
        for (int j = 0; j <= (int)i - 1; j++)
            soma += LU[i][j] * y[j];

        y[i] = b_permut[i] - soma;
    }

    for (size_t i = 0; i < n; i++)
        x[i][0] = 0;

    for (int i = (n - 1); i >= 0; i--)
    {
        double soma = 0;
        for (size_t j = (i + 1); j < n; j++)
            soma += LU[i][j] * x[j][0];

        x[i][0] = (y[i] - soma) / LU[i][i];
    }

    return x;
}

matriz_t decomposicao_LU(matriz_t A, matriz_t LU, size_t *vpermut, size_t n)
{
    /*
        Decompõem A em LU.

        vpermut vetor de indices nas quais linhas foram permutadas durante a decomposição.
    */
    for (size_t i = 0; i < n; i++)
    {
        memcpy(LU[i], A[i], sizeof(double) * n);
        vpermut[i] = i;
    }

    for (size_t k = 0; k < (n - 1); k++)
    {
        double pivo = LU[k][k];
        size_t l_pivo = k;
        for (size_t i = (k + 1); i < n; i++)
            if (fabs(LU[i][k]) > fabs(pivo))
            {
                pivo = LU[i][k];
                l_pivo = i;
            }

        if (fabs(pivo) < 1e-6)
        {
            printf("[ERRO] Matriz de restrições A é singular, sistema não possível !\n");
            exit(EXIT_FAILURE);
        }

        if (l_pivo != k)
        {
            size_t aux = vpermut[k];
            vpermut[k] = vpermut[l_pivo];
            vpermut[l_pivo] = aux;

            for (size_t j = 0; j < n; j++)
            {
                double aux = LU[k][j];
                LU[k][j] = LU[l_pivo][j];
                LU[l_pivo][j] = aux;
            }
        }

        for (size_t i = (k + 1); i < n; i++)
        {
            double escalar = LU[i][k] / LU[k][k];
            LU[i][k] = escalar;
            for (size_t j = (k + 1); j < n; j++)
                LU[i][j] -= (escalar * LU[k][j]);
        }
    }

    return LU;
}

matriz_t multi_matriz(matriz_t A, matriz_t B, matriz_t dest, size_t m, size_t n, size_t r)
{
    for (size_t i = 0; i < m; i++)
        for (size_t k = 0; k < r; k++)
        {
            dest[i][k] = 0;
            for (size_t j = 0; j < n; j++)
                dest[i][k] += A[i][j] * B[j][k];
        }

    return dest;
}

matriz_t multi_escalar(matriz_t src, double escalar, matriz_t dest, size_t m, size_t n)
{
    for (size_t i = 0; i < m; i++)
        for (size_t j = 0; j < n; j++)
            dest[i][j] = escalar * src[i][j];

    return dest;
}

matriz_t transposta(matriz_t src, matriz_t dest, size_t m, size_t n)
{
    for (size_t i = 0; i < m; i++)
        for (size_t j = 0; j < n; j++)
            dest[j][i] = src[i][j];

    return dest;
}
