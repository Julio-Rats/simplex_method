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
        SetConsoleOutputCP(65001); //UTF-8
    #elif __linux__
        setlocale(LC_NUMERIC, "Portuguese");
        setlocale(LC_CTYPE, "Portuguese");
    #endif

    if (argc < 2)
    {
        printf("\nPor favor forneça o nome do arquivo de restrições via parâmetros.\n\n");
        printf("Uso:\n\tsimplex <arquivo.txt>\n\n");
    }
    else
    {
        srand(time(NULL));
        input_file(argv[1]);
        simplex();
    }
    
    #ifdef _WIN32
        SetConsoleOutputCP(GetConsoleOutputCP());
    #endif

    return EXIT_SUCCESS;
}