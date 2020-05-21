/**
 * recover.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 * Recovers JPEGs from a forensic image.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 512

int main(int argc, char* argv[])
{   
    // create pointer ti infile and outfiles
    FILE* infile = fopen("card.raw", "r");
    FILE* outfiles = NULL;
    
    // checking if we can create the pointer to infile
    if (infile == NULL)
    {
        fclose(infile);
        fprintf(stderr, "Could not open the file");
        return 1;
    }
    
    // counter to name the outfiles from 0 to ...
    int counter = 0;
    
    // title of the outfiles
    char title_outfile[10];
    
    // array containing the 512 bytes 
    unsigned char buffer[SIZE];
    
    // iterate while fread has read every chunk of 512 bytes
    while (fread(buffer, sizeof(buffer), 1, infile) > 0)
    {
        
        // chacking if those 4 bytes occur
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] == 0xe0 || buffer[3] == 0xe1))
        {
            sprintf(title_outfile, "%03d.jpg", counter);
            
            if (outfiles == NULL)
            {
                outfiles = fopen(title_outfile, "w");
                fwrite(buffer, sizeof(buffer), 1, outfiles);
                counter++;
            }
            else
            {
                fclose(outfiles);
                outfiles = fopen(title_outfile, "w");
                fwrite(buffer, sizeof(buffer), 1, outfiles);
                counter++;
            }
        }
        else
        {
            if (counter != 0)
            {
                fwrite(buffer, sizeof(buffer), 1, outfiles);
            }
        }
    
    }
    
    fclose(outfiles);
    fclose(infile);
    return 0;
}
