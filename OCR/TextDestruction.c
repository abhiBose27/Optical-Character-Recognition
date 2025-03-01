#include "err.h"
#include "SdlLibs.h"
#include "PixelOperations.h"
#include "TextDestruction.h"
#include "PreProcessing.h"


double get_mean_space_in_line(SDL_Surface* image, size_t y1, size_t y2) {
    bool is_space_start = false;
    SDL_Surface* line_image = crop_image(image, 0, y1, y2 - y1, image->w);
    SDL_Surface* updated_line_image = remove_white_edges(line_image);
    size_t width = updated_line_image->w;
    size_t height = updated_line_image->h;
    size_t total_space = 0;
    size_t nb_space = 0;
    double* histogram = get_vertical_pixel_intensity_histogram(updated_line_image, 0.0, true, 0, height);
    double mean_density = get_mean_pixel_intensity_density(histogram, width);

    for (size_t x = 0; x < width; x++) {
        // Space detected
        if (histogram[x] < mean_density * 0.01) {
            if (!is_space_start)
                is_space_start = true;
            total_space++;
            continue;
        }
        // Space end
        if (is_space_start) {
            nb_space++;
            is_space_start = false;
        }
    }
    free(histogram);
    SDL_FreeSurface(line_image);
    SDL_FreeSurface(updated_line_image);
    return nb_space != 0 ? (total_space / nb_space) * 1.5 : 0.0;
}

// Draw lines in between words
void word_detection(SDL_Surface* image, size_t y1, size_t y2) {
    Uint32 color;
    size_t space = 0;
    size_t width = image->w;

    // Get black pixel vertical projection
    double* histogram = get_vertical_pixel_intensity_histogram(image, 0.0, true, y1, y2);

    // Get average vertical spaces in a line block
    double mean_space = get_mean_space_in_line(image, y1, y2);

    // Get mean pixel density over columns
    double mean_density = get_mean_pixel_intensity_density(histogram, width);
    
    for (size_t x = 0; x < width; x++){
        // Mark the space
        if (histogram[x] < mean_density * 0.01) {
            space++;
            continue;
        }

        // Space higher than mean space
        // Then Space between words.
        // Denote Red.
        // Else denote pink space between characters.
        color = space >= mean_space ? SDL_MapRGB(image->format, 255, 0, 0) : SDL_MapRGB(image->format, 255, 0, 255);
        for (size_t i = x - space; i < x; i++) {
            for (size_t j = y1; j < y2; j++)
                put_pixel(image, i, j, color);
        }
        space = 0;
    }
    if (space != 0) {
        for (size_t i = width - space; i < width; i++) {
            for (size_t j = y1; j < y2; j++)
                put_pixel(image, i, j, SDL_MapRGB(image->format, 255, 0, 0));
        }
    }
    free(histogram);
}

// Draw lines in between line blocks
void line_detection(SDL_Surface* image) {
    int y1 = -1;
    size_t width = image->w, height = image->h;
    
    // Get black pixel horizontal projection
    double* histogram = get_horizontal_pixel_intensity_histogram(image, 0.0, true, 0, width);

    // Get mean pixel density over rows
    double mean_density = get_mean_pixel_intensity_density(histogram, height);
    for (size_t y = 0; y < height; y++) {

        // Mark the row where the black pixels are higher than mean
        if (histogram[y] >= mean_density * 0.01) {
            if (y1 == -1)
                y1 = y;
            continue;
        }

        // Mark the row where the black pixels are lower than mean.
        // Found the end of the line block
        if (y1 != -1) {
            word_detection(image, y1, y);
            y1 = -1;
        }

        // Denote the horizontal space in red
        for (size_t x = 0; x < width; x++)
            put_pixel(image, x, y, SDL_MapRGB(image->format, 255, 0, 0));
    }
    if (y1 != -1)
        word_detection(image, y1, height);
    free(histogram);
}

void text_destruction(SDL_Surface* image) {
    line_detection(image);
}