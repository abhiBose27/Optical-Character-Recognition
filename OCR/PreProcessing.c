#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <err.h>
#include <math.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "pixeloperations.h"
#include "PreProcessing.h"
#include "Kernels.h"


float get_adaptive_threshold(SDL_Surface* image, size_t x, size_t y, int blocksize) {
    int count = 0, halfsize = blocksize / 2;
    float sum = 0;
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

float get_normalised_factor(int contrast_factor) {
    return (259 * (contrast_factor + 255)) / (255 * (259 - contrast_factor)); 
}

// Median sorting helper
int compare(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

Uint8 get_new_intensity(float intensity, float normalised_factor) {
    return (normalised_factor * intensity - 128) + 128;
}

float get_variance(size_t arr[], size_t n){
	float sum = 0.0, sum_eq = 0.0;
	for (size_t i = 0; i < n; i++)
        sum += arr[i];

    for (size_t i = 0; i < n; i++)
        sum_eq += pow((arr[i] - sum / (float)n), 2);

    return sum_eq / (float)n;
}

void init_hist(size_t arr[], size_t n){
	for(size_t i = 0; i < n;i++)
		arr[i] = 0;
}

void gray_scale(SDL_Surface* image){
    size_t width = image->w, height = image->h;
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

void contrast(SDL_Surface* image, int contrast_factor){
    Uint8 new_intensity;
    Uint32 pixel;
    size_t width = image->w, height = image->h;
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

SDL_Surface* binarization(SDL_Surface* image, int blocksize){
    Uint32 pixel;
    Uint8 new_intensity;
    float threshold, intensity;
    size_t width = image->w, height = image->h;
    SDL_Surface* new_image = SDL_CreateRGBSurface(image->flags, image->w, image->h, image->format->BitsPerPixel,
        image->format->Rmask, image->format->Gmask, image->format->Bmask, image->format->Amask);
    for (size_t x = 0; x < width; x++) {
        for (size_t y = 0; y < height; y++) {
            threshold = get_adaptive_threshold(image, x, y, blocksize);
            intensity = get_pixel_intensity(image, x, y);
            new_intensity = intensity < threshold ? 0 : 255;
            pixel = SDL_MapRGB(new_image->format, new_intensity, new_intensity, new_intensity);
            put_pixel(new_image, x, y, pixel);
        }
    }
    return new_image;
}

SDL_Surface* man_deskew(SDL_Surface* image, float angle){
    SDL_Surface* new_image = SDL_CreateRGBSurface(image->flags, image->w, image->h, image->format->BitsPerPixel,
        image->format->Rmask, image->format->Gmask, image->format->Bmask, image->format->Amask);
    SDL_FillRect(new_image, NULL, SDL_MapRGB(new_image->format, 255, 255, 255));


	float CX = image->w / 2, CY = image->h / 2;
	float X, Y, X2, Y2;
	float radians = (angle * 3.14) / 100;

	for(int y = 0; y < image->h; y++) {
        for(int x= 0; x < image->w; x++) {
			X = x - CX;
			Y = y - CY;
			X2 = (X * cos(radians) - Y * sin(radians));
			Y2 = (X * sin(radians) + Y * cos(radians));
			X2 += CX;
			Y2 += CY;
			if( X2 >= image->w || X2 < 0 || Y2 >= image->h || Y2 < 0) 
				continue;
			put_pixel(new_image, x, y, get_pixel(image, X2, Y2));
		}
	}
	return new_image;
}

float skew_degree(SDL_Surface* image, float precision){
	Uint32 pixel;
	size_t sum;
	size_t height = image->h, width = image->w;
	size_t hist[height];
	Uint8 r, g, b;
	float max_variance = 0, max_variance_deg = 0, variance;
	float max = 45., min = -45.;
	for(; min < max; min += precision){
		SDL_Surface* tilted_copy = man_deskew(image, min);
		for (size_t y = 0; y < height; y++){
			sum = 0;
			for(size_t x = 0; x < width; x++){
				pixel = get_pixel(tilted_copy, x, y);
				SDL_GetRGB(pixel, tilted_copy->format, &r, &g, &b);
				if (r == 0)
					sum +=1;
			}
			hist[y]= sum;
		}
		variance = get_variance(hist, height);
		
		if (variance > max_variance){
			max_variance = variance;
			max_variance_deg = min;
		}
		SDL_FreeSurface(tilted_copy);
		init_hist(hist, height);
	}
	return max_variance_deg;
}

SDL_Surface* auto_deskew(SDL_Surface* image){
	return man_deskew(image, skew_degree(image, 0.25));
}

SDL_Surface* convolution(SDL_Surface* image, float kernel[], size_t kernel_size) {
    Uint32 pixel;
    Uint8 new_intensity;
    size_t width = image->w, height = image->h;
    int offset = kernel_size / 2;
    float kernel_sum = 0;
    SDL_Surface* new_image = SDL_CreateRGBSurface(image->flags, image->w, image->h, image->format->BitsPerPixel,
        image->format->Rmask, image->format->Gmask, image->format->Bmask, image->format->Amask);
    SDL_FillRect(new_image, NULL, SDL_MapRGB(new_image->format, 255, 255, 255));

    for (size_t x = 0; x < kernel_size; x++) {
        for (size_t y = 0; y < kernel_size; y++)
            kernel_sum += kernel[x * kernel_size + y];
    }

    if (kernel_sum == 0) kernel_sum = 1;

    for (size_t x = 0; x < width; x++) {
        for (size_t y = 0; y < height; y++) {
            float intensity = 0;
            for (int kx = -offset; kx <= offset; kx++) {
                for (int ky = -offset; ky <= offset; ky++) {
                    int ny = y + ky;
                    int nx = x + kx;
                    if (ny >= 0 && ny < height && nx >= 0 && nx < width)
                        intensity += get_pixel_intensity(image, nx, ny) * 
                                    kernel[(kx + offset) * kernel_size + (ky + offset)];
                }
            }
            new_intensity = intensity / kernel_sum;
            pixel = SDL_MapRGB(new_image->format, new_intensity, new_intensity, new_intensity);
            put_pixel(new_image, x, y, pixel);
        }
    }
    return new_image;
}

SDL_Surface* median_filter(SDL_Surface* image, size_t kernel_size) {
    float window[kernel_size * kernel_size];
    float intensity;
    int offset = kernel_size / 2;
    size_t width = image->w, height = image->h;
    SDL_Surface* new_image = SDL_CreateRGBSurface(image->flags, image->w, image->h, image->format->BitsPerPixel,
        image->format->Rmask, image->format->Gmask, image->format->Bmask, image->format->Amask);
    SDL_FillRect(new_image, NULL, SDL_MapRGB(new_image->format, 255, 255, 255));
    
    for (size_t x = 0; x < width; x++) {
        for (size_t y = 0; y < height; y++) {
            size_t k = 0;
            for (int kx = -offset; kx <= offset; kx++) {
                for (int ky = -offset; ky <= offset; ky++) {
                    int ny = ky + y;
                    int nx = kx + x;
                    if (nx < 0 || nx >= width || ny < 0 || ny >= height)
                        continue;
                    intensity = get_pixel_intensity(image, nx, ny);
                    window[k++] = intensity;
                }
            }
            qsort(window, 9, sizeof(float), compare);
            put_pixel(new_image, x, y, SDL_MapRGB(new_image->format, window[4], window[4], window[4]));
        }
    }
    return new_image;
}

SDL_Surface* resize(SDL_Surface* image, size_t factor){
    Uint8 r, g, b;
    Uint32 pixel;
    SDL_Surface* new_image;
    size_t height = image->h;
    size_t width = image->w;
    size_t new_width = width + factor;
    size_t new_height = height + factor;
    new_image = SDL_CreateRGBSurface(0, new_width, new_height, 32, 0, 0, 0, 0);
    SDL_FillRect(new_image, NULL, SDL_MapRGB(new_image->format, 255, 255, 255));

    for (size_t x = 0; x < width; x++){
        for (size_t y = 0; y < height; y++){
            pixel = get_pixel(image, x, y);
            SDL_GetRGB(pixel, image->format, &r, &g, &b);
            pixel = SDL_MapRGB(new_image->format, r, g, b);
            size_t new_x = round(x + factor / 2);
            size_t new_y = round(y + factor / 2);
            put_pixel(new_image, new_x, new_y, pixel);
        }
    }
    return new_image;
}

SDL_Surface* pre_processing(SDL_Surface* image) {
    SDL_Surface* new_image;
    gray_scale(image);
    contrast(image, 10);
    new_image = binarization(image, 10);
    new_image = median_filter(new_image, 3);
    new_image = auto_deskew(new_image);
    new_image = resize(new_image, 5);
    return new_image;
}