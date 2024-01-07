#ifndef _LINEAR_
#define _LINEAR_

#include "structs.h"

matriz_t decomposicao_LU(matriz_t A, matriz_t LU, size_t *vpermut, size_t n);
matriz_t solver_LU(matriz_t LU, size_t *vpermut, matriz_t x, matriz_t b, size_t n);
matriz_t multi_matriz(matriz_t A, matriz_t B, matriz_t dest, size_t m, size_t n, size_t r);
matriz_t multi_escalar(matriz_t src, double escalar, matriz_t dest, size_t m, size_t n);
matriz_t transposta(matriz_t src, matriz_t dest, size_t m, size_t n);

#endif
