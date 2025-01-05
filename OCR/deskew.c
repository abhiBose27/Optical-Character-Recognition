#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "pixeloperations.h"
#include "deskew.h"


SDL_Surface* man_deskew(SDL_Surface* image, double angle){
    SDL_Surface* _ret = SDL_CreateRGBSurface(image->flags, image->w, image->h, image->format->BitsPerPixel,
        image->format->Rmask, image->format->Gmask, image->format->Bmask, image->format->Amask);

	double CX = image->w / 2, CY = image->h / 2; //Center coordinates of image, or close enough.
	double X, Y, X2, Y2;
	double radians = (angle * 3.14) / 100;

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

double variance(size_t arr[],size_t n)
{

	int sum = 0;
	int sum1 = 0;
	for (size_t i = 0; i < n; i++)
    {
        sum = sum + arr[i];
    }
    double average = sum / (double)n;

    for (size_t i = 0; i < n; i++)
    {
        sum1 = sum1 + pow((arr[i] - average), 2);
    }

    return (sum1 / (double)n);
}

void init_hist (size_t arr[],size_t n){

for(size_t i = 0; i<n;i++)
{
	arr[i] = 0;
}

}
double skew_deg(SDL_Surface* image, float precision){
	size_t h = image->h;
	size_t w = image->w;
	size_t hist[h];
	size_t sum;
	Uint8 r,g,b;
	double max_var = 0;
	double max_var_deg=0;
	double var;
	double max = 45.;
	double min = -45.;
	Uint32 pixel;
	for(; min<max; min+= precision){
		SDL_Surface *tilted_copy = man_deskew(image,min);
		for (size_t i=0; i<h;i++){
			sum = 0;
				for(size_t j=0; j<w; j++){
					pixel = get_pixel(tilted_copy,j,i);
					SDL_GetRGB(pixel, tilted_copy->format, &r, &g, &b);
					if (r== 0)
						sum +=1;
				}
			hist[i]= sum;
		}
		var = variance(hist, h);
		
		if (var >max_var){
			max_var = var;
			max_var_deg = min;
		}
		SDL_FreeSurface(tilted_copy);
		init_hist(hist,h);
}
	return max_var_deg;
}

SDL_Surface *auto_deskew(SDL_Surface* img)
{
return man_deskew(img, skew_deg(img,0.25));

}
