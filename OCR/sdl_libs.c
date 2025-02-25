#include "err.h"
#include "SDL2/SDL_image.h"
#include "sdl_libs.h"


SDL_Surface* get_image(char* path){
    return IMG_Load(path);
}

int init_SDL(){
    return SDL_Init(SDL_INIT_VIDEO);    
}

SDL_Surface* convert_to_all_format(SDL_Surface* image) {
    return SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_RGBA32, 0);
}

void display_image(SDL_Surface* image, int duration){
    SDL_Window* window = SDL_CreateWindow("SDL2 Surface Example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, image->w, image->h, SDL_WINDOW_SHOWN);
    if (!window)
        errx(1, "Unable to create a window");
    SDL_Surface* windowSurface = SDL_GetWindowSurface(window);
    if (!windowSurface)
        errx(1, "Unable to createWindowSurface");
    if (SDL_BlitSurface(image, NULL, windowSurface, NULL) != 0)
        errx(1, "Cant BlitSurface");
    if (SDL_UpdateWindowSurface(window) != 0)
        errx(1, "Cant update");
    SDL_FreeSurface(windowSurface);
    SDL_Delay(duration);
    SDL_DestroyWindow(window);
}

void save_image_to_bmp(SDL_Surface* image, const char* path) {
    if (!image) return;
    if (SDL_SaveBMP(image, path) != 0) {
        SDL_Log("Failed to save BMP: %s", SDL_GetError());
        return;
    }
    return;
}
