#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "sistema_linear.h"

/* 
    Local Function
*/
double** multi_escalar(double** matriz, double escalar, size_t m, size_t n);

double** decomposicao_LU(double** a, double** b, size_t n)
{
    size_t vpermut[n];
    
    double** lu = (double**) malloc(n * sizeof(double*));
    if (!lu)
    {
        printf("Erro alocação matriz\n");
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < n; i++)
    {
        lu[i] = (double*) calloc(n, sizeof(double));
        if (!lu[i])
        {
            printf("Erro alocacão matriz\n");
            exit(EXIT_FAILURE);
        }
        memcpy(lu[i], a[i], n * sizeof(double));
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
            printf("[ERRO]: Matriz singular, sistema não possivel !\n");
            exit(EXIT_FAILURE);
        }

        if (l_pivo != k)
        {
            size_t aux      = vpermut[k];
            vpermut[k]      = vpermut[l_pivo];
            vpermut[l_pivo] = aux;

            for (int j = 0; j < n; j++)
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

    for (int i = 0; i < n; i++)
    {
        double soma = 0;
        for (int j = 0; j <= (i-1); j++)
            soma += (lu[i][j]*y[j]);

        y[i] = b_permut[i] - soma;
    }

    double** x = (double**) malloc(n*sizeof(double*));
    if (!x)
    {
        printf("erro malloc\n");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < n ; i++)
    {
        x[i] = (double*) calloc(1, sizeof(double));
        if (!x[i])
        {
            printf("erro malloc\n");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = (n-1); i >= 0; i--)
    {
        double soma = 0;
        for (size_t j = (i+1); j < n; j++)
            soma += (lu[i][j]*x[j][0]);

        x[i][0] = (y[i]-soma)/lu[i][i];
    }

    for (size_t i = 0; i < n ; i++)
        free(lu[i]);
    free(lu);

    return x;
}

double** multi_matriz(double** a, double** b, size_t m, size_t n, size_t r)
{
      double** c = (double**) calloc(m,sizeof(double*));
      if (!c)
      {
          printf("Erro alocação matriz\n");
          exit(EXIT_FAILURE);
      }
      for (size_t i = 0; i < m; i++)
      {
          c[i] = (double*) calloc(r,sizeof(double));
          if (!c[i])
          {
              printf("Erro alocação matriz\n");
              exit(EXIT_FAILURE);
          }
      }

      for (size_t i = 0; i < m; i++)
          for (size_t k = 0; k < r; k++)
              for (size_t j = 0; j < n; j++)
                  c[i][k] += a[i][j]*(b[j][k]);

      return c;
}

double** multi_escalar(double** matriz, double escalar, size_t m, size_t n)
{
      double** c = (double**) calloc(m,sizeof(double*));
      if (!c)
      {
          printf("Erro alocacão matriz\n");
          exit(EXIT_FAILURE);
      }
      for (size_t i = 0; i < m; i++)
      {
          c[i] = (double*) calloc(n,sizeof(double));
          if (!c[i])
          {
              printf("Erro alocacão matriz\n");
              exit(EXIT_FAILURE);
          }
      }

      for (size_t i = 0; i < m; i++)
          for (size_t j = 0; j < n; j++)
              c[i][j] = escalar*matriz[i][j];

      return c;
}

double** transposta(double** matriz, size_t m, size_t n)
{
      double** transposta = (double**) calloc(n,sizeof(double*));
      if (!transposta)
      {
          printf("Erro alocacão matriz\n");
          exit(EXIT_FAILURE);
      }
      for (size_t i = 0; i < n; i++)
      {
          transposta[i] = (double*) calloc(m,sizeof(double));
          if (!transposta[i])
          {
              printf("Erro alocacão matriz\n");
              exit(EXIT_FAILURE);
          }
      }

      for (size_t i = 0; i < m; i++)
          for (size_t j = 0; j < n; j++)
              transposta[j][i] = matriz[i][j];

      return transposta;
}
