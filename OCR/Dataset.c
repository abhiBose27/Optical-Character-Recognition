#include <dirent.h>
#include <regex.h>
#include <sys/stat.h>
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

char** get_training_sets(const char* entry_path, const char* pattern, size_t* nb_matches) {
    DIR *dir;
    struct dirent *entry;
    struct stat st;
    regex_t regex;
    size_t match_size = 10;
    char** matches = malloc(match_size * sizeof(char*));

    // Compile the regex pattern
    if (regcomp(&regex, pattern, REG_EXTENDED | REG_NOSUB) != 0) {
        fprintf(stderr, "Could not compile regex\n");
        return NULL;
    }

    // Open the directory
    dir = opendir(entry_path);
    if (!dir) {
        perror("opendir");
        regfree(&regex);
        return NULL;
    }

    // Iterate through directory entries
    while ((entry = readdir(dir)) != NULL) {
        char full_path[1024];

        // Ignore "." and ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(full_path, sizeof(full_path), "%s/%s", entry_path, entry->d_name);

        // Check if it's a directory
        if (stat(full_path, &st) == 0 && S_ISDIR(st.st_mode)) {
            // Check if it matches the regex pattern
            if (regexec(&regex, entry->d_name, 0, NULL, 0) == 0) {
                if (*nb_matches == match_size) {
                    match_size *= 2;
                    matches = realloc(matches, match_size * sizeof(char*));
                }
                matches[*nb_matches] = malloc(256 * sizeof(char));
                strcpy(matches[*nb_matches], entry->d_name);
                *(nb_matches)+=1;
            }
        }
    }

    // Cleanup
    closedir(dir);
    regfree(&regex);
    return matches;
}

Training_data prepare_training_data(char* path, size_t target) {
    Training_data tr_data;
    tr_data.image = get_image(path);
    tr_data.target = target;
    return tr_data;
}

Training_set prepare_training_dataset(size_t data_per_set) {
    if (data_per_set > 52)
        errx(1, "Not valid data per set");
    
    Training_set tr_set;
    size_t nb_sets = 0;
    char** training_sets = get_training_sets("./Training_sets", "Training_set*", &nb_sets);
    printf("Matched folders: %ld\n", nb_sets);
    
    if (!training_sets)
        errx(1, "No training images");
    
    tr_set.data = malloc(nb_sets * data_per_set * sizeof(Training_data));
    tr_set.nb_data = nb_sets * data_per_set;
    for (size_t i = 0; i < nb_sets; i++) {
        for (size_t j = 0; j < data_per_set; j++) {
            char file_path[256];
            snprintf(file_path, sizeof(file_path), "./Training_sets/%s/%ldchar.bmp", training_sets[i], j);
            tr_set.data[i * data_per_set + j] = prepare_training_data(file_path, j);
        }
    }
    
    // Cleanup
    for(size_t i = 0; i < nb_sets; i++)
        free(training_sets[i]);
    free(training_sets);

    return tr_set;
}

void free_dataset(Training_set* dataset) {
    for (size_t i = 0; i < dataset->nb_data; i++) {
        SDL_FreeSurface(dataset->data[i].image);
    }
    free(dataset->data);
}