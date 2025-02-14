#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include <time.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "pixeloperations.h"
#include "err.h"
#include "Network.h"
#include "Neuron.h"
#include "Layer.h"
#include "sdl_libs.h"


/////Generates 3 layered Network ///////////
Network generate_network(size_t nb_layers, size_t nb_inputs, size_t nb_outputs) {
    Network network;
    network.nb_layers = nb_layers;
    network.layers = malloc(nb_layers * sizeof(Layer));
    double* neurons = malloc(nb_layers * sizeof(double));
    double* inputs = malloc(nb_layers * sizeof(double));

    // neurons = {256, 256, 52}
    // inputs = {784, 256, 256}
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

//////// Feed Forward ///// Called after every epoch //////
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

//// BACK PROPAGATION //////////////
/* void BackPropagation(Network* network, double* target, double* inputs)
{
  double error;
  double dl_dw;
  double dl_db;
  Neuron* neuron;
  Neuron* neuron1;

  int k = 2;
  while (k >= 0)
  {
    for (int i = 0; i < network.layers[k].nb_outputs; i++)
    {
      neuron = &network.layers[k].neurons[i];
      for (int j = 0; j < neuron->nb_weights; j++)
      {
        // If iterating over last layer
        if (k == 2)
        {
          error = -(target[i] - res[i]);
          dl_dw = error * sigmoid_derivative(neuron->output) * network.layers[k - 1].neurons[j].output;
          dl_db = error * sigmoid_derivative(neuron->output) * 1;
          
          neuron1 = &network.layers[k - 1].neurons[j];
          neuron1->dl_wrt_curr += neuron->weights[j] * sigmoid_derivative(neuron->output)  * error;

          neuron->weights[j] = neuron->weights[j] - 0.2 * dl_dw;
          neuron->bias = neuron->bias - 0.2 * dl_db;
        }
        else if (k > 0)
        {
          dl_dw = neuron->dl_wrt_curr * sigmoid_derivative(neuron->output) * network.layers[k - 1].neurons[j].output;
          dl_db = neuron->dl_wrt_curr * sigmoid_derivative(neuron->output) * 1;
          
          neuron1 = &network.layers[k-1].neurons[j];
          neuron1->dl_wrt_curr += neuron->weights[j] * sigmoid_derivative(neuron->output) * neuron->dl_wrt_curr;

          neuron->weights[j] = neuron->weights[j] - 0.2 * dl_dw;
          neuron->bias = neuron->bias - 0.2 * dl_db;
        }
        else
        {
          dl_dw = neuron->dl_wrt_curr * sigmoid_derivative(neuron->output) * inputs[j];
          dl_db = neuron->dl_wrt_curr * sigmoid_derivative(neuron->output) * 1;
          
          neuron->weights[j] = neuron->weights[j] - 0.2 * dl_dw;
          neuron->bias = neuron->bias - 0.2 * dl_db;
        }
      }

    }
    k--;

  }

}
 */

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

char get_prediction(Network* network, SDL_Surface* image) {
    char letters[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
    double* inputs = get_image_to_pixel_intensity_matrix(image);
    double* outputs = feed_forward(network, inputs);
    size_t index = 0;
    double result = outputs[index];
    for (size_t i = 1; i < 52; i++) {
        if (outputs[i] > result) {
            result = outputs[i];
            index = i;
        }
    }
    free(inputs);
    free(outputs);
    return letters[index];
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