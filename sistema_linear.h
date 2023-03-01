#ifndef _LINEAR_
#define _LINEAR_

#include "structs.h"

double** decomposicao_LU(double** a, double** b, size_t n);
double** multi_matriz(double** a, double** b, size_t m, size_t n, size_t r);
double** transposta(double** matriz, size_t m, size_t n);

#endif
