#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <float.h>
#include "math.h"
#include "Neuron.h"


double sigmoid(double x) {
    return 1 / (1 + exp(-x));
}

// Does the derivation for activation function. Here sigmoid. Helps in back propagation
double sigmoid_derivative(double guess) {
    return guess * (1.0 - guess);
}

/*This function applies the basic calculations involved in a neural network to a specifc node
and returns an output of 'double' data type.*/
void process_neuron(double* inputs, Neuron* neuron) {
    for(size_t i = 0; i < neuron->nb_weights; i++) {
        neuron->output += neuron->weights[i] * inputs[i];
    }
	neuron->output += neuron->bias;
	neuron->output = sigmoid(neuron->output);
    neuron->dl_wrt_curr = 0;
}

double randomnum(){
    return (double)rand() / (double)RAND_MAX * 2.0 - 1.0;
}

double get_neuron_output(Neuron* neuron) {
    return neuron->output;
}

Neuron generate_neuron(size_t nb_inputs) {
    Neuron n;
    n.nb_weights = nb_inputs;
    n.output = 0;
    n.dl_wrt_curr = 0;
    n.bias = randomnum();
    n.weights = malloc(nb_inputs * sizeof(double));
    for (size_t i = 0; i < nb_inputs; i++){
        n.weights[i] = randomnum();
    }
    return n; 
}
