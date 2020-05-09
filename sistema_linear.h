#ifndef  _LINEAR_
#define  _LINEAR_

#include "structs.h"

double**  decomposicao_LU(double** a, double** b, int n);
double    modulo(double valor);
double**  multi_matriz(double** a, double** b, int m, int n, int r);
double**  multi_escalar(double** matriz, double escalar, int m, int n);
double**  transposta(double** matriz, int m, int n);

#endif
