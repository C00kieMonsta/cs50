/**
 * helpers.c
 *
 * Computer Science 50
 * Problem Set 3
 *
 * Helper functions for Problem Set 3.
 */
       
#include <cs50.h>

#include "helpers.h"

/**
 * Returns true if value is in array of n values, else false.
 */
bool search(int value, int values[], int n)
{
    int lowerBound = 0;
    int upperBound = n - 1;
    int point = 0;
    
    while(lowerBound <= upperBound)
    {   
        point = ((upperBound + lowerBound) / 2);
        if (values[point] < value)
        {
            lowerBound = point + 1;
        }
        else if (values[point] > value)
        {
            upperBound = point - 1;
        }
        else
            return true;
    }
    return false;
}

/**
 * Sorts array of n values.
 */
void sort(int values[], int n)
{
    for (int i = 0; i < n; i++)
    {
        int element = values[i];
        int j = i;
        
        while (j > 0 && values[j - 1] > element)
        {
            values[j] = values[j - 1];
            j = j - 1;
        }
        
        values[j] = element;
    }
    
    return;
}
