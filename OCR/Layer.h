#ifndef LAYER_H
#define LAYER_H

#include "Neuron.h"

typedef struct Layer{
	Neuron* neurons;
	size_t nb_neurons;
	//double *output;
  	//double nb_outputs;
} Layer;

Layer generate_layer(size_t nb_neurons, size_t nb_inputs);
void process_layer(double* inputs, Layer* layer);
double* get_layer_output(Layer* layer);

#endif
