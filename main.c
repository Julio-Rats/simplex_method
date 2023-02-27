#include <stdlib.h>
#include <time.h>
#include <locale.h>

#include "input.h"
#include "simplex.h"

int main(int argc, char const *argv[])
{
    setlocale(LC_NUMERIC, "Portuguese");
    setlocale(LC_CTYPE, "Portuguese");
    srand(time(NULL));
    input_file("teste.txt");
    simplex();

    return 0;
}
