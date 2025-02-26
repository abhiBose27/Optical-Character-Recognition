#include "PixelOperations.h"
#include "err.h"
#include "Network.h"
#include "Neuron.h"
#include "Layer.h"
#include "SdlLibs.h"


char letters[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};

Network generate_network(size_t nb_layers, size_t nb_inputs, size_t nb_outputs) {
    Network network;
    network.nb_layers = nb_layers;
    network.layers = malloc(nb_layers * sizeof(Layer));
    double* neurons = malloc(nb_layers * sizeof(double));
    double* inputs = malloc(nb_layers * sizeof(double));

    // neurons = {128, 128, 52}
    // inputs = {784, 128, 128}
    for (size_t i = 0; i < nb_layers; i++) {
        neurons[i] = i == nb_layers - 1 ? nb_outputs : 128;
    }
    for (size_t i = 0; i < nb_layers; i++) {
        inputs[i] = i == 0 ? nb_inputs : 128;
    }
    for (size_t i = 0; i < nb_layers; i++) {
        network.layers[i] = generate_layer(neurons[i], inputs[i]);
    }
    free(neurons);
    free(inputs);
    return network;
}

double* feed_forward(Network* network, double* inputs) {
    double* layer_output = NULL;
    for(size_t i = 0; i < network->nb_layers; i++) {
        if (layer_output) {
            process_layer(layer_output, &(network->layers[i]));
            free(layer_output);
        }
        else
            process_layer(inputs, &(network->layers[i]));
        layer_output = get_layer_output(&(network->layers[i]));
    }
    return layer_output;
}

void back_propagation(Network* network, double* targets, double* inputs, double learning_rate) {
    double error, dE_dw, dE_db, derivative;
    Layer* current_layer;
    Neuron* current_neuron;
    Neuron* previous_neuron;
    bool last_layer, first_layer;
    for (int l = network->nb_layers - 1; l >= 0; l--) {
        current_layer = &network->layers[l];
        last_layer = l == network->nb_layers - 1;
        first_layer = l == 0;
        for (size_t i = 0; i < current_layer->nb_neurons; i++) {
            current_neuron = &current_layer->neurons[i];
            derivative = sigmoid_derivative(current_neuron->output);
            for (size_t j = 0; j < current_neuron->nb_weights; j++) {
                previous_neuron = first_layer ? NULL : &network->layers[l - 1].neurons[j];
                error           = last_layer ? (current_neuron->output - targets[i]) : current_neuron->dl_wrt_curr;
                
                dE_dw = -error * derivative * (previous_neuron ? previous_neuron->output : inputs[j]);
                dE_db = -error * derivative;

                if (!first_layer)
                    previous_neuron->dl_wrt_curr += current_neuron->weights[j] * derivative * error;
                
                current_neuron->weights[j] += learning_rate * dE_dw;
                current_neuron->bias       += learning_rate * dE_db;
            }
        }
    }    
}

void save_network(Network* network) {
    Layer* curr_layer;
    Neuron* neuron;
    FILE* file;
    for (size_t i = 0; i < network->nb_layers; i++) {
        curr_layer = &network->layers[i];
        char file_path[256];
        snprintf(file_path, sizeof(file_path), "./Model_data/%ld_layer_%ld_data", network->nb_layers, i);
        
        // Delete the previous data
        file = fopen(file_path, "w");
        fclose(file);
        
        file = fopen(file_path, "a");
        for (size_t j = 0; j < curr_layer->nb_neurons; j++) {
            neuron = &curr_layer->neurons[j];
            for (size_t k = 0; k < neuron->nb_weights; k++) {
                fprintf(file, "%f\n", neuron->weights[k]);
            }
            fprintf(file, "%f\n", neuron->bias);
        }
        fclose(file);
    }
}

Network get_trained_network(size_t nb_layer, size_t nb_inputs, size_t nb_outputs) {
    Layer* curr_layer;
    Neuron* neuron;
    FILE* file;
    Network network = generate_network(nb_layer, nb_inputs, nb_outputs);
    for (size_t i = 0; i < network.nb_layers; i++) {
        curr_layer = &network.layers[i];
        char file_path[256];
        snprintf(file_path, sizeof(file_path), "./Model_data/%ld_layer_%ld_data", network.nb_layers, i);
        file = fopen(file_path, "r");
        if (!file)
            errx(1, "No data present for a model");
        for (size_t j = 0; j < curr_layer->nb_neurons; j++) {
            neuron = &curr_layer->neurons[j];
            for (size_t k = 0; k < neuron->nb_weights; k++) {
                if (fscanf(file, "%lf", &neuron->weights[k]) != 1) {
                    fclose(file);
                    printf("Error: Incomplete data\n");
                    return network;
                }
            }
            if (fscanf(file, "%lf", &neuron->bias) != 1) {
                fclose(file);
                printf("Error: Incomplete data\n");
                return network;
            }
        }
        fclose(file);
    }
    return network;
}

size_t get_prediction(Network* network, double* inputs, size_t nb_targets) {
    double* outputs = feed_forward(network, inputs);
    size_t index = 0;
    double result = outputs[index];
    for (size_t i = 1; i < nb_targets; i++) {
        if (outputs[i] > result) {
            result = outputs[i];
            index = i;
        }
    }
    free(outputs);
    return index;
}  

void free_network(Network* network) {
    for (size_t i = 0; i < network->nb_layers; i++){
        Layer* curr = &network->layers[i];
        for (size_t j = 0; j < curr->nb_neurons; j++) {
            free(curr->neurons[j].weights);
        }
        free(curr->neurons);
    }
    free(network->layers);
}