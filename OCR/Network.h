#ifndef _NETWORK_H_
#define _NETWORK_H_

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "Layer.h"
struct Network
{
  Layer layers[3];
  double *output;
};


typedef struct Network Network;

struct training_set
{
  SDL_Surface* img;
  double* target;
};

typedef struct training_set training_set;

void text_conversion(SDL_Surface** surfaces, int nb_characters);
void shuffle(training_set* array, size_t n);
void read_file(Layer l, int nb_bias, int nb_weigths, char* filename);

#endif