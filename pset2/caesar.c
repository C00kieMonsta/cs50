/**
 * caesar.c
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
    // creating a variable which is the key
    int key = 0;
    string plainText = "";
    
    
    // checking if the user uses the program properly
    if (argc != 2)
    {
        printf("You need to insert a key and only a key!\n");
        return 1;
    }
    
    int length1 = strlen(argv[1]);
    
    for (int i = 0; i < length1; i++)
    {
        if (isdigit(argv[1][i]))
        {
            key = atoi(argv[1]);
        }
        else
        {
            return 1;
        }
    }
    
    
    plainText = GetString();
    int length2 = strlen(plainText);
    
    
    for (int i = 0; i < length2; i++)
    {
     
        if (plainText[i] >= 'a' && plainText[i] <= 'z')
        {
            
            if (((plainText[i] - 'a') + key) >= NUM_AL)
            {
                plainText[i] = ((((plainText[i] - 'a') + key) % NUM_AL) + 'a');
            }
            else
            {
                plainText[i] = plainText[i] + key;
            }
        }
        
        else if (plainText[i] >= 'A' && plainText[i] <= 'Z')
        {
            if (((plainText[i] - 'A') + key) >= NUM_AL)
            {
                plainText[i] = ((((plainText[i] - 'A') + key) % NUM_AL) + 'A');
            }
            else
            {
                plainText[i] = plainText[i] + key;
            }
        }
        else
        {
            plainText[i] = plainText[i];
        }
    }
    
    // display the encrypted message
    for (int j = 0, n = strlen(plainText); j < n; j++)
    {
        printf("%c",plainText[j]);
    }
    printf("\n");
}


