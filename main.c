#include "input.h"


int main(int argc, char const *argv[]) {

    srand(time(NULL));
    input_file("teste.txt");

    simplex();

    // printf("Basicas\n\n");
    // for (int i = 0; i < number_base; i++)
    // {
    //     printf("%s: ", var_base[i].name);
    //     for (int j = 0; j < number_base; j++)
    //     {
    //         printf("%.2lf ",var_base[i].aj[0][j]);
    //     }
    //     printf("\n");
    // }
    // printf("\nNAO Basicas\n\n");
    // for (int i = 0; i < number_Nbase; i++)
    // {
    //     printf("%s: ", var_Nbase[i].name);
    //     for (int j = 0; j < number_base; j++)
    //     {
    //         printf("%.2lf ",var_Nbase[i].aj[0][j]);
    //     }
    //     printf("\n");
    // }
    // printf("\n");
    //
    // for (int i = 0; i < number_base; i++)
    //     printf("b[%d]: %.2lf\n", i, vetor_b[i][0]);


    return 0;
}
