#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "pixeloperations.h"
#include "Resize.h"

// A potential Image bug fix
SDL_Surface* resize(SDL_Surface* image, size_t factor){
    Uint8 r, g, b;
    Uint32 pixel;
    SDL_Surface* newImage;
    size_t height = image->h;
    size_t width = image->w;
    size_t new_width = width + factor;
    size_t new_height = height + factor;
    newImage = SDL_CreateRGBSurface(0, new_width, new_height, 32, 0, 0, 0, 0);
    SDL_FillRect(newImage, NULL, SDL_MapRGB(newImage->format, 255, 255, 255));

    for (size_t x = 0; x < width; x++){
        for (size_t y = 0; y < height; y++){
            pixel = get_pixel(image, x, y);
            SDL_GetRGB(pixel, image->format, &r, &g, &b);
            pixel = SDL_MapRGB(newImage->format, r, g, b);
            size_t new_x = round(x + factor / 2);
            size_t new_y = round(y + factor / 2);
            put_pixel(newImage, new_x, new_y, pixel);
        }
    }
    return newImage;
}
