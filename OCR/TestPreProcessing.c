#include <err.h>
#include "SdlLibs.h"
#include "PreProcessing.h"


int main(int argc, char* argv[]) {
    if (argc != 2)
        errx(1, "Invalid number of arguments.\n");
    if (init_SDL() == -1)
        errx(1, "Could not initialise SDL: %s.\n", SDL_GetError());
    SDL_Surface* image = get_image(argv[1]);
    if (!image)
        errx(1, "Cant get image from %s: %s", argv[1], SDL_GetError());
    SDL_Surface* new_image = pre_processing(image);
    display_image(new_image, 5000);
    SDL_FreeSurface(image);
    SDL_FreeSurface(new_image);
    SDL_Quit();
    return EXIT_SUCCESS;
}