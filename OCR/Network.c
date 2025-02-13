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
        if (layer_output)
            process_layer(layer_output, &(network->layers[i]));
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

/* 
/// Train The mode. Preparation for prediction
void Train_model(double* inputs, double* target, Network* network, size_t iterations)
{
  for (size_t i = 0; i < iterations; i++)
  {
    double* res = FeedForward(inputs, network);
    BackPropagation(network, target, inputs, res);

  }

}

void Train_Network(Network net, training_set* set, int iterations)
{
  for (int i = 0; i < iterations; i++)
  {
    shuffle(set, 52);
    for (int j = 0; j < 52; j++)
    {
      Train_model(pixel_values(set[j].img), set[j].target, net, 200);
    }
    printf("epoch done\n");

  }
  printf("done\n");

}


/////////// Prediction Function /////////////
char Prediction(double* result)
{
    char letters[52] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
                              'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
    //char letters_upper[26] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
    double res = 0;
    size_t index = 0;
    for (size_t i = 0; i < 52; i++)
    {
      if (result[i] > res)
      {

          res = result[i];
          index = i;
      }
    }
    //strcat(res, '0');
    return letters[index];
}

//////////TEXT CONVERSION //////////
void text_conversion(SDL_Surface** surfaces, int nb_characters)
{
    Network net = GenerateNetwork();
    read_file(net.layers[0], 80, 80*784, "training_data/11_layer_1data");
    read_file(net.layers[1], 80, 80*80, "training_data/11_layer_2data");
    read_file(net.layers[2], 52, 52*80, "training_data/11_layer_3data");
    //char letters[26] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
    double* temp;
    char *result = calloc(nb_characters+1, sizeof(char));
    FILE *file;
    file = fopen("Result", "w");
    for (int i = 0; i < nb_characters; i++)
    {
      temp = FeedForward(pixel_values(surfaces[i]), net);
      result[i] = Prediction(temp);
    }
    result[nb_characters] = '\n';
    fprintf(file, "%s", result);
    fclose(file);
    free(result);
}

////////////Saving weights and biases in a txt file///////////

void write_file(Layer layer, char* filename)
{
  FILE *file;
  Neuron* neuron;
  file = fopen(filename, "a");
  for (size_t i = 0; i < layer.nb_outputs; i++)
  {
    neuron = &layer.neurons[i];
    for (size_t j = 0; j < neuron->nb_weights; j++)
    {
      fprintf (file, "%f\n", neuron->weights[j]);
    }
    fprintf(file, "%f\n", neuron->bias);
  }
  fclose(file);
}

////////////reading weights and biases in a txt file///////////
void read_file(Layer l, int nb_bias, int nb_weigths, char* filename)
{
  FILE *file;
  Neuron* neuron;
  file = fopen(filename, "r");
  int r = nb_bias + nb_weigths;
  double data[r];
  int i = 0;
  while (!feof(file))
  {
    fscanf(file, "%lf", &data[i]);
    i += 1;
    //printf("%i\n", x);
  }
  fclose(file);
  i = 0;
  for (size_t j = 0; j < l.nb_outputs; j++)
  {
    neuron = &l.neurons[j];
    //printf(" neuron %d\n", i);
    for (size_t k = 0; k < neuron->nb_weights; k++)
    {
      neuron->weights[k] = data[i];
      i += 1;
    }
    neuron->bias = data[i];
    i += 1;
  }

}
//////////////////////////////////////////



///// Print the guess outputs ////////

void _Print(double* results)
{
  for (size_t i = 0; i < 52; i++)
  {
    printf("%f\n", results[i]);
  }
}

//// Array of training images ///////////////
training_set* image_set1(double** targets)
{
  training_set* set = calloc(52, sizeof(training_set));

  for (int i = 0; i < 52; i++)
  {
    char number_str[10];
    char filename[100] = "Training_set2/";
    sprintf(number_str, "%d", i);
    strcat(filename, number_str);
    strcat(filename, "char.bmp");
    set[i].img = get_image(filename);
    set[i].target = targets[i];
    
  }
  printf("initialsed\n");
  return set;
  
}

SDL_Surface** _Prediction_Set()
{
  SDL_Surface** validation = calloc(68, sizeof(SDL_Surface*));
  for (int i = 0; i < 68; i++)
  {
    char number_str[10];
    char filename[100] = "Validation_set3/";
    sprintf(number_str, "%d", i);
    strcat(filename, number_str);
    strcat(filename, "char.bmp");
    validation[i] = get_image(filename);
  }
  return validation;
  

}


//////////////////shuffling function////////////////////////

void shuffle(training_set* array, size_t n)
{
    if (n > 1)
    {
        size_t i;
        for (i = n-1; i > 0; i--)
        {
          size_t j = rand() % (i+1);
          training_set t = array[i];
          array[i] = array[j];
          array[j] = t;
        }
    }
}

///////// Generates the targets /////////////////////////
double* gen_a_target(int x)
{
  double* target = calloc(52, sizeof(double));

  for (int i = 0; i < 52; i++)
  {
    if (i == x)
      target[i] = 1;
    else
      target[i] = 0;
  }
  return target;
  

}

double** gen_targets()
{
  double** list_targets = calloc(52, sizeof(double*));

  for (int i = 0; i < 52; i++)
  {
    list_targets[i] = gen_a_target(i);
  }
  return list_targets;
  
}
*/