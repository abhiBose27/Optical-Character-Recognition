#ifndef NETWORK_H
#define NETWORK_H

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "Layer.h"
typedef struct Network {
    Layer* layers;
    size_t nb_layers;
} Network;

extern char letters[];
Network generate_network(size_t nb_layers, size_t nb_inputs, size_t nb_outputs);
double* feed_forward(Network* network, double* inputs);
void back_propagation(Network* network, double* targets, double* inputs, double learning_rate);
void save_network(Network* network);
Network get_trained_network(size_t nb_layer, size_t nb_inputs, size_t nb_outputs);
size_t get_prediction(Network* network, SDL_Surface* image, size_t nb_targets);
void free_network(Network* network);

#endif