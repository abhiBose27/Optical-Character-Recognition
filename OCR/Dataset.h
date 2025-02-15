#ifndef DATASET_H
#define DATASET_H

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

typedef struct Image_data {
    SDL_Surface* image;
    size_t target;
} Image_data;

typedef struct Training_set {
    Image_data* data;
    size_t nb_data;
} Training_set;

void shuffle_dataset(Image_data* list, size_t nb_data);
Training_set prepare_training_dataset(size_t data_per_set);
void free_dataset(Training_set* tr_set);

#endif