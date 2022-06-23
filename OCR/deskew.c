#include <stdio.h>
#include <stdlib.h>
#include"SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <math.h>
#include"pixeloperations.h"
#include"deskew.h"


SDL_Surface *man_deskew(SDL_Surface* Surface,double Angle){

	/*SDL_Surface* rotatedimage= rotozoomSurface(image_surface, degree,1.0,0);
	return rotatedimage;*/

    SDL_Surface* _ret = SDL_CreateRGBSurface(Surface->flags, Surface->w, Surface->h, Surface->format->BitsPerPixel,
        Surface->format->Rmask, Surface->format->Gmask, Surface->format->Bmask, Surface->format->Amask);

	double CX = Surface->w / 2, CY = Surface->h / 2; //Center coordinates of image, or close enough.
	double X, Y, X2, Y2;

	for(int y = 0; y < Surface->h; y++) {
        for(int x= 0; x < Surface->w; x++) {
			X = x - CX;
			Y = y - CY;
			X2 = (X * cos(Angle) - Y * sin(Angle));
			Y2 = (X * sin(Angle) + Y * cos(Angle));
			X2 += CX;
			Y2 += CY;
			if( X2 >= Surface->w || X2 < 0 || Y2 >= Surface->h || Y2 < 0) put_pixel(_ret, x, y, SDL_MapRGB(Surface->format, 255, 255, 255));
/*
			X = x - CX;
			Y = y - CY;
			X2 = (X * cos(Angle) + Y * sin(Angle));
			Y2 = (-X * sin(Angle) + Y * cos(Angle));
			X2 += CX;
			Y2 += CY;
			if( X2 >= 0 && X2 < Surface->w && Y2 >= 0 && Y2 < Surface->h) put_pixel(_ret, (Uint32)X2, (Uint32)Y2, get_pixel(Surface, x, y));*/
			else put_pixel(_ret,x,y,get_pixel(Surface,X2,Y2));
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
