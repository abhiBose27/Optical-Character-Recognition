#include"SDL/SDL.h"
#include"SDL/SDL_image.h"
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<err.h>
#include "pixeloperations.h"
#include "CharDetection.h"



int Char;


// This function is to provide a bit of white background to character images just to have a cleaner look // 
SDL_Surface* IncSizeImg(SDL_Surface* image)
{
    Uint8 r, g, b;
    Uint32 pixel;
    SDL_Surface* newImage;
    int height = image->h;
    int width = image->w;
    int new_Width = width + 10;
    int new_height = height + 10;
    newImage = SDL_CreateRGBSurface(0, new_Width, new_height, 32, 0, 0, 0, 0);
    SDL_FillRect(newImage, NULL, SDL_MapRGB(newImage->format, 255, 255, 255));

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            pixel = get_pixel(image, j, i);
            SDL_GetRGB(pixel, image->format, &r, &g, &b);
            pixel = SDL_MapRGB(newImage->format, r, g, b);
            put_pixel(newImage, j+5, i+5, pixel);
        }
        
    }
    return newImage;
}


// Segment the characters and store them
void CharSeg(SDL_Surface** surfaces, SDL_Surface *image, int h1, int h2, int start, int end)
{
    Uint8 r, g, b;
    SDL_Surface *newImage;
    Uint32 pixel;
    int xCor;
    int yCor = 0;

    newImage = SDL_CreateRGBSurface(0, end-start, h2-h1, 32, 0, 0, 0, 0);
    SDL_FillRect(newImage, NULL, SDL_MapRGB(newImage->format, 255, 255, 255));

    for (int i = h1; i < h2; i++)
    {
        xCor = 0;
        for (int j = start; j < end; j++)
        {
            pixel = get_pixel(image, j, i);
            SDL_GetRGB(pixel, image->format, &r, &g, &b);
            pixel = SDL_MapRGB(newImage->format, r, g, b);
            put_pixel(newImage, xCor, yCor, pixel);
            xCor += 1;
        }
        yCor += 1;

    }
    *(surfaces + Char) = newImage;
    Char += 1;

}


// Finds Characters 
void FindCharacters(SDL_Surface** surfaces, SDL_Surface *image, int start, int end)
{
    Uint8 r, g, b;
    Uint32 pixel;
    int width = image->w;
    int height = end;
    bool metBlack = false;
    bool metWhiterow;
    int y = 0;

    for (int i = 0; i < width; i++)
    {
        metWhiterow = true;
        for (int j = start; j < height; j++)
        {
            pixel = get_pixel(image, i, j);
            SDL_GetRGB(pixel, image->format, &r, &g, &b);
            if (r == 0 && g == 0 && b  == 0)
            {
                metWhiterow = false;
                if (!metBlack)
                {
                    metBlack = true;
                    //DrawVerLine(image, i, start, end);
                    y = i;
                }
            }   
        }
        if (metWhiterow && metBlack)
        {
            metBlack = false;
            CharSeg(surfaces, image, start, end, y, i);
            //DrawVerLine(image, i, start, end);
        }
        
    }
}





// Isolates lines
void FindLineBlock(SDL_Surface *img, SDL_Surface** surfaces)
{
    Uint8 r, g, b;
    Uint32 pixel;
    int width = img->w;
    int height = img->h;
    bool metBlack = false;
    bool metWhiterow;
    int start = 0;
    


    for (int i = 0; i < height; i++)
    {
        metWhiterow = true;
        for (int j = 0; j < width; j++)
        {
            pixel = get_pixel(img, j, i);
			SDL_GetRGB(pixel, img->format, &r, &g, &b);
            if (r == 0 && g == 0 && b == 0)
            {
                metWhiterow = false;
                if (!metBlack)
                {
                    metBlack = true;
                    //DrawHorLine(img, i);
                    start = i;
                }
            }

        }
        if (metWhiterow && metBlack)
        {
            metBlack = false;
            //DrawHorLine(img, i);
            FindCharacters(surfaces, img, start, i);
        }
        
    }
}



// For compression of the image
SDL_Surface* Compression(SDL_Surface* img, int x, int y)
{
    Uint8 r, g, b;
    Uint32 pixel;
    SDL_Surface *CompressedImage;
    int height = img->h;
    int width = img->w;
    float ratioX = (float) width / x;
    float ratioY = (float) height / y;

    CompressedImage = SDL_CreateRGBSurface(0, x, y, 32, 0, 0, 0, 0);
    SDL_FillRect(CompressedImage, NULL, SDL_MapRGB(CompressedImage->format, 255, 255, 255));

    for (int i = 0; i < y; i++)
    {
        for (int j = 0; j < x; j++)
        {
            pixel = get_pixel(img, (int)(j * ratioX),(int)(i * ratioY));
            SDL_GetRGB(pixel, img->format, &r, &g, &b);
            pixel = SDL_MapRGB(CompressedImage->format, r, g, b);
            put_pixel(CompressedImage, j, i, pixel);


        }
    }
    return CompressedImage;

}




/// Wapper function //////
/* Returns the array of Character images with Compression */
SDL_Surface** DetectCharacter(SDL_Surface *img)
{
    int size = img->w;
    //printf("%i\n", size);
    Char = 0;
    SDL_Surface** surfaces = (SDL_Surface**)malloc(size * sizeof(SDL_Surface*));
    SDL_Surface *cImages;
    
    if (surfaces == NULL)
    {
        printf("Could not allocate memory. Sorry!\n");
        exit(-1);
    }


    // Initialise the array with NULL
    for (int i = 0; i < size; i++)
        *(surfaces + i) = NULL;


    
    
    // Function that stores the Character Images in the global array.
    FindLineBlock(img, surfaces);



    // Compress the images in that array.
    int i = 0;
    while (*(surfaces + i) != NULL)
    {
        cImages = *(surfaces + i);
        cImages = IncSizeImg(cImages);
        *(surfaces + i) = Compression(cImages, 28, 28);
        i++;
    }
    
    return surfaces;
}




int NbCharacters()
{
    return Char;
}

