#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "structs.h"

#ifdef _WIN32
    #include <windows.h>
#elif __linux__
    #include <locale.h>
#else
    #error "OS Not Supported"
#endif

/*  Global Function  */
void input_file(string path_file); // input.c
void simplex();                    // simplex.c


/* Main Program */
int main(int argc, char* argv[])
{
    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
    #elif __linux__
        setlocale(LC_NUMERIC, "Portuguese");
        setlocale(LC_CTYPE, "Portuguese");
    #endif

    if (argc < 2)
    {
        printf("\nPlease provide the constraints file name via parameters.\n\n");
        printf("Uso:\n\tsimplex <file.txt>\n\n");
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