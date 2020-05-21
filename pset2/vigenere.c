/**
 * vigenere.c
 *
 * Antoine Boxho
 * 
 * Simple program which enables you to encrypt and decrypt a message
 *
 */


#include <cs50.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define NUM_AL 26

int main(int argc, string argv[])
{      
    // checking if the user uses the program properly
    if (argc != 2)
    {
        printf("You need to insert a key and only a key!\n");
        return 1;
    }
    
    // creating a variable which is the key
    int size_key = strlen(argv[1]);
    int keys[size_key];
    
    // Looping to see if the key is correctely given
    for (int i = 0; i < size_key; i++)
    {
        // The key must be a word
        if (isdigit(argv[1][i]))
        {
            printf("The key must be a word! \n");
            return 1;
        }
        else
        {
            // We create an array of keys, converting every letter in a digit
            for (int j = 0; j < size_key; j++)
            {
                keys[j] = (int) argv[1][j];
                if (argv[1][j] >= 'a' && argv[1][j] <= 'z')
                {
                    keys[j] = ((int) argv[1][j] - 'a');
                }
                else
                {
                    keys[j] = ((int) argv[1][j] - 'A');
                } 
            }
        }
    }   

    string pText = GetString();
    
    int size_string = strlen(pText);
    int counter = 0;
    int k = 0;
    int j = 0;
    
    // The loop continues while counter is <= to the size of plaintext
    while (counter <= size_string)
    {
        // Verify if it's a lowercase
        if (pText[j] >= 'a' && pText[j] <= 'z')
        {
        
            if (((pText[j] - 'a') + keys[k]) >= NUM_AL)
            {
                pText[j] = ((((pText[j] - 'a') + keys[k]) % NUM_AL) + 'a');
            }
            else
            {
                pText[j] = pText[j] + keys[k];
            }
            k++;
            k = k % size_key;
        }
    
    
        // Verify if it's a uppercase
        else if (pText[j] >= 'A' && pText[j] <= 'Z')
        {
            if (((pText[j] - 'A') + keys[k]) >= NUM_AL)
            {
                pText[j] = ((((pText[j] - 'A') + keys[k]) % NUM_AL) + 'A');
            }
            else
            {
                pText[j] = pText[j] + keys[k];
            }
            k++;
            k = k % size_key;
        }
        else
        {
            pText[j] = pText[j];
        }
        
        j++;
        counter++;
    }
    
    
    // display the encrypted message
    for (int index = 0;index < size_string; index++)
    {
        printf("%c",pText[index]);
    }
    printf("\n");
    

 }   
    
    

