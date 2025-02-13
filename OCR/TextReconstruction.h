#ifndef TEXT_RECONSTRUCTION_H
#define TEXT_RECONSTRUCTION_H

#include "SDL2/SDL.h"

typedef struct Result {
    char* string;
    size_t size;
    size_t index;
} Result;

void text_reconstruction(SDL_Surface* image);

#endif