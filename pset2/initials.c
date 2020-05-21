/**
 * initials.c
 *
 * Antoine Boxho
 * 
 * A program which allows you to display the initials of a name
 *
 */

#include <cs50.h>
#include <stdio.h>
#include <string.h>


// Function to upper a character 
char changeUpper(char caracter);

int main(void)
{
    string name = GetString();
    printf("%c", changeUpper(name[0]));
    
    // We iterate over each character of the given name 
    for (int i = 0, n = strlen(name); i < n; i++)
    {
        // If there is a space before the character at position i+1
        // we make it upper
        if (name[i] == ' ' && name[i + 1] != '\0')
        {
            printf("%c", changeUpper(name[i + 1]));
        }
    }
    printf("\n");
}

char changeUpper(char caracter)
{
    
    if (caracter >= 'a' && caracter <= 'z')
    {
        caracter = caracter + ('A' - 'a');
    }
    return caracter;
}
