#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>

#include "input.h"
#include "simplex.h"

int main(int argc, char* argv[])
{
    setlocale(LC_NUMERIC, "Portuguese");
    setlocale(LC_CTYPE, "Portuguese");
    srand(time(NULL));

    if (argc < 2)
    {
        printf("\nPor favor, forneça o nome do arquivo a ser lido.\n\n");
        return EXIT_FAILURE;
    }

    input_file(argv[1]);
    simplex();
    
    return EXIT_SUCCESS;
}