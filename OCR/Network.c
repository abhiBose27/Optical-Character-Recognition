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

////////////////////// SDL //////////////////////////////





double* pixel_values(SDL_Surface* img)
{
  Uint8 pixel;
  Uint8 r, g, b;
  int height = img->h;
  int width = img->w;
  double* pixel_val = calloc(784, sizeof(double));
  for (int i = 0; i < height; i++)
  {
    for (int j = 0; j < width; j++)
    {
      pixel = get_pixel(img, j, i);
      SDL_GetRGB(pixel, img->format, &r, &g, &b);
      if (r == 0 && g == 0 && b == 0)
        pixel_val[i * 28 + j] = 1;
      else
        pixel_val[i * 28 + j] = 0;
    }

  }
  return pixel_val;

}

//////////////////////////////////////////////////////////


/////Generates 3 layered Network ///////////
Network GenerateNetwork()
{
  double nb_neurons[3] = {80, 80, 52};
  double nb_inputs[3] = {784, 80, 80};
  Network net;
  for (size_t i = 0; i < 3; i++)
  {
    net.layers[i] = GenerateLayer(nb_neurons[i], nb_inputs[i]);
  }

  return net;
}

//////// Feed Forward ///// Called after every epoch //////
double* FeedForward(double* input, Network net)
{
  net.output = input;
  for(size_t i = 0; i < 3; i++)
  {
    net.output = ProcessLayer(net.output, &net.layers[i]);
  }
  return net.output;
}




//// BACK PROPAGATION //////////////
void BackPropagation(Network net, double* target, double* inputs, double* res)
{
  double error;
  double dl_dw;
  double dl_db;
  Neuron* neuron;
  Neuron* neuron1;

  int k = 2;
  while (k >= 0)
  {
    for (int i = 0; i < net.layers[k].nb_outputs; i++)
    {
      neuron = &net.layers[k].neurons[i];
      for (int j = 0; j < neuron->nb_weights; j++)
      {
        // If iterating over last layer
        if (k == 2)
        {
          error = -(target[i] - res[i]);
          dl_dw = error * sigmoid_derivative(neuron->output) * net.layers[k - 1].neurons[j].output;
          dl_db = error * sigmoid_derivative(neuron->output) * 1;
          
          neuron1 = &net.layers[k - 1].neurons[j];
          neuron1->dl_wrt_curr += neuron->weights[j] * sigmoid_derivative(neuron->output)  * error;

          neuron->weights[j] = neuron->weights[j] - 0.2 * dl_dw;
          neuron->bias = neuron->bias - 0.2 * dl_db;
        }
        else if (k > 0)
        {
          dl_dw = neuron->dl_wrt_curr * sigmoid_derivative(neuron->output) * net.layers[k - 1].neurons[j].output;
          dl_db = neuron->dl_wrt_curr * sigmoid_derivative(neuron->output) * 1;
          
          neuron1 = &net.layers[k-1].neurons[j];
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


/// Train The mode. Preparation for prediction
void Train_model(double* inputs, double* target, Network net, size_t iterations)
{
  for (size_t i = 0; i < iterations; i++)
  {
    double* res = FeedForward(inputs, net);
    BackPropagation(net, target, inputs, res);

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



