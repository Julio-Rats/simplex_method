#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "structs.h"

#if defined(__CYGWIN__) || defined(_WIN32)
#include <windows.h>
#elif __linux__
#include <locale.h>
#else
#error "OS Not Supported"
#endif

/*  Global Variables  */
extern long double BIGM;    // input.c
extern long double EPSILON; // sistema_linear.c
// simplex.c
extern bool bland;
extern bool verbose;
extern bool iterative;
// extern unsigned long steps;

/*  Global Function  */
void input_file(string path_file); // input.c
void simplex();                    // simplex.c

void get_args(int argc, char *argv[])
{
    for (size_t i = 1; i < argc; i++)
        if (argv[i][0] == '-')
            switch (argv[i][1])
            {
            case 'b':
                bland = true;
                break;
            case 'd':
                verbose = false;
                iterative = false;
                break;
            case 'e':
                if (i < argc - 2)
                {
                    EPSILON = atof(argv[i + 1]);
                    printf("\n[Warning]: Make sure the value of EPSILON is small enough for the problem\n\n");
                }
                else
                    printf("\n[Warning]: Missing value in -e parameter\n\n");
                break;
            case 'i':
                iterative = verbose;
                break;
            case 'm':
                if (i < argc - 2)
                {
                    BIGM = atof(argv[i + 1]);
                    printf("\n[Warning]: Make sure the value of BIGM is large enough for the problem\n\n");
                }
                else
                    printf("\n[Warning]: Missing value in -m parameter\n\n");
                break;
                // case 's':
                //     if (i < argc - 2)
                //         steps = atol(argv[i + 1]);
                //     else
                //         printf("\n[Warning]: Missing value in -s parameter\n\n");
                //     break;
            }
}

/* Main Program */
int main(int argc, char *argv[])
{

#if defined(__CYGWIN__) || defined(_WIN32)
    SetConsoleOutputCP(CP_UTF8);
#elif __linux__
    setlocale(LC_NUMERIC, "en_US.UTF-8");
    setlocale(LC_CTYPE, "en_US.UTF-8");
#endif

    if (argc == 1)
    {
        printf("\nPlease provide the constraints file name via parameters.\n\n"
               "Use:\n\tsimplex [options] <file.txt>\n\n"
               "Options:\n"
               "-b:         Enable Bland's pivot rule\n"
               "-d:         Disable verbose mode\n"
               "-e <value>: Set manual value to Epsilon (Default: 1e-9)\n"
               "-i:         Activate iterative mode (incompatible with disable verbose, -d)\n"
               "-m <value>: Set manual value to BIGM (Default: 1e9)\n\n"
               //    "-s <value>: Defines maximum interactions and new instructions for the user\n\n"
        );
    }
    else
    {
        srand(time(NULL));
        get_args(argc, argv);
        input_file(argv[argc - 1]);
        simplex();
    }

#if defined(__CYGWIN__) || defined(_WIN32)
    SetConsoleOutputCP(GetConsoleOutputCP());
#endif

    return EXIT_SUCCESS;
}