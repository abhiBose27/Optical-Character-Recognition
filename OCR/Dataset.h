#ifndef DATASET_H
#define DATASET_H

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

typedef struct Training_data {
    SDL_Surface* image;
    size_t target;
} Training_data;

typedef struct Training_set {
    Training_data* data;
    size_t nb_data;
} Training_set;

void shuffle_dataset(Training_data* list, size_t nb_data);
Training_set prepare_training_dataset(size_t nb_sets, size_t data_per_set);
void free_dataset(Training_set* tr_set) ;

#endif