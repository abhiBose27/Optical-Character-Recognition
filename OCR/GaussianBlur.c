#include <err.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <stdlib.h>
#include<stdbool.h>
#include "pixeloperations.h"
#include "GaussianBlur.h"


double gauss[9] =    // guauss kernel dim 3 (approximation), calculated ahead of time
{
    1./16., 1./8., 1./16.,
    
    1./8., 1./4., 1./8.,
    
    1./16., 1./8., 1./16.
};

double sharpen[9] = {


    0., -1., 0.,
    
    -1., 5., -1.,
    
    0., -1., 0.
};

double edge_detection[9] = {



     0. -1., 0.,
    
    -1., 4., -1.,
    
    0., -1., 0.
};


Uint8 clamp(double n){

	if (n>255)
		n= 255;
	else if (n<0)
		n= 0;
	return n;
}

void convolute(SDL_Surface* image_surface, double m[], size_t cols){ 
       	//intented for kernel of dim 3
    size_t width = image_surface->w;
    size_t height = image_surface->h;
    Uint8 r, g, b;
    double r1,g1,b1;
    Uint32 pixel;
    SDL_Surface *image_surface_copy  = SDL_ConvertSurface(image_surface, image_surface->format, SDL_SWSURFACE);
    for (size_t i = 1; i < height-1; i++)
    {
        for (size_t j = 1; j < width-1; j++)
        {
            r1 = g1 = b1 = 0;
            for (int x = -1; x <= 1; x++)
            {
                for (int y = -1; y <= 1; y++)
                {
                    pixel = get_pixel(image_surface_copy,y+j,x+i);
                    SDL_GetRGB(pixel, image_surface_copy->format, &r, &g, &b);
                    r1 += (r * m[(x+1)*cols + y+1]);
                    g1 += (g * m[(x+1)*cols + y+1]);
                    b1 += (b * m[(x+1)*cols + y+1]);
                }
            }
	        pixel = SDL_MapRGB(image_surface->format, clamp(r1), clamp(g1), clamp(b1));
            put_pixel(image_surface,j,i,pixel);
        }
    }

    for (size_t i = 0; i < width; i++)
    {
        pixel = SDL_MapRGB(image_surface->format, 255, 255, 255);
        put_pixel(image_surface, i, 0, pixel);
        put_pixel(image_surface, i, height-1, pixel);
    }

    for (size_t i = 0; i < height; i++)
    {
        pixel = SDL_MapRGB(image_surface->format, 255, 255, 255);
        put_pixel(image_surface, 0, i, pixel);
        put_pixel(image_surface, width - 1, i, pixel);
    
    }

   SDL_FreeSurface(image_surface_copy); 
    
    //possibility to crop image, with white contour this won't matter though
    
}

void Convolution(SDL_Surface *img)
{
   // SDL_Surface* s;
    convolute(img, gauss, 3);
    //return s;
}
