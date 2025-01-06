#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "pixeloperations.h"
#include "deskew.h"


SDL_Surface* man_deskew(SDL_Surface* image, float angle){
    SDL_Surface* _ret = SDL_CreateRGBSurface(image->flags, image->w, image->h, image->format->BitsPerPixel,
        image->format->Rmask, image->format->Gmask, image->format->Bmask, image->format->Amask);

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
				put_pixel(_ret, x, y, SDL_MapRGB(image->format, 255, 255, 255));
			else 
				put_pixel(_ret, x, y, get_pixel(image, X2, Y2));
		}
	}
	return _ret;
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
	for(size_t i = 0; i<n;i++)
		arr[i] = 0;
}

float skew_deg(SDL_Surface* image, float precision){
	Uint32 pixel;
	size_t sum;
	size_t height = image->h, width = image->w;
	size_t hist[height];
	Uint8 r,g,b;
	float max_variance = 0, max_variance_deg = 0, variance;
	float max = 45., min = -45.;
	for(; min<max; min += precision){
		SDL_Surface* tilted_copy = man_deskew(image, min);
		for (size_t y = 0; y < height; y++){
			sum = 0;
			for(size_t x = 0; x < width; x++){
				pixel = get_pixel(tilted_copy, x, y);
				SDL_GetRGB(pixel, tilted_copy->format, &r, &g, &b);
				if (r== 0)
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
	return man_deskew(image, skew_deg(image, 0.25));
}
