#ifndef _LAYER_
#define _LAYER_
#include "Neuron.h"

typedef struct Layer
{
	Neuron *neurons;
	double *output;
  double nb_outputs;
}Layer;



Layer GenerateLayer(double nb_neurons, double nb_inputs);

double* ProcessLayer(double *input, Layer *l);

#endif
