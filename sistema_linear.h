#ifndef _LINEAR_
#define _LINEAR_

#include "structs.h"

matrix_t init_matriz(size_t m, size_t n);
matrix_t decomposicao_LU(matrix_t A, matrix_t LU, size_t *vpermut, size_t n);
matrix_t solver_LU(matrix_t LU, size_t *vpermut, matrix_t x, matrix_t b, size_t n);
matrix_t multi_matriz(matrix_t A, matrix_t B, matrix_t dest, size_t m, size_t n, size_t r);
matrix_t multi_escalar(matrix_t src, double escalar, matrix_t dest, size_t m, size_t n);
matrix_t transposta(matrix_t src, matrix_t dest, size_t m, size_t n);

#endif
