#include "simplex.h"


int main(int argc, char const *argv[]) {

  srand(time(NULL));
  simplex();

  //
  // int m = 3;
  // int n = 3;
  // int r = 1;
  //
  // double** a = (double**) malloc(sizeof(double*)*m);
  // for (size_t i = 0; i < m; i++)
  //     a[i] = (double*) malloc(sizeof(double)*n);
  //
  // double** b = (double**) malloc(sizeof(double*)*n);
  // for (size_t i = 0; i < n; i++)
  //     b[i] = (double*) malloc(sizeof(double)*r);
  //
  // a[0][0] = 1;
  // a[1][0] = 0;
  // a[2][0] = 0;
  //
  // a[0][1] = 0;
  // a[1][1] = 1;
  // a[2][1] = 0;
  //
  // a[0][2] = 0;
  // a[1][2] = 0;
  // a[2][2] = 1;
  //
  // b[0][0] = 1;
  // b[1][0] = 1;
  // b[2][0] = 0;
  //
  //
  // double** c = decomposicao_LU(a, b, n);
  // c = decomposicao_LU(a, b, n);
  // c = decomposicao_LU(a, b, n);
  // c = decomposicao_LU(a, b, n);
  //
  // for (int i = 0; i < m; i++)
  // {
  //    printf("\n");
  //    for (int j = 0; j < r; j++)
  //        printf("%.2lf\t", c[i][j]);
  // }
  // printf("\n");

  return 0;
}
