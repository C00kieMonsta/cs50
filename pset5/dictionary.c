#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "dictionary.h"


// size of hashtable
#define SIZE 1000000

/*char* changeUpper(const char* s)*/
/*{*/
/*    char* l = NULL;*/
/*    strcpy(l, s);*/
/*    */
/*    */
/*    for(int i = 0, n = strlen(l); i<n; i++)*/
/*    {*/
/*        if(l[i] >= 'a' && l[i] <= 'z')*/
/*        {*/
/*            l[i] = l[i]+('A'-'a');*/
/*        }*/
/*        else*/
/*        {*/
/*            l[i] = l[i];*/
/*        }*/
/*    }*/
/*    */
/*    return l;*/
/*}*/


// create nodes for linked list
typedef struct node
{
    char word[LENGTH+1];
    struct node* next;
}
node;

// create hashtable
node* hashtable[SIZE] = {NULL};

// create hash function
// hash function from internet
int hashing (const char* word)
{
    int hash = 0;
    int n;
    for (int i = 0; word[i] != '\0'; i++)
    {
        if(isalpha(word[i]))
        {
            n = word [i] - 'a' + 1;
        }
        else
        {
            n = 27;
        }   
        hash = ((hash << 3) + n) % SIZE;
    }
    return hash;    
}

// create global variable to count size
int numberWord = 0;

/**********************************************************************************
 * Returns true if word is in dictionary else false.
 *********************************************************************************/
bool check(const char* word)
{
    char new_word[LENGTH + 1];
    int n = strlen(word);
    
/*    strcpy(new_word, changeUpper(word));*/
    
    for(int i = 0; i < n; i++)
    {
      new_word[i] = tolower(word[i]);  
    }
    new_word[n] = '\0';
    
    int index = hashing(new_word);
    
    if (hashtable[index] == NULL)
    {
        return false;
    }
    
    node* crawler = hashtable[index];
    
    while(crawler != NULL)
    {
        if (strcmp(new_word, crawler->word) == 0) 
        {
            return true;
        }
        else
        {
            crawler = crawler->next;
        }
    }
    
    return false;
}


/*********************************************************************************
 * Loads dictionary into memory.  Returns true if successful else false.
 *********************************************************************************/
bool load(const char* dictionary)
{
    FILE* dico = fopen(dictionary, "r");
    if (dico == NULL)
    {
        fclose(dico);
        fprintf(stderr, "Could not open the file");
        return false;
    }
    
    char word[LENGTH+1];
    
    while (fscanf(dico, "%s\n", word) != EOF)
    {
        numberWord++;
        
        node* new_node = malloc(sizeof(node));
        
        if (new_node == NULL)
        {
            return false;
        }
        
        strcpy(new_node->word, word);
        
        new_node->next = NULL;
        
        // now that we've stored the word, we've to hash the word
        // in order to receive the position of the array in which we
        // should store the new_node
        int index = hashing(word);
        
        // if the index is greater than the size, than there is a problem
        // with the hash function
        if (index > SIZE)
            return false;
        
        if (hashtable[index] == NULL)
        {
            hashtable[index] = new_node;
            new_node =NULL;
        }
        else if (toupper(new_node->word[1]) < toupper(hashtable[index]->word[1]))
        {
            new_node->next = hashtable[index];
            hashtable[index] = new_node;
        }
        
        else
        {
            node* predptr = hashtable[index];
            while(true)
            {
                if (strcmp(predptr->word, new_node->word) == 0)
                {
                    free(new_node);
                    break;
                }
                else if (predptr->next == NULL)
                {
                    predptr->next = new_node;
                    break;
                }
                else if (predptr->next->word[1] > new_node->word[1])
                {
                    new_node->next = predptr->next;
                    predptr->next = new_node;
                    break;
                }
                
                predptr = predptr->next;
            }
        
        }
    }
    
    fclose(dico);
    return true;
}



/**********************************************************************************
 * Returns number of words in dictionary if loaded else 0 if not yet loaded.
 *********************************************************************************/
unsigned int size(void)
{
    if (numberWord > 0)
    {
        return numberWord;
    }
    else
    {
        return 0;
    }
}


/**********************************************************************************
 * Unloads dictionary from memory.  Returns true if successful else false.
 *********************************************************************************/
bool unload(void)
{
    int index = 0;
    
    while (index < SIZE)
    {
        if (hashtable[index] == NULL)
        {
            index++;
        }
        else
        {
            while(hashtable[index] != NULL)
            {
                node* crawler = hashtable[index];
                hashtable[index] = crawler->next;
                free(crawler);
            }
            index++;
        }
    }
    return true;
}
