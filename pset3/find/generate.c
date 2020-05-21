/***************************************************************************
 * generate.c
 *
 * Computer Science 50
 * Problem Set 3
 *
 * Generates pseudorandom numbers in [0,LIMIT), one per line.
 *
 * Usage: generate n [s]
 *
 * where n is number of pseudorandom numbers to print
 * and s is an optional seed
 ***************************************************************************/
 
// standard libraries
#define _XOPEN_SOURCE
#include <cs50.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// constant
#define LIMIT 65536

int main(int argc, string argv[])
{
    // It checks if we correctely use the program, 0 arguments is not enough but 3 is too much
    if (argc != 2 && argc != 3)
    {
        printf("Usage: generate n [s]\n");
        return 1;
    }

    // We convert the frist argument which is a string in an integer
    int n = atoi(argv[1]);

    // If the user entered 2 arguments, 
    if (argc == 3)
    {
        srand48((long int) atoi(argv[2]));
    }
    else
    {
        // initialising srand48 function on the actual time we run the program
        srand48((long int) time(NULL));
    }

    // We loop to display the random numbers between 0 and LIMIT
    for (int i = 0; i < n; i++)
    {
        printf("%i\n", (int) (drand48() * LIMIT));
    }

    // success
    return 0;
}
