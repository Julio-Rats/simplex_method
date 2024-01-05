#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32
    #include <windows.h>
#elif __linux__
    #include <locale.h>
#else
    #error "OS Not Supported"
#endif

#include "input.h"
#include "simplex.h"

int main(int argc, char* argv[])
{
    #ifdef _WIN32
        SetConsoleOutputCP(65001); //UTF8 
    #elif __linux__
        setlocale(LC_NUMERIC, "Portuguese");
        setlocale(LC_CTYPE, "Portuguese");
    #endif

    srand(time(NULL));

    if (argc < 2)
    {
        printf("\nPor favor, forneça o nome do arquivo a ser lido.\n\n");
        return EXIT_FAILURE;
    }

    input_file(argv[1]);
    simplex();
    
    #ifdef _WIN32
        SetConsoleOutputCP(GetConsoleOutputCP());
    #endif

    return EXIT_SUCCESS;
}