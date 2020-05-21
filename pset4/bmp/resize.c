/**
 * resize.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 * A program which resizes a picture
 */
       
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "bmp.h"


int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: ./copy size infile outfile\n");
        return 1;
    }

    // remember filenames
    int size = atoi(argv[1]);
    
    // checking if the size is correctly defined
    if (size < 0 || size > 100)
    {
        // printf("The should be positif and less or equal to 100");
        return 2;
    }
    
    char* infile = argv[2];
    char* outfile = argv[3];

    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 3;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 4;
    }

    // new variable bf of type BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    
    // new variable of type BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    
    // it starts reading into the bitmapfile from the beginning
    // it says that we are going to put inside "bf" what we read from inptr, which is our infile
    // how much do we read? sizeof(BIT...)
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);
    
    // it says that we are going to put inside "bi" what we read from inptr, which is our infile
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);
    

    // in other words we are making sure that the infile is a bitmap
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }
    
    BITMAPFILEHEADER resize_bf;
    BITMAPINFOHEADER resize_bi;
    resize_bf = bf;
    resize_bi = bi;
    
    // update bitmapinfo header
    resize_bi.biWidth = bi.biWidth * size;
    resize_bi.biHeight = bi.biHeight * size;
    
    // determine padding for scanlines
    int padding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int resize_padding = (4 - (resize_bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    // updating bitmapinfo header (from internet)
    resize_bi.biSizeImage = ((((resize_bi.biWidth * resize_bi.biBitCount) + 31) & ~31) / 8) * abs(resize_bi.biHeight);
    
    // update bitmapfile header (equation from internet)
    resize_bf.bfSize = 54 + resize_bi.biWidth * abs(resize_bi.biHeight) * 3 + abs(resize_bi.biHeight) *  resize_padding;

    // write outfile's BITMAPFILEHEADER
    // we create a new bitmap, so we have to make sure that we include the BITMAPFILEHEADER and BITMAPINFOHEADER
    fwrite(&resize_bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&resize_bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
    
        // resize vertically
        for (int l = 0; l < size; l++)
        {
            // iterate over pixels in scanline
            for (int j = 0; j < bi.biWidth; j++)
            {
                // temporary storage
                RGBTRIPLE triple;

                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
                
                // resize horizontally
                for (int f = 0; f < size; f++)
                {
                    // write RGB triple to outfile where outptr is the pointer to that file
                    // this is writing several bytes at a time
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                }  
            }
            
            // skip over padding, if any paddings
            fseek(inptr, padding, SEEK_CUR);
            

            // then add it back (to demonstrate how)
            for (int k = 0; k < resize_padding; k++)
            {
                fputc(0x00, outptr);
            }
            
            fseek(inptr, -(bi.biWidth * sizeof(RGBTRIPLE) + padding ), SEEK_CUR);
        }
        fseek(inptr, bi.biWidth * sizeof(RGBTRIPLE) + padding, SEEK_CUR);
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);
    

    // that's all folks
    return 0;
}
