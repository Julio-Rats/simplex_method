#include "input.h"


int main(int argc, char const *argv[]) {

    srand(time(NULL));
    input_file("teste.txt");
    simplex();
    //
    // double** m= malloc(sizeof(double*)*3);
    // m[0]= malloc(sizeof(double)*3);
    // m[1]= malloc(sizeof(double)*3);
    // m[2]= malloc(sizeof(double)*3);
    //
    // m[0][0] = 1;
    // m[1][0] = 0;
    // m[2][0] = 0;
    // m[0][1] = -1;
    // m[1][1] = 1;
    // m[2][1] = 0;
    // m[0][2] = 0;
    // m[1][2] = 0;
    // m[2][2] = 1;
    //
    // double** b = malloc(sizeof(double*)*3);
    // b[0] = malloc(sizeof(double));
    // b[1] = malloc(sizeof(double));
    // b[2] = malloc(sizeof(double));
    //
    // b[0][0] = 2;
    // b[1][0] = 4;
    // b[2][0] = 1;
    //
    // double** x = decomposicao_LU(m,b, 3);
    // // x = decomposicao_LU(m,b, 2);
    // // x = decomposicao_LU(m,b, 2);
    //
    // for (size_t i = 0; i < 3; i++)
    // {
    //     for (size_t j = 0; j < 3; j++)
    //         printf("%.2lf ", m[i][j]);
    //     printf("\n");
    // }
    //
    // printf("b\n");
    //
    // for (size_t i = 0; i < 3; i++)
    // {
    //   printf("%.2lf\n", b[i][0]);
    // }

    // double** z = decomposicao_LU(m, b, 3);

    // printf("x\n");
    //
    // for (size_t i = 0; i < 3; i++)
    // {
    //   printf("%.2lf\n", x[i][0]);
    // }

    //
    // printf("z\n");
    //
    // for (size_t i = 0; i < 3; i++)
    // {
    //   printf("%.2lf\n", z[i][0]);
    // }


    return 0;
}
