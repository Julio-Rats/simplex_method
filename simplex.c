#include "simplex.h"

custo_t*  custos     = NULL;
int       len_custos = 0;

void simplex()
{
    double**   lambda  = NULL;
    double**   Xb      = NULL;
    double**   vetor_y = NULL;
    double**   coeficientes = (double**) malloc(sizeof(double*));
            coeficientes[0] = (double*) malloc(sizeof(double)*number_base);

    matriz_Base = (double**) malloc(sizeof(double*)*number_base);

    int iter = 1;

    while(true)
    {
        printf("\t\tITERAÇÃO %d\n\n", iter++);
        printf("Variaveis Basicas \t B[");
        for (int i = 0; i < number_base-1; i++)
            printf("%s, ", var_base[i].name);

        printf("%s]\n", var_base[number_base-1].name);
        printf("Variaveis Não Basicas \t N[");
        for (int i = 0; i < number_Nbase-1; i++)
            printf("%s, ", var_Nbase[i].name);

        printf("%s]\n\n", var_Nbase[number_Nbase-1].name);

        if (lambda)
        {
            free(lambda);
            lambda = NULL;
        }

        for (int i = 0; i < number_base; i++)
        {
            coeficientes[0][i] = var_base[i].custo;
            matriz_Base[i]     = var_base[i].aj[0];
        }

        matriz_Base = transposta(matriz_Base, number_base, number_base);
        lambda = transposta(decomposicao_LU(transposta(matriz_Base, number_base, number_base), transposta(coeficientes, 1, number_base), number_base), number_base, 1);

        printf("Coeficientes Basicos:\n");
        for (int j = 0; j < number_base; j++)
            printf("%.2lf\t", coeficientes[0][j]);
        printf("\n\n");


        printf("MATRIZ BASICA:");
        for (int i = 0; i < number_base; i++)
        {
          printf("\n");
          for (int j = 0; j < number_base; j++)
              printf("%.2lf\t", matriz_Base[i][j]);
        }
        printf("\n\n");


        printf("LAMBDA:\n");
        for (int i = 0; i < number_base; i++)
            printf("%.2lf\n", lambda[0][i]);
        printf("\n");

        bool    otimo              = true;
        double  custo, menor_custo = 1;
        int     variavel_entra;

        if (custos)
        {
            free(custos);
            custos = NULL;
            len_custos = 0;
        }
        sort = 0;
        for (int i = 0; i < number_Nbase; i++)
        {
            custo = var_Nbase[i].custo - multi_matriz(lambda, transposta(var_Nbase[i].aj, 1, number_base), 1, number_base, 1)[0][0];
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

        printf("\nVetor b:\n");
        for (int i = 0; i < number_base; i++)
            printf("%.2lf\n", vetor_b[i][0]);
        printf("\n");

        printf("\nVetor Xb:\n");
        for (int i = 0; i < number_base; i++)
            printf("%.2lf\n", Xb[i][0]);
        printf("\n");

        if (otimo)
           break;

        bool   ilimitada = true;
        double passo, menor_passo;
        int    variavel_sai;
        do{
            variavel_entra = var_menor_custo();

            if (variavel_entra == -1)
            {
                printf("\n\tSolução ILIMITADA\n\n");
                exit(0);
            }

            vetor_y = decomposicao_LU(matriz_Base, transposta(var_Nbase[variavel_entra].aj, 1, number_base), number_base);

            for (int i = 0; i < number_base; i++)
                if (vetor_y[i][0] > 0)
                {
                    passo = (Xb[i][0])/(vetor_y[i][0]);
                    if (ilimitada) // primeira verificação
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
                printf("Escolhido: %s\n", var_Nbase[variavel_entra].name);
                printf("Vetor Y:\n");
                for (int i = 0; i < number_base; i++)
                printf("%.2lf\n", vetor_y[i][0]);
                printf("\n");
                if (ilimitada)
                   printf("não achou nenhum Y positivo\n\n");
        }while(ilimitada);


        printf("\nEntra = %s, Sai = %s\n", var_Nbase[variavel_entra].name, var_base[variavel_sai].name);
        printf("custo %.2lf, Variavel %s entra na base\n\n", menor_custo, var_Nbase[variavel_entra].name);
        printf("Menor passo %.2lf, Variavel %s sai da base\n", menor_passo, var_base[variavel_sai].name);


        variavel_t aux = var_Nbase[variavel_entra];
        var_Nbase[variavel_entra] = var_base[variavel_sai];
        var_base[variavel_sai] = aux;
        char a = getc(stdin);
    }

    double fx=0; // valor função objetivo

    for (int i = 0; i < number_base; i++)
        if (var_base[i].type == ARTIFICIAL)
        {
            printf("\nVariavel Artificial na base, problema infactivel\n");
            exit(0);
        }

    printf("Otimo encontrado\n");
    printf("\n\tResultado:\n\n");
    for (int i = 0; i < number_base; i++)
        if (var_base[i].type == ORIGINAL)
        {
            fx+= Xb[i][0]*var_base[i].custo;
            printf("\tVariavel %s -> %.2lf\n", var_base[i].name, Xb[i][0]);
        }

    for (int i = 0; i < number_Nbase; i++)
        if (var_Nbase[i].type == ORIGINAL)
            printf("\tVariavel %s -> 0\n", var_Nbase[i].name);

    printf("\n\tFunção objetivo: %.2lf\n\n", fx);
}

void calc_menor_custo(double valor, int var)
{
    if (len_custos == 0)
    {
        custos = (custo_t*) malloc(sizeof(custo_t));
        if (!custos)
        {
            printf("Erro Alocação de memoria\n");
            exit(4);
        }
        len_custos = 1;
        custos[0].valor    = valor;
        custos[0].variavel = var;
    }else if (custos[0].valor > valor)
    {
        len_custos = 1;
        custos = (custo_t*) realloc(custos, sizeof(custo_t));
        if (!custos)
        {
            printf("Erro Realocação de memoria\n");
            exit(4);
        }
        custos[0].valor    = valor;
        custos[0].variavel = var;
    }else if (custos[0].valor == valor)
    {
        custos = (custo_t*) realloc(custos, sizeof(custo_t)*(++len_custos));
        if (!custos)
        {
            printf("Erro Realocação de memoria\n");
            exit(4);
        }
        custos[len_custos-1].valor    = valor;
        custos[len_custos-1].variavel = var;
    }
}

int var_menor_custo()
{
    if (sort < len_custos)
        return custos[sort++].variavel;
    else
        return -1;
}
