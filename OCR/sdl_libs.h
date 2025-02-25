#ifndef SDL_LIBS_H
#define SDL_LIBS_H

#include "SDL2/SDL.h"

int init_SDL();
SDL_Surface* get_image(char* path);
SDL_Surface* convert_to_all_format(SDL_Surface* image);
void display_image(SDL_Surface* image, int duration);
void save_image_to_bmp(SDL_Surface* image, const char* path);
#endif