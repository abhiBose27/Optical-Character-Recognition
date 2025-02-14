#include <time.h>
#include "Network.h"
#include "pixeloperations.h"
#include "sdl_libs.h"
#include "PreProcessing.h"
#include "Dataset.h"


double* get_targets(size_t x, size_t nb_outputs) {
    double* target = calloc(nb_outputs, sizeof(double));
    for (size_t i = 0; i < nb_outputs; i++){
        if (i == x)
            target[i] = 0.99;
        else
            target[i] = 0.01;
    }
    return target;
}

void train_model(Network* network, size_t nb_data) {
    double* targets;
    double* inputs;
    double* outputs;
    double cost;
    Training_set tr_set = prepare_training_dataset(nb_data);
    printf("Training done with %ld\n", tr_set.nb_data);
    for (size_t epoch = 0; epoch < 1000; epoch++) {
        cost = 0;
        shuffle_dataset(tr_set.data, tr_set.nb_data);
        for (size_t i = 0; i < tr_set.nb_data; i++) {
            if (!tr_set.data[i].image)
                continue;
            
            targets = get_targets(tr_set.data[i].target, nb_data);
            inputs = get_image_to_pixel_intensity_matrix(tr_set.data[i].image);
            outputs = feed_forward(network, inputs);
            for (size_t i = 0; i < nb_data; i++) {
                cost += 0.5 * (targets[i] - outputs[i]) * (targets[i] - outputs[i]);
            }
            back_propagation(network, targets, inputs, 0.09);
            free(targets);
            free(inputs);
            free(outputs);
        }
        if (epoch % 10 == 0) {
            printf("Epoch: %ld, Loss: %f\n", epoch, cost / tr_set.nb_data);
        }
    }
    free_dataset(&tr_set);
}

int main(int argc, char* argv[]) {
    srand (time(NULL));
    Network network = generate_network(3, 784, 52);   
    train_model(&network, 52);
    save_network(&network);
    free_network(&network);

    
    /* network = get_trained_network(3, 784, 52);
    SDL_Surface* image = get_image("./Training_images4/7char.bmp");
    double* inputs = get_image_to_pixel_intensity_matrix(image);
    double* outputs = feed_forward(&network, inputs);
    for (size_t j = 0; j < 52; j++) {
        printf("%f: %ld\n", outputs[j], j);
    }
    free(outputs);
    free(inputs);
    SDL_FreeSurface(image);
    free_network(&network); */
    return 0;
}