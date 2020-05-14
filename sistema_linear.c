#include "sistema_linear.h"

double modulo(double valor)
{
      if (valor >= 0)
          return valor;
      return (-valor);
}

double** decomposicao_LU(double** a, double** b, int n)
{
    double pivo, escalar, soma;
    int    l_pivo, aux;

    double** lu = (double**) calloc(n,sizeof(double*));
    if (!lu)
    {
        printf("Erro alocação matriz\n");
        exit(1);
    }
    for (int i = 0; i < n; i++)
    {
        lu[i] = (double*) calloc(n,sizeof(double));
        if (!lu[i])
        {
            printf("Erro alocação matriz\n");
            exit(1);
        }
    }

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            lu[i][j] = a[i][j];

    int vpermut[n];
    for (int i = 0; i < n; i++)
        vpermut[i] = i;

    for (int k = 0; k < (n-1); k++)
    {
        pivo = lu[k][k];
        l_pivo = k;
        for (int i = (k+1); i < n; i++)
            if(modulo(lu[i][k]) > modulo(pivo))
            {
                pivo = lu[i][k];
                l_pivo = i;
            }

        if (pivo == 0)
        {
            printf("[ERRO]: Matriz singular, sistema não possivel determinado\n");
            exit(7);
        }


        if (l_pivo != k)
        {
            aux             = vpermut[k];
            vpermut[k]      = vpermut[l_pivo];
            vpermut[l_pivo] = aux;

            for (int j = 0; j < n; j++)
            {
                aux = lu[k][j];
                lu[k][j] = lu[l_pivo][j];
                lu[l_pivo][j] = aux;
            }
        }

        for (int i = (k+1); i < n; i++)
        {
            escalar = lu[i][k]/lu[k][k];
            lu[i][k] = escalar;
            for (int j = (k+1); j < n; j++)
                lu[i][j] -= (escalar*lu[k][j]);
        }
    }

    double b_permut[n][1];

    for (int i = 0; i < n; i++)
    {
        aux = vpermut[i];
        b_permut[i][0] = b[aux][0];
    }

    double y[n];

    for (int i = 0; i < n; i++)
    {
        soma = 0;
        for (int j = 0; j < (n-1); j++)
            soma += (lu[i][j]*y[j]);

        y[i] = b_permut[i][0] - soma;
    }

    double** x = (double**) malloc(sizeof(double*)*n);
    if (!x)
    {
        printf("erro malloc\n");
        exit(3);
    }

    for (int i = 0; i < n ; i++)
    {
        x[i] = (double*) malloc(sizeof(double));
        if (!x[i])
        {
            printf("erro malloc\n");
            exit(3);
        }
    }

    for (int i = (n-1); i > -1; i--)
    {
        soma = 0;
        for (int j = (i+1); j < n; j++)
            soma += (lu[i][j]*x[j][0]);


        x[i][0] = (y[i]-soma)/lu[i][i];
    }

    return x;
}

double** multi_matriz(double** a, double** b, int m, int n, int r)
{
      double** c = (double**) calloc(m,sizeof(double*));
      if (!c)
      {
          printf("Erro alocação matriz\n");
          exit(1);
      }
      for (int i = 0; i < m; i++)
      {
          c[i] = (double*) calloc(r,sizeof(double));
          if (!c[i])
          {
              printf("Erro alocação matriz\n");
              exit(1);
          }
      }


      for (int i = 0; i < m; i++)
          for (int k = 0; k < r; k++)
              for (int j = 0; j < n; j++)
                  c[i][k] += a[i][j]*b[j][k];

      return c;
}


double** multi_escalar(double** matriz, double escalar, int m, int n)
{
      double** c = (double**) calloc(m,sizeof(double*));
      if (!c)
      {
          printf("Erro alocação matriz\n");
          exit(1);
      }
      for (int i = 0; i < m; i++)
      {
          c[i] = (double*) calloc(n,sizeof(double));
          if (!c[i])
          {
              printf("Erro alocação matriz\n");
              exit(1);
          }
      }

      for (int i = 0; i < m; i++)
          for (int j = 0; j < n; j++)
              c[i][j] = escalar*matriz[i][j];

      return c;
}

double**  transposta(double** matriz, int m, int n)
{
      double** transposta = (double**) calloc(n,sizeof(double*));
      if (!transposta)
      {
          printf("Erro alocação matriz\n");
          exit(1);
      }
      for (int i = 0; i < n; i++)
      {
          transposta[i] = (double*) calloc(m,sizeof(double));
          if (!transposta[i])
          {
              printf("Erro alocação matriz\n");
              exit(1);
          }
      }

      for (int i = 0; i < m; i++)
          for (int j = 0; j < n; j++)
              transposta[j][i] = matriz[i][j];

      return transposta;
}
