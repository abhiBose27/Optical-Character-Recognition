#ifndef BLACK_WHITE_H
#define BLACK_WHITE_H

#include <stdlib.h>
#include "SDL2/SDL.h"

void gray_scale(SDL_Surface* image);
void binarization(SDL_Surface* image, int blocksize);
void contrast(SDL_Surface* image, int contrast_factor);

#endif
