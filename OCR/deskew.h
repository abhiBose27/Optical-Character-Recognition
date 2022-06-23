#ifndef _DESKEW_H
#define _DESKEW_H
#include<math.h>
#include<stdlib.h>
#include"SDL/SDL.h" 
SDL_Surface* man_deskew(SDL_Surface* image_surface,double degree);
SDL_Surface* auto_deskew(SDL_Surface* img);
#endif

