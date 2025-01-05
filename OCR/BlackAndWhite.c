#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <err.h>
#include <math.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "pixeloperations.h"
#include "BlackAndWhite.h"


/// Does the Black and white aspect of the images
void gray_scale(SDL_Surface* image){
    size_t width = image->w;
    size_t height = image->h;
    Uint32 pixel;
    Uint8 r, g, b, newcolor;
    for (size_t x = 0; x < width; x++){
        for (size_t y = 0; y < height; y++){
            pixel = get_pixel(image, x, y);
            SDL_GetRGB(pixel, image->format, &r, &g, &b);
            newcolor = 0.3 * r + 0.59 * g + 0.11 * b;
            pixel = SDL_MapRGB(image->format, newcolor, newcolor, newcolor);
            put_pixel(image, x, y, pixel);
        }
    }
}

float get_adaptive_threshold(SDL_Surface* image, size_t x, size_t y, int blocksize) {
    int count = 0;
    float sum = 0;
    int halfsize = blocksize / 2;
    for (int i = -halfsize; i <= halfsize; i++) {
        for (int j = -halfsize; j <= halfsize; j++) {
            int new_x = x + i;
            int new_y = y + j;
            if (new_x >= 0 && new_x < image->w && new_y >= 0 && new_y < image->h) {
                sum += get_pixel_intensity(image, new_x, new_y);
                count++;
            }
        }
    }
    return (count > 0) ? sum / count : 0.0; 
}

void binarization(SDL_Surface* image, int blocksize){
    size_t width = image->w;
    size_t height = image->h;
    Uint32 pixel;
    Uint8 new_intensity;
    float threshold, intensity;
    for (size_t x = 0; x < width; x++) {
        for (size_t y = 0; y < height; y++) {
            threshold = get_adaptive_threshold(image, x, y, blocksize);
            intensity = get_pixel_intensity(image, x, y);
            new_intensity = intensity < threshold ? 0 : 255;
            pixel = SDL_MapRGB(image->format, new_intensity, new_intensity, new_intensity);
            put_pixel(image, x, y, pixel);
        }
    }
}

float get_normalised_factor(int contrast_factor) {
    return (259 * (contrast_factor + 255)) / (255 * (259 - contrast_factor)); 
}

Uint8 get_new_intensity(float intensity, float normalised_factor) {
    return (normalised_factor * intensity - 128) + 128;
}

void contrast(SDL_Surface* image, int contrast_factor){
    Uint8 new_intensity;
    Uint32 pixel;
    size_t width = image->w;
    size_t height = image->h;
    float normalised_factor, intensity;
    for (size_t x = 0; x < width; x++){
        for (size_t y = 0; y < height; y++) {
            normalised_factor = get_normalised_factor(contrast_factor);
            intensity = get_pixel_intensity(image, x, y);
            new_intensity = get_new_intensity(intensity, normalised_factor);
            pixel = SDL_MapRGB(image->format, new_intensity, new_intensity, new_intensity);
            put_pixel(image, x, y, pixel);
        }
    }
}