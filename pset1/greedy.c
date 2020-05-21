/**
 * greedy.c
 *
 * Antoine Boxho
 * 
 * Simple greedy program
 *
 */
 
// we include the libraries
#include <cs50.h>
#include <stdio.h>
#include <math.h>

// we define some constant variables
#define QUARTER 25
#define DIME 10
#define NICKEL 5
#define PENNY 1

int main(void)
{
    int count = 0;
    float change = 0.0;
    
    do
    {
        printf("Hai! How much change is owed? ");
        change = GetFloat();
    }while (change < 0.0);
    
    int conChange = round(change * 100);
    
    while ((conChange / QUARTER) >= 1)
    {
        count += 1;
        conChange = conChange - QUARTER;
    }
    
    while ((conChange / DIME) >= 1)
    {
        count += 1;
        conChange = conChange - DIME;
    }
    
    while ((conChange / NICKEL) >= 1)
    {
        count += 1;
        conChange = conChange - NICKEL;
    }

    
    while ((conChange / PENNY) >= 1)
    {
        count += 1;
        conChange = conChange - PENNY;
    }
    
    printf("%d\n", count);
}


