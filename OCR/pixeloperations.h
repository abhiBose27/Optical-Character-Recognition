#ifndef PIXEL_OPERATIONS_H
#define PIXEL_OPERATIONS_H

#include <stdbool.h>
#include "SDL2/SDL.h"

Uint32 get_pixel(SDL_Surface *surface, unsigned x, unsigned y);
void put_pixel(SDL_Surface *surface, unsigned x, unsigned y, Uint32 pixel);
void pretty_print_hist(double* histogram, size_t size);
double get_pixel_intensity(SDL_Surface* image, unsigned x, unsigned y);
double* get_image_to_pixel_intensity_matrix(SDL_Surface* image);
double get_mean_pixel_intensity_density(double* histogram, size_t size);
double* get_horizontal_pixel_intensity_histogram(SDL_Surface* image, double pixel_intensity, bool density, size_t x1, size_t x2);
double* get_vertical_pixel_intensity_histogram(SDL_Surface* image, double pixel_intensity, bool density, size_t y1, size_t y2);

#endif
