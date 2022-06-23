#ifndef BLACK_WHITE_H
#define BLACK_WHITE_H

#include<stdlib.h>
#include"SDL/SDL.h"

void grayScale(SDL_Surface *img);
//float mean_value(SDL_Surface* image_surface);

//float automatic_threshold_value(SDL_Surface* image_surface, float limit);
//void binarization(SDL_Surface* image_surface, float limit);
void Binarise(SDL_Surface *img);
void Contrast(SDL_Surface *img, int threshold);

#endif
