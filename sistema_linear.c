#include <stdio.h>
#include <string.h>
#include <math.h>

#include "sistema_linear.h"

matrix_t init_matriz(size_t m, size_t n)
{
    matrix_t A = (matrix_t)malloc(sizeof(vector_t) * m);
    if (!A)
    {
        printf("\n[ERROR] Memory allocation failure for matrix, init_matriz()\n\n");
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < m; i++)
        if (!(A[i] = (vector_t)calloc(n, sizeof(double))))
        {
            printf("\n[ERROR] Memory allocation failure for matrix, init_matriz()\n\n");
            exit(EXIT_FAILURE);
        }

    return A;
}

matrix_t solver_LU(matrix_t LU, size_t *vpermut, matrix_t x, matrix_t b, size_t n)
{
    /*
        Ax = b, Solve the system using LU decomposition
        LUx = b (Lines of b adjusted with those permuted during construction of the LU, vector vpermut)

        Note: Using x and b as matrices for the convenience of operations between matrices, without the need for operations
            between matrices and vectors, a vector of i elements is a matrix with index [i][0] (column vector)
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

matrix_t decomposicao_LU(matrix_t A, matrix_t LU, size_t *vpermut, size_t n)
{
    /*
       Decompose A into LU.

        vpermut vector of indices at which rows were permuted during decomposition.
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
            printf("[ERROR] Constraint matrix A is singular, system not possible!\n\n");
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

matrix_t multi_matriz(matrix_t A, matrix_t B, matrix_t dest, size_t m, size_t n, size_t r)
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

matrix_t multi_escalar(matrix_t src, double escalar, matrix_t dest, size_t m, size_t n)
{
    for (size_t i = 0; i < m; i++)
        for (size_t j = 0; j < n; j++)
            dest[i][j] = escalar * src[i][j];

    return dest;
}

matrix_t transposta(matrix_t src, matrix_t dest, size_t m, size_t n)
{
    for (size_t i = 0; i < m; i++)
        for (size_t j = 0; j < n; j++)
            dest[j][i] = src[i][j];

    return dest;
}
