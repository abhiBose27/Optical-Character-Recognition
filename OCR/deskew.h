#ifndef _DESKEW_H
#define _DESKEW_H
#include <stdlib.h>
#include "SDL2/SDL.h" 
SDL_Surface* man_deskew(SDL_Surface* image, float angle);
SDL_Surface* auto_deskew(SDL_Surface* image);
#endif

