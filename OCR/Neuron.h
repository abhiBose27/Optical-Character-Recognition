#ifndef NEURON_H
#define NEURON_H
typedef struct Neuron {
  	size_t nb_weights;
	double* weights;
	double bias;
	double output;
  	double dl_wrt_curr;
} Neuron;

double randomnum();
double sigmoid(double x);
double sigmoid_derivative(double guess);
double get_neuron_output(Neuron* neuron);
void process_neuron(double* inputs, Neuron* neuron);
Neuron generate_neuron(size_t nb_inputs);

#endif
