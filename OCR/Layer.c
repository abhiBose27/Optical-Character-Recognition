#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include <time.h>
#include "Neuron.h"
#include "Layer.h"


// Generate a layer of neurons
Layer generate_layer(size_t nb_neurons, size_t nb_inputs) {
    Layer l;
    l.nb_neurons = nb_neurons;
    l.neurons = malloc(nb_neurons * sizeof(Neuron));
    for (size_t i = 0 ; i < nb_neurons; i++) {
        l.neurons[i] = generate_neuron(nb_inputs);
    }
    return l;
}

void process_layer(double* inputs, Layer* layer) {
    for (size_t i = 0; i < layer->nb_neurons; i++) {
        process_neuron(inputs, &(layer->neurons[i]));
    }
}

double* get_layer_output(Layer* layer) {
    double* output = malloc(layer->nb_neurons * sizeof(double));
    for (size_t i = 0; i < layer->nb_neurons; i++) {
        output[i] = get_neuron_output(&(layer->neurons[i]));
    }
    return output;
}
