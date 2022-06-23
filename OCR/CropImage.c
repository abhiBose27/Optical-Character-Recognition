#include"SDL/SDL.h"
#include"SDL/SDL_image.h"
#include<stdio.h>
#include<stdlib.h>
#include "pixeloperations.h"
#include "CropImage.h"
#include<err.h>

// A potential Image bug fix
SDL_Surface* IncreaseTheSizeImage(SDL_Surface* image)
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








SDL_Surface* ChipTheEdges(SDL_Surface *img)
{
    SDL_Surface* s;
    s = IncreaseTheSizeImage(img);
    return s;

}
