#ifndef _SDL_LIBS_
#define _SDL_LIBS
#include"SDL/SDL.h"
#include"SDL/SDL_image.h"
SDL_Surface* load_image(char* path);
void init_SDL();
SDL_Surface* displayImage(SDL_Surface *img);
void wait_for_keypressed();
#endif