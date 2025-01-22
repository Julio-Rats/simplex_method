#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <float.h>
#include <math.h>

#include "sistema_linear.h" // includes together "structs.h"

/*  Global Variables  */
extern char signal;         // input.c
extern long double EPSILON; // sistema_linear.c

/*  Global Local Variables  */
bool bland = false;
bool verbose = true;
bool iterative = false;
// unsigned long steps = -1; // Max long

/*  Local Struct  */
typedef struct
{
    double value;
    size_t index;
} cost_t; // Tuple (value, index)

/*  Local Variables  */
cost_t *costs = NULL;
size_t len_costs = 0;
size_t next_cost = 0;
size_t *last_var = NULL;
size_t len_last_var = 0;

/*  Local Functions  */
void list_cost_equal(double valor, size_t var);
long var_lowest_cost();
void add_index_var(size_t index);
bool found_index_var(size_t index);

void simplex()
{
    matrix_t matrix_Base = init_matriz(number_base, number_base);               // (B) Matrix with the j columns of A, of the j basic variables.
    matrix_t matrix_Base_tr = (matrix_t)malloc(sizeof(vector_t) * number_base); // (B^t) Transposed Basic Matrix.
    matrix_t matrix_Base_lu = init_matriz(number_base, number_base);            // LU decomposition of B
    matrix_t matrix_Base_tr_lu = init_matriz(number_base, number_base);         // LU decomposition of B^t

    matrix_t lambda = init_matriz(number_base, 1);       // cb^t*B^-1
    matrix_t var_Nbase_tr = init_matriz(number_base, 1); // (Aj)^t | Column j of A of non-basic variables.
    matrix_t cost_basic = init_matriz(1, 1);             // ci^t*B^-1*Aj | j var non-basic, i basic variable.

    matrix_t vector_x = init_matriz(number_base, 1); // x = B^-1*b
    matrix_t vector_d = init_matriz(number_base, 1); // d = B^-1*Aj | j non-basic.
    matrix_t coeff = init_matriz(number_base, 1);    // c ; min c^t*x

    size_t vpermut_tr[number_base];
    size_t vpermut[number_base];

    bool mult_sol = false;

    size_t iter = 0;
    double fx = 0.f; // objective function value

    while (true)
    {
        if (verbose)
        {
            iter++;
            printf("\t\t%zu%s ITERATION\n\n", iter, iter > 3 ? "th" : iter == 3 ? "rd" : iter == 2 ? "nd" : "st");

            printf("Basic Variables \t B[");
            for (size_t i = 0; i < number_base - 1; i++)
                printf("%s, ", var_base[i].name);

            printf("%s]\n", var_base[number_base - 1].name);
            printf("Non-Basic Variables \t N[");
            for (size_t i = 0; i < number_Nbase - 1; i++)
                printf("%s, ", var_Nbase[i].name);

            printf("%s]\n\n", var_Nbase[number_Nbase - 1].name);
        }

        for (size_t i = 0; i < number_base; i++)
        {
            coeff[i][0] = var_base[i].cost;
            matrix_Base_tr[i] = var_base[i].aj[0];
        }

        transpose(matrix_Base_tr, matrix_Base, number_base, number_base);
        LU_decomposition(matrix_Base, matrix_Base_lu, vpermut, number_base);
        LU_decomposition(matrix_Base_tr, matrix_Base_tr_lu, vpermut_tr, number_base);

        solver_LU(matrix_Base_tr_lu, vpermut_tr, lambda, coeff, number_base);

        if (verbose)
        {
            printf("Basic Coefficients (Cb):\n");
            for (size_t i = 0; i < number_base; i++)
                printf("%.4lf\n", coeff[i][0]);

            printf("\nMatrix B = [A[j]] | For all basic j:");
            for (size_t i = 0; i < number_base; i++)
            {
                printf("\n");
                for (size_t j = 0; j < number_base; j++)
                    printf("%.4lf\t", matrix_Base[i][j]);
            }

            printf("\n\nVector lambda = (B^t)^-1 * Cb:\n");
            for (size_t i = 0; i < number_base; i++)
                printf("%.4lf\n", lambda[i][0]);
        }

        if (costs)
        {
            free(costs);
            costs = NULL;
            len_costs = 0;
        }

        if (last_var)
        {
            free(last_var);
            last_var = NULL;
            len_last_var = 0;
        }

        solver_LU(matrix_Base_lu, vpermut, vector_x, vector_b, number_base);

        if (verbose)
        {
            printf("\nVector x = B^-1 * b:\n");
            for (size_t i = 0; i < number_base; i++)
                printf("%.4lf\n", vector_x[i][0]);
        }

        fx = 0;
        for (size_t i = 0; i < number_base; i++)
            fx += vector_x[i][0] * var_base[i].cost;

        if (verbose)
            printf("\nObjective function value: %.4lf\n", signal * fx);

        next_cost = 0;
        double custo, menor_custo = DBL_MAX;
        bool otimo = true;

        if (verbose)
            printf("\nCosts (Cn[j] - A[j]^t * lambda | j non-basic):\n");
        for (size_t i = 0; i < number_Nbase; i++)
        {
            matrix_multi(var_Nbase[i].aj, lambda, cost_basic, 1, number_base, 1);
            custo = var_Nbase[i].cost - cost_basic[0][0];
            if (verbose)
                printf("Var Cost: %s -> %.4lf\n", var_Nbase[i].name, custo);
            if (custo < 0)
            {
                if (otimo)
                    otimo = false;

                menor_custo = custo;
                list_cost_equal(custo, i);
            }
            else if (!mult_sol && fabsl((long double)custo) <= EPSILON)
                mult_sol = true;
        }

        if (otimo)
            break;

        bool ilimitada = true;
        long variavel_entra;
        size_t variavel_sai = 0;
        double passo, menor_passo = DBL_MAX; // safe warning (warning uninitialized)
        do
        {
            if ((variavel_entra = var_lowest_cost()) == -1)
            {
                printf("\n\n\tUNBOUNDED Solution.\n\n");
                exit(EXIT_SUCCESS);
            }

            transpose(var_Nbase[(unsigned long)variavel_entra].aj, var_Nbase_tr, 1, number_base);
            solver_LU(matrix_Base_lu, vpermut, vector_d, var_Nbase_tr, number_base);

            for (size_t i = 0; i < number_base; i++)
            {
                if (vector_d[i][0] <= EPSILON)
                    continue;

                passo = (vector_x[i][0]) / (vector_d[i][0]);
                if (passo < menor_passo)
                    menor_passo = passo;
                else if ((passo > menor_passo) || (bland && var_base[variavel_sai].index > var_base[i].index))
                    continue;
                variavel_sai = i;
                ilimitada = false;
            }
            if (verbose)
            {
                printf("\nVector d = B^-1 * A[j], j index of variable '%s'%s\n", var_Nbase[(unsigned long)variavel_entra].name,
                       bland ? " (Enable Bland's pivot rule):" : ":");
                for (size_t i = 0; i < number_base; i++)
                    printf("%.4lf\n", vector_d[i][0]);
            }

            if (ilimitada)
                printf("\nDidn't find any positive d for var '%s'\n", var_Nbase[(unsigned long)variavel_entra].name);

        } while (ilimitada);

        if (verbose)
        {
            printf("\nInteraction Summary:\n\n");
            printf("Lower cost %.4lf, variable '%s'%s\n", menor_custo, var_Nbase[variavel_entra].name,
                   bland ? " (Enable Bland's pivot rule)" : "");
            printf("Smallest step %.4lf (min = {x[i]/d[i]} | For all d[i] > 0), variable '%s'%s\n", menor_passo, var_base[variavel_sai].name,
                   bland ? " (Enable Bland's pivot rule)" : "");
            printf("In this iteration variable '%s' enters and variable '%s' leaves the base\n\n", var_Nbase[variavel_entra].name, var_base[variavel_sai].name);
        }

        variavel_t aux = var_Nbase[variavel_entra];
        var_Nbase[variavel_entra] = var_base[variavel_sai];
        var_base[variavel_sai] = aux;

        if (iterative)
            getc(stdin);
    }

    for (size_t i = 0; i < number_base; i++)
        if (var_base[i].type == ARTIFICIAL && fabsl((long double)vector_x[i][0]) >= EPSILON)
        {
            printf("\nArtificial variable (non-zero) at the end of optimization, INFEASIBLE problem! (Try larger values for BIGM, -m <value>)\n\n");
            exit(EXIT_FAILURE);
        }

    if (mult_sol)
        printf("\n[WARNING]: MULTIPLE POSSIBLE SOLUTIONS! (One of them presented)\n");
    else
        printf("\nOptimal found !\n");

    printf("\n\tOptimal Solution:\n\n");
    fx = 0;
    for (size_t i = 0; i < number_base; i++)
        if (var_base[i].type == ORIGINAL)
        {
            fx += vector_x[i][0] * var_base[i].cost;
            printf("\tVariable %s -> %.4lf\n", var_base[i].name, vector_x[i][0]);
        }
    for (size_t i = 0; i < number_Nbase; i++)
        if (var_Nbase[i].type == ORIGINAL)
            printf("\tVariable %s -> %.4lf\n", var_Nbase[i].name, 0.0);

    printf("\n\tObjective function value: %.4lf\n\n", signal * fx);
}

void list_cost_equal(double valor, size_t var)
{
    if (len_costs == 0)
    {
        if (!(costs = (cost_t *)malloc(sizeof(cost_t))))
        {
            printf("\n[ERROR]: Memory allocation failed for cost list, function list_cost_iguais()\n\n");
            exit(EXIT_FAILURE);
        }
        len_costs = 1;
        costs[0].value = valor;
        costs[0].index = var;
    }
    else if (costs[0].value > valor) // reset size to 1
    {
        len_costs = 1;
        if (!(costs = (cost_t *)realloc(costs, sizeof(cost_t))))
        {
            printf("\n[ERROR]: Memory allocation failed for cost list, function list_cost_iguais()\n\n");
            exit(EXIT_FAILURE);
        }
        costs[0].value = valor;
        costs[0].index = var;
    }
    else if (costs[0].value == valor) // make list same values (min value)
    {
        if (!(costs = (cost_t *)realloc(costs, sizeof(cost_t) * (++len_costs))))
        {
            printf("\n[ERROR]: Memory allocation failed for cost list, function list_cost_iguais()\n\n");
            exit(EXIT_FAILURE);
        }
        costs[len_costs - 1].value = valor;
        costs[len_costs - 1].index = var;
    }
}

void add_index_var(size_t index)
{
    if (len_last_var == 0)
    {
        if (!(last_var = (size_t *)malloc(sizeof(size_t))))
        {
            printf("\n[ERROR]: Memory allocation failed for index list, function add_index_var()\n\n");
            exit(EXIT_FAILURE);
        }
        last_var[0] = index;
        len_last_var = 1;
    }
    else
    {
        if (!(last_var = (size_t *)realloc(last_var, sizeof(size_t) * (++len_last_var))))
        {
            printf("\n[ERROR]: Memory allocation failed for index list, function add_index_var()\n\n");
            exit(EXIT_FAILURE);
        }
        last_var[len_last_var - 1] = index;
    }
}

bool found_index_var(size_t index)
{
    for (size_t i = 0; i < len_last_var; i++)
        if (last_var[i] == index)
            return true;

    return false;
}

long var_lowest_cost()
{
    if (next_cost >= len_costs)
        return -1;

    if (bland)
    {
        size_t small_index = -1; // max size_t
        long index = -1;

        for (size_t i = 0; i < len_costs; i++)
        {
            size_t var_index = var_Nbase[costs[i].index].index;
            if (var_index < small_index && !found_index_var(var_index))
            {
                small_index = var_index;
                index = costs[i].index;
            }
        }
        add_index_var(small_index);
        return index;
    }
    else
        return (long)costs[next_cost++].index;
}
