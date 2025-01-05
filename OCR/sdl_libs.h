#ifndef _SDL_LIBS_
#define _SDL_LIBS
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

SDL_Surface* get_image(char* path);
int init_SDL();
void display_image(SDL_Surface* image, int duration);
#endif