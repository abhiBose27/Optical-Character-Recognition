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

double get_cost(double* targets, double* outputs, size_t nb_targets) {
    double cost = 0;
    for (size_t i = 0; i < nb_targets; i++) {
        cost += 0.5 * (targets[i] - outputs[i]) * (targets[i] - outputs[i]);
    }
    return cost;
}

void train_model(Network* network, size_t nb_targets) {
    double* targets;
    double* inputs;
    double* outputs;
    double cost;
    Training_set tr_set = prepare_training_dataset(nb_targets);
    printf("Training done with %ld\n", tr_set.nb_data);
    for (size_t epoch = 0; epoch < 2000; epoch++) {
        cost = 0;
        shuffle_dataset(tr_set.data, tr_set.nb_data);
        for (size_t i = 0; i < tr_set.nb_data; i++) {
            if (!tr_set.data[i].image)
                continue;
            
            targets = get_targets(tr_set.data[i].target, nb_targets);
            inputs = get_image_to_pixel_intensity_matrix(tr_set.data[i].image);
            outputs = feed_forward(network, inputs);
            cost += get_cost(targets, outputs, nb_targets);
            back_propagation(network, targets, inputs, 0.05);
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

void test_network(Network* network, size_t nb_targets) {
    size_t prediction;
    size_t nb_correct = 0;
    Training_set tr_set = prepare_training_dataset(52);
    shuffle_dataset(tr_set.data, tr_set.nb_data);
    for (size_t i = 0; i < tr_set.nb_data; i++) {
        if (!tr_set.data[i].image)
            continue;
        prediction = get_prediction(network, tr_set.data[i].image, nb_targets);
        if (prediction == tr_set.data[i].target)
            nb_correct += 1;
    }
    printf("Accuracy: %f\n", (double) nb_correct * 100 / tr_set.nb_data);
    free_dataset(&tr_set);
}

int main(int argc, char* argv[]) {
    srand (time(NULL));
    Network network = generate_network(3, 784, 52);   
    train_model(&network, 52);
    save_network(&network);
    free_network(&network);

    
    network = get_trained_network(3, 784, 52);
    test_network(&network, 52);
    free_network(&network);
    return 0;
}