#include "PixelOperations.h"

static inline
Uint8* pixel_ref(SDL_Surface *surf, unsigned x, unsigned y){
    int bpp = surf->format->BytesPerPixel;
    return (Uint8*)surf->pixels + y * surf->pitch + x * bpp;
}

Uint32 get_pixel(SDL_Surface *surface, unsigned x, unsigned y){
    Uint8 *p = pixel_ref(surface, x, y);
    switch (surface->format->BytesPerPixel){
        case 1:
            return *p;

        case 2:
            return *(Uint16 *)p;

        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;

        case 4:
            return *(Uint32 *)p;
        
        default:
            return 0;
    }
}

double get_pixel_intensity(SDL_Surface* image, unsigned x, unsigned y) {
    Uint8 r, g, b;
    Uint32 pixel = get_pixel(image, x, y);
    SDL_GetRGB(pixel, image->format, &r, &g, &b);
    return (r + g + b) / 3;
}

double* get_image_to_pixel_intensity_matrix(SDL_Surface* image) {
    double pixel_intensity;
    size_t height = image->h, width = image->w;
    double* pixel_intensity_matrix = malloc(height * width * sizeof(double));
    for(size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x++) {
            pixel_intensity = get_pixel_intensity(image, x, y);
            if (pixel_intensity == 0)
                pixel_intensity_matrix[y * width + x] = 0.99;
            else
                pixel_intensity_matrix[y * width + x] = 0.01;
        }
    }
    return pixel_intensity_matrix;
}

void pretty_print_hist(double* histogram, size_t size) {
    printf("Histogram: ");
    for (size_t x = 0; x < size; x++)
        printf("%f ", histogram[x]);
    printf("\n");
}

double get_mean_pixel_intensity_density(double* histogram, size_t size) {
    double sum = 0;
    for (size_t x = 0; x < size; x++)
        sum += histogram[x];
    return size != 0 ? sum / size: 0.0;
}

double* get_horizontal_pixel_intensity_histogram(SDL_Surface* image, double pixel_intensity, bool density, size_t x1, size_t x2) {
    double intensity;
    size_t height = image->h;
    double* histogram = calloc(height, sizeof(double));
    for (size_t y = 0; y < height; y++) {
        for (size_t x = x1; x < x2; x++) {
            intensity = get_pixel_intensity(image, x, y);
            if (intensity == pixel_intensity)
                histogram[y]++; 
        }
        if (density)
            histogram[y] /= (x2 - x1);
    }
    return histogram;
}

// For a block of text line
double* get_vertical_pixel_intensity_histogram(SDL_Surface* image, double pixel_intensity, bool density, size_t y1, size_t y2) {
    double intensity;
    size_t width = image->w;
    double* histogram = calloc(width, sizeof(double));
    for (size_t x = 0; x < width; x++) {
        for (size_t y = y1; y < y2; y++) {
            intensity = get_pixel_intensity(image, x, y);
            if (intensity == pixel_intensity)
                histogram[x]++;
        }
        if (density)
            histogram[x] /= (y2 - y1);
    }
    return histogram;
}

void put_pixel(SDL_Surface *surface, unsigned x, unsigned y, Uint32 pixel){
    Uint8 *p = pixel_ref(surface, x, y);
    switch(surface->format->BytesPerPixel)
    {
        case 1:
            *p = pixel;
            break;

        case 2:
            *(Uint16 *)p = pixel;
            break;

        case 3:
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            }
            else
            {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
            break;

        case 4:
            *(Uint32 *)p = pixel;
            break;
    }
}
