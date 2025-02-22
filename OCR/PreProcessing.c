#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <err.h>
#include <math.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "pixeloperations.h"
#include "PreProcessing.h"
#include "Kernels.h"
#include "sdl_libs.h"


double get_adaptive_threshold(SDL_Surface* image, size_t x, size_t y, int blocksize) {
    int count = 0, halfsize = blocksize / 2;
    double sum = 0;
    for (int i = -halfsize; i <= halfsize; i++) {
        for (int j = -halfsize; j <= halfsize; j++) {
            int new_x = x + i;
            int new_y = y + j;
            if (new_x >= 0 && new_x < image->w && new_y >= 0 && new_y < image->h) {
                sum += get_pixel_intensity(image, new_x, new_y);
                count++;
            }
        }
    }
    return (count > 0) ? sum / count : 0.0; 
}

double get_normalised_factor(int contrast_factor) {
    return (259 * (contrast_factor + 255)) / (255 * (259 - contrast_factor)); 
}

// Median sorting helper
int compare(const void *a, const void *b) {
    return (*(double *)a - *(double *)b);
}

double get_normalised_intensity(double intensity, double normalised_factor) {
    return (normalised_factor * intensity - 128) + 128;
}

double gaussian(double x, double sigma) {
    return exp(-(x * x) / (2 * sigma * sigma));
}

double get_variance(size_t arr[], size_t n){
	double sum = 0.0, sum_eq = 0.0;
	for (size_t i = 0; i < n; i++)
        sum += arr[i];

    for (size_t i = 0; i < n; i++)
        sum_eq += pow((arr[i] - sum / (double)n), 2);

    return sum_eq / (double)n;
}

SDL_Surface* compression(SDL_Surface* image, size_t new_height, size_t new_width) {
    double pixel_intensity;
    size_t height = image->h, width = image->w;
    double x_ratio = (double) width / new_width;
    double y_ratio = (double) height / new_height;

    SDL_Surface* new_image = SDL_CreateRGBSurface(image->flags, new_width, new_height, image->format->BitsPerPixel,
        image->format->Rmask, image->format->Gmask, image->format->Bmask, image->format->Amask);

    for (size_t x = 0; x < new_width; x++) {
        for (size_t y = 0; y < new_height; y++) {
            pixel_intensity = get_pixel_intensity(image, x * x_ratio, y * y_ratio);
            put_pixel(new_image, x, y, SDL_MapRGB(new_image->format, pixel_intensity, pixel_intensity, pixel_intensity));
        }
    }
    return new_image;
}

SDL_Surface* crop_image(SDL_Surface* image, int left_offset, int top_offset, int height, int width) {
    SDL_Surface* new_image = SDL_CreateRGBSurface(image->flags, width, height, image->format->BitsPerPixel,
        image->format->Rmask, image->format->Gmask, image->format->Bmask, image->format->Amask);
    SDL_Rect rect = {left_offset, top_offset, width, height};
    if (SDL_BlitSurface(image, &rect, new_image, NULL) != 0)
        errx(1, "Cant crop the image");
    return new_image;
}

SDL_Surface* gray_scale(SDL_Surface* image){
    size_t width = image->w, height = image->h;
    Uint32 pixel;
    Uint8 r, g, b, newcolor;
    SDL_Surface* new_image = SDL_CreateRGBSurface(image->flags, image->w, image->h, image->format->BitsPerPixel,
        image->format->Rmask, image->format->Gmask, image->format->Bmask, image->format->Amask);
    for (size_t x = 0; x < width; x++){
        for (size_t y = 0; y < height; y++){
            pixel = get_pixel(image, x, y);
            SDL_GetRGB(pixel, image->format, &r, &g, &b);
            newcolor = 0.21 * r + 0.72 * g + 0.07 * b;
            put_pixel(new_image, x, y, SDL_MapRGB(new_image->format, newcolor, newcolor, newcolor));
        }
    }
    return new_image;
}

SDL_Surface* contrast(SDL_Surface* image, int contrast_factor){
    Uint8 new_intensity;
    size_t width = image->w, height = image->h;
    double normalised_factor, intensity;
    SDL_Surface* new_image = SDL_CreateRGBSurface(image->flags, image->w, image->h, image->format->BitsPerPixel,
        image->format->Rmask, image->format->Gmask, image->format->Bmask, image->format->Amask);
    for (size_t x = 0; x < width; x++){
        for (size_t y = 0; y < height; y++) {
            normalised_factor = get_normalised_factor(contrast_factor);
            intensity = get_pixel_intensity(image, x, y);
            new_intensity = get_normalised_intensity(intensity, normalised_factor);
            put_pixel(new_image, x, y, SDL_MapRGB(new_image->format, new_intensity, new_intensity, new_intensity));
        }
    }
    return new_image;
}

SDL_Surface* binarization(SDL_Surface* image, int blocksize, int constant){
    Uint8 new_intensity;
    double threshold, intensity;
    size_t width = image->w, height = image->h;
    SDL_Surface* new_image = SDL_CreateRGBSurface(image->flags, image->w, image->h, image->format->BitsPerPixel,
        image->format->Rmask, image->format->Gmask, image->format->Bmask, image->format->Amask);
    for (size_t x = 0; x < width; x++) {
        for (size_t y = 0; y < height; y++) {
            threshold = get_adaptive_threshold(image, x, y, blocksize) - constant;
            intensity = get_pixel_intensity(image, x, y);
            new_intensity = intensity <= threshold ? 0 : 255;
            put_pixel(new_image, x, y, SDL_MapRGB(new_image->format, new_intensity, new_intensity, new_intensity));
        }
    }
    return new_image;
}

SDL_Surface* man_deskew(SDL_Surface* image, double angle){
    SDL_Surface* new_image = SDL_CreateRGBSurface(image->flags, image->w, image->h, image->format->BitsPerPixel,
        image->format->Rmask, image->format->Gmask, image->format->Bmask, image->format->Amask);
    SDL_FillRect(new_image, NULL, SDL_MapRGB(new_image->format, 255, 255, 255));

	double CX = (double) image->w / 2, CY = (double) image->h / 2;
	double X, Y, X2, Y2;
	double radians = (angle * 3.14) / 100;

	for(int y = 0; y < image->h; y++) {
        for(int x = 0; x < image->w; x++) {
			X = x - CX;
			Y = y - CY;
			X2 = (X * cos(radians) - Y * sin(radians));
			Y2 = (X * sin(radians) + Y * cos(radians));
			X2 += CX;
			Y2 += CY;
			if( X2 >= image->w || X2 < 0 || Y2 >= image->h || Y2 < 0) 
				continue;
			put_pixel(new_image, x, y, get_pixel(image, X2, Y2));
		}
	}
	return new_image;
}

double skew_degree(SDL_Surface* image, double precision){
	Uint32 pixel;
	size_t sum;
	size_t height = image->h, width = image->w;
	size_t* histogram;
	Uint8 r, g, b;
    SDL_Surface* tilted_copy;
	double max_variance = 0, max_variance_deg = 0, variance;
	double max = 45., min = -45.;
	for(; min < max; min += precision){
        histogram = calloc(height, sizeof(size_t));
		tilted_copy = man_deskew(image, min);
		for (size_t y = 0; y < height; y++){
			sum = 0;
			for(size_t x = 0; x < width; x++){
				pixel = get_pixel(tilted_copy, x, y);
				SDL_GetRGB(pixel, tilted_copy->format, &r, &g, &b);
				if (r == 0)
					sum +=1;
			}
			histogram[y]= sum;
		}
		variance = get_variance(histogram, height);		
		if (variance > max_variance){
			max_variance = variance;
			max_variance_deg = min;
		}
		SDL_FreeSurface(tilted_copy);
        free(histogram);
	}
	return max_variance_deg;
}

SDL_Surface* auto_deskew(SDL_Surface* image){
	return man_deskew(image, skew_degree(image, 0.25));
}

SDL_Surface* convolution(SDL_Surface* image, double kernel[], size_t kernel_size) {
    int offset = kernel_size / 2;
    double kernel_sum = 0;
    double new_intensity;
    size_t width = image->w, height = image->h;
    SDL_Surface* new_image = SDL_CreateRGBSurface(image->flags, image->w, image->h, image->format->BitsPerPixel,
        image->format->Rmask, image->format->Gmask, image->format->Bmask, image->format->Amask);

    for (size_t x = 0; x < kernel_size; x++) {
        for (size_t y = 0; y < kernel_size; y++)
            kernel_sum += kernel[x * kernel_size + y];
    }

    if (kernel_sum == 0) 
        kernel_sum = 1;

    for (size_t x = 0; x < width; x++) {
        for (size_t y = 0; y < height; y++) {
            double intensity = 0;
            for (int kx = -offset; kx <= offset; kx++) {
                for (int ky = -offset; ky <= offset; ky++) {
                    int ny = y + ky;
                    int nx = x + kx;
                    if (nx < 0 || nx >= width || ny < 0 || ny >= height)
                        continue;
                    intensity += get_pixel_intensity(image, nx, ny) * 
                                    kernel[(kx + offset) * kernel_size + (ky + offset)];                       
                }
            }
            new_intensity = intensity / kernel_sum;
            new_intensity = new_intensity < 0 ? 0 : (new_intensity > 255 ? 255 : new_intensity);
            put_pixel(new_image, x, y, SDL_MapRGB(new_image->format, new_intensity, new_intensity, new_intensity));
        }
    }
    return new_image;
}

SDL_Surface* median_filter(SDL_Surface* image, size_t kernel_size) {
    size_t window_size = kernel_size * kernel_size;
    double window[window_size];
    double intensity;
    int offset = kernel_size / 2;
    size_t width = image->w, height = image->h;
    SDL_Surface* new_image = SDL_CreateRGBSurface(image->flags, image->w, image->h, image->format->BitsPerPixel,
        image->format->Rmask, image->format->Gmask, image->format->Bmask, image->format->Amask);
    for (size_t x = 0; x < width; x++) {
        for (size_t y = 0; y < height; y++) {
            size_t k = 0;
            for (int kx = -offset; kx <= offset; kx++) {
                for (int ky = -offset; ky <= offset; ky++) {
                    int ny = ky + y;
                    int nx = kx + x;
                    if (nx < 0 || nx >= width || ny < 0 || ny >= height)
                        continue;
                    intensity = get_pixel_intensity(image, nx, ny);
                    window[k++] = intensity;
                }
            }
            qsort(window, window_size, sizeof(double), compare);
            double current_intensity = get_pixel_intensity(image, x, y);
            double new_intensity = window[window_size / 2];
            if (abs(new_intensity - current_intensity) > 10)
                put_pixel(new_image, x, y, SDL_MapRGB(new_image->format, window[window_size / 2], window[window_size / 2], window[window_size / 2]));
            else
                put_pixel(new_image, x, y, SDL_MapRGB(new_image->format, current_intensity, current_intensity, current_intensity));
        }
    }
    return new_image;
}

SDL_Surface* remove_white_edges(SDL_Surface* image) {
    double intensity;
    size_t height = image->h, width = image->w;
    size_t right_offset = 0, left_offset = width, top_offset = height, bottom_offset = 0;

    for (size_t x = 0; x < width; x++) {
        for (size_t y = 0; y < height; y++) {
            intensity = get_pixel_intensity(image, x, y);
            if (intensity == 0) {
                if (y < top_offset)
                    top_offset = y;
                if (y > bottom_offset)
                    bottom_offset = y;
                if (x < left_offset)
                    left_offset = x;
                if (x > right_offset)
                    right_offset = x;
            }
        }
    }
    if (top_offset > bottom_offset || left_offset > right_offset)
        right_offset = left_offset = top_offset = bottom_offset = 0;
    size_t new_width = right_offset - left_offset + 1;
    size_t new_height = bottom_offset - top_offset + 1;
    return crop_image(image, left_offset, top_offset, new_height, new_width);
}

// Fixes the aspect ratio to 1:1
SDL_Surface* fix_aspect_ratio(SDL_Surface* image) {
    double pixel_intensity;
    size_t height = image->h, width = image->w;
    size_t new_size = height > width ? height : width;
    SDL_Surface* new_image = SDL_CreateRGBSurface(image->flags, new_size, new_size, image->format->BitsPerPixel,
        image->format->Rmask, image->format->Gmask, image->format->Bmask, image->format->Amask);
    SDL_FillRect(new_image, NULL, SDL_MapRGB(new_image->format, 255, 255, 255));
    int offset_x = (new_size - width) / 2;
    int offset_y = (new_size - height) / 2;
    for (size_t x = 0; x < width; x++) {
        for (size_t y = 0; y < height; y++) {
            int new_x = x + offset_x;
            int new_y = y + offset_y;
            pixel_intensity = get_pixel_intensity(image, x, y);
            put_pixel(new_image, new_x, new_y, SDL_MapRGB(new_image->format, pixel_intensity, pixel_intensity, pixel_intensity));
        }
    }
    return new_image;
}

// Apply bilateral filter
SDL_Surface* bilateral_filter(SDL_Surface* image, int radius) {
    int ny, nx;
    double current_intensity, neighbour_intensity, new_intensity;
    double sum, weight_sum, spatial_weight, range_weight, weight;
    size_t width = image->w, height = image->h;
    SDL_Surface* new_image = SDL_CreateRGBSurface(image->flags, width, height, image->format->BitsPerPixel,
        image->format->Rmask, image->format->Gmask, image->format->Bmask, image->format->Amask);

    for (size_t x = 0; x < width; x++) {
        for (size_t y = 0; y < height; y++) {
            sum = 0, weight_sum = 0;
            current_intensity = get_pixel_intensity(image, x, y);
            for (int kx = -radius; kx <= radius; kx++) {
                for (int ky = -radius; ky <= radius; ky++) {
                    ny = y + ky;
                    nx = x + kx;
                    if (ny < 0 || ny >= height || kx < 0 || kx >= width)
                        continue;
                    neighbour_intensity = get_pixel_intensity(image, nx, ny);
                    spatial_weight = gaussian(sqrt(kx * kx + ky * ky), 2);
                    range_weight = gaussian(fabs(neighbour_intensity - current_intensity), 50);
                    weight = spatial_weight * range_weight;
                    sum += neighbour_intensity * weight;
                    weight_sum += weight;
                }
            }
            new_intensity = sum / weight_sum;
            put_pixel(new_image, x, y, SDL_MapRGB(new_image->format, new_intensity, new_intensity, new_intensity));
        }
    }
    return new_image;
}

SDL_Surface* pre_processing(SDL_Surface* image) {
    SDL_Surface* standard_image = convert_to_all_format(image);
    SDL_Surface* grey_scale_image = gray_scale(standard_image);
    SDL_Surface* filtered_image = bilateral_filter(grey_scale_image, 3);
    SDL_Surface* filtered_image_2 = bilateral_filter(filtered_image, 2);
    SDL_Surface* contrast_image = contrast(filtered_image_2, 12);
    SDL_Surface* binarized_image = binarization(contrast_image, 13, 10);
    //new_image = auto_deskew(new_image);
    SDL_Surface* removed_white_edges_image = remove_white_edges(binarized_image);

    SDL_FreeSurface(standard_image);
    SDL_FreeSurface(grey_scale_image);
    SDL_FreeSurface(filtered_image);
    SDL_FreeSurface(filtered_image_2);
    SDL_FreeSurface(contrast_image);
    SDL_FreeSurface(binarized_image);
    return removed_white_edges_image;
}