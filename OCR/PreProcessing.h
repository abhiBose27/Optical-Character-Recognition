#ifndef PRE_PROCESSING_H
#define PRE_PROCESSING_H

#include "SDL2/SDL.h"

SDL_Surface* compression(SDL_Surface* image, size_t new_height, size_t new_width);
SDL_Surface* pre_processing(SDL_Surface* image);
SDL_Surface* remove_white_edges(SDL_Surface* image);
SDL_Surface* crop_image(SDL_Surface* image, int left_offset, int top_offset, int height, int width);
SDL_Surface* fix_aspect_ratio(SDL_Surface* image);

#endif