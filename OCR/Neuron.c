#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <float.h>
#include "math.h"
#include <time.h>
#include "Neuron.h"


double sigmoid(double x)
{
  double result;
  result = 1 / (1 + exp(-x));
  return result;
}


// Does the derivation for activation function. Here sigmoid. Helps in back propagation
double sigmoid_derivative(double guess)
{
  double result;
  result = guess * (1.0 - guess);
  return result;
}


/*This function applies the basic calculations involved in a neural network to a specifc node
and returns an output of 'double' data type.*/

double ProcessNeuron(double *inputs, Neuron *N)
{
  for(size_t i = 0; i < N -> nb_weights; i++)
	{
    N -> output += N -> weights[i] * inputs[i];
	}
	N -> output += N -> bias;
	N -> output = sigmoid(N -> output);
  N -> dl_wrt_curr = 0;
  //free(N -> weights);
	return N -> output;
}

double randomnum()
{
  return (double)rand() / (double)RAND_MAX * 2.0 - 1.0;
}

Neuron GenerateNeuron(double nb_inputs)
{
	Neuron n;
  n.nb_weights = nb_inputs;
	n.output = 0;
  n.bias = randomnum();
  n.dl_wrt_curr = 0;
  n.weights = calloc(nb_inputs, sizeof(double));
  double* begin = n.weights;
  for (size_t i = 0 ; i < nb_inputs; i++)
  {
    begin[i] = randomnum();
    //printf("%f weight initialised\n", begin[i]);
  }
	return n;
}
