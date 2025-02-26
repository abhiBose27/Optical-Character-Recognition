#include "SdlLibs.h"
#include "PixelOperations.h"
#include "TextReconstruction.h"
#include "PreProcessing.h"
#include "Network.h"

//int i = 0;

void add_character(Result* result, char character) {
    if (strlen(result->string) == result->size - 1) {
        result->size *= 2;
        result->string = realloc(result->string, result->size * sizeof(char));
    }
    result->string[result->index] = character;
    result->index += 1;
}

void extract_characters(Network* network, SDL_Surface* image, size_t x1, size_t x2, Result* result) {
    SDL_Surface* character_image = crop_image(image, x1, 0, image->h, x2 - x1);
    SDL_Surface* updated_character_image = remove_white_edges(character_image);
    SDL_Surface* fixed_aspect_ratio = fix_aspect_ratio(updated_character_image);
    SDL_Surface* compressed_image = compression(fixed_aspect_ratio, 28, 28);
    double* inputs = get_image_to_pixel_intensity_matrix(compressed_image);
    //char file_path[256];
    //snprintf(file_path, sizeof(file_path), "./Training_sets/Training_set8/%dchar.bmp", i);
    //save_image_to_bmp(compressed_image, file_path);
    // Run the characters through ML
    add_character(result, letters[get_prediction(network, inputs, 52)]);
    //display_image(compressed_image, 2000);
    //printf("%d\n", result);
    SDL_FreeSurface(updated_character_image);
    SDL_FreeSurface(character_image);
    SDL_FreeSurface(compressed_image);
    SDL_FreeSurface(fixed_aspect_ratio);
    free(inputs);
    //i++;
}

void character_segmentation(Network* network, SDL_Surface* image, size_t x1, size_t x2, Result* result) {
    int word_x1 = -1;
    SDL_Surface* word_image = crop_image(image, x1, 0, image->h, x2 - x1);
    size_t width = word_image->w, height = word_image->h;
    double* histogram = get_vertical_pixel_intensity_histogram(word_image, 0.0, true, 0, height);
    double mean_density = get_mean_pixel_intensity_density(histogram, width);
    
    for (size_t x = 0; x < width; x++) {
        if (histogram[x] >= mean_density * 0.1) {
            if (word_x1 == -1)
                word_x1 = x;
            continue;
        }
        if (word_x1 != -1) {
            extract_characters(network, word_image, word_x1, x, result);
            word_x1 = -1;
        }
    }
    if (word_x1 != -1)
        extract_characters(network, word_image, word_x1, width, result);
    
    add_character(result, ' ');
    free(histogram);
    SDL_FreeSurface(word_image);
}

void word_segmentation(Network* network, SDL_Surface* image, size_t y1, size_t y2, Result* result) {
    int x1 = -1;
    SDL_Surface* line_image = crop_image(image, 0, y1, y2 - y1, image->w);
    size_t width = line_image->w, height = line_image->h;
    double* histogram = get_vertical_pixel_intensity_histogram(line_image, 85.0, false, 0, height);
    for (size_t x = 0; x < width; x++) {
        if (histogram[x] == 0) {
            if (x1 == -1)
                x1 = x;
            continue;
        }
        if (x1 != -1) {
            character_segmentation(network, line_image, x1, x, result);
            x1 = -1;
        }
    }
    if (x1 != -1)
        character_segmentation(network, line_image, x1, width, result);
    
    add_character(result, '\n');
    free(histogram);
    SDL_FreeSurface(line_image);
}

char* line_segmentation(SDL_Surface* image) {
    int y1 = -1;
    Network network = get_trained_network(3, 784, 52);
    Result result;
    result.size = 256;
    result.index = 0;
    result.string = calloc(result.size, sizeof(char));
    size_t width = image->w, height = image->h;
    double* histogram = get_horizontal_pixel_intensity_histogram(image, 0.0, false, 0, width);
    for (size_t y = 0; y < height; y++) {
        if (histogram[y] != 0) {
            if (y1 == -1)
                y1 = y;
            continue;
        }
        if (y1 != -1) {
            word_segmentation(&network, image, y1, y, &result);
            y1 = -1;
        }
    }
    if (y1 != -1)
        word_segmentation(&network, image, y1, height, &result);
        
    free(histogram);
    free_network(&network);
    return result.string;
}

char* text_reconstruction(SDL_Surface* image) {
    return line_segmentation(image);
}
