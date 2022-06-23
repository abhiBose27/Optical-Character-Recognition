
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include <time.h>
#include "Neuron.h"
#include "Layer.h"


Layer GenerateLayer(double nb_neurons, double nb_inputs) 
{
  Layer l;
  l.nb_outputs = nb_neurons;
  l.neurons = calloc(nb_neurons, sizeof(Neuron));
  Neuron* begin = l.neurons;
  for (size_t i = 0 ; i < nb_neurons; i++)
  {
    begin[i] = GenerateNeuron(nb_inputs);
  }
  return l;
}

double* ProcessLayer(double *input, Layer *l)
{
	l -> output = calloc(l -> nb_outputs, sizeof(double));
	double *begin = l -> output;
	for (size_t i = 0 ; i < l -> nb_outputs; i++)
	{
		begin[i] = ProcessNeuron(input, &(l -> neurons[i]));
	}	
	return l -> output;
}
