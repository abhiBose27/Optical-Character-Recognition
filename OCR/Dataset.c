#include <err.h>
#include "Dataset.h"
#include "PreProcessing.h"
#include "sdl_libs.h"


void shuffle_dataset(Training_data* list, size_t nb_data) {
    if (nb_data <= 1)
        return;
    for (size_t i = nb_data - 1; i > 0; i--) {
        size_t j = rand() % (i + 1);
        Training_data tmp = list[i];
        list[i] = list[j];
        list[j] = tmp;
    }
}

Training_data prepare_training_data(char* path, size_t target) {
    Training_data tr_data;
    tr_data.image = get_image(path);
    tr_data.target = target;
    return tr_data;
}

Training_set prepare_training_dataset(size_t nb_sets, size_t data_per_set) {
    if (data_per_set > 52)
        errx(1, "Not valid data per set");
    Training_set tr_set;
    tr_set.data = malloc(nb_sets * data_per_set * sizeof(Training_data));
    tr_set.nb_data = nb_sets * data_per_set;
    size_t training_set_tags[] = {1, 2, 3, 4, 5, 6};
    for (size_t i = 0; i < nb_sets; i++) {
        for (size_t j = 0; j < data_per_set; j++) {
            char file_path[256];
            snprintf(file_path, sizeof(file_path), "./Training_images%ld/%ldchar.bmp", training_set_tags[i], j);
            tr_set.data[i * data_per_set + j] = prepare_training_data(file_path, j);
        }
    }
    return tr_set;
}

void free_dataset(Training_set* dataset) {
    for (size_t i = 0; i < dataset->nb_data; i++) {
        SDL_FreeSurface(dataset->data[i].image);
    }
    free(dataset->data);
}