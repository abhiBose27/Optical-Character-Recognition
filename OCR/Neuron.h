#ifndef _NEURON_
#define _NEURON_
struct Neuron
{
  double nb_weights;
	double *weights;
	double bias;
	double output;
  double dl_wrt_curr;
};

typedef struct Neuron Neuron;

double sigmoid(double x);

double sigmoid_derivative(double guess);

double ProcessNeuron(double *inputs, Neuron *N);

double randomnum();

Neuron GenerateNeuron(double nb_inputs);

#endif
