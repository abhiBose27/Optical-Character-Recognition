#include"SDL/SDL.h"
#include"SDL/SDL_image.h"
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include "pixeloperations.h"
#include<err.h>
#include "BlackAndWhite.h"
#include <math.h>



/// Does the Black and white aspect of the images
void grayScale(SDL_Surface* image_surface)
{

    int width = image_surface->w;
    int height = image_surface->h;
    Uint8 r, g, b, newcolor;
    Uint32 pixel;

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            pixel = get_pixel(image_surface,x,y);
            SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
            newcolor = 0.3*r + 0.59*g + 0.11*b;
            r = g = b = newcolor;
            pixel = SDL_MapRGB(image_surface->format, r, g, b);
            put_pixel(image_surface,x,y,pixel);



        }
        
    }
}


float mean_value(SDL_Surface* image_surface)  // image here has been grayscaled
{

    int width = image_surface->w;
    int height = image_surface->h;
    Uint32 pixel;
    Uint8 r, g, b;
    int res = 0;
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            pixel = get_pixel(image_surface,x,y);
            SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
            res+= r;
        }
        
    }
    return res / (width * height);    //mean value
}


float automatic_threshold_value(SDL_Surface* image_surface, float limit)  // image here has been grayscaled
{

    float old_thre=0;
    float new_thre = mean_value(image_surface);
    int width = image_surface->w;
    int height = image_surface->h;
    Uint32 pixel;
    Uint8 r, g, b;
    float count1, count2, total1, total2;

    while(old_thre - new_thre >= limit || old_thre - new_thre <= -limit)
    {
        old_thre = new_thre;
        count1 = count2 = total1 = total2 = 0;
        

        for (int x = 0; x < width; x++){
            for (int y = 0; y < height; y++){

            pixel = get_pixel(image_surface,x,y);
            SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
            if(r <= old_thre){
                count1++;
                total1 += r;
            }
            else{
                count2++;
                total2 += r;

            }


            }
        }
        new_thre = ((total1/count1)+(total2/count2))/2.;

    }
    return old_thre;
}






void binarization(SDL_Surface* image_surface, float limit)
{

    int width = image_surface->w;
    int height = image_surface->h;
    Uint8 r, g, b;
    Uint32 pixel;
    //grayscale(image_surface);
    int threshold = automatic_threshold_value(image_surface,limit);
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            pixel = get_pixel(image_surface,x,y);
            SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
            if(r < threshold){
                r = g = b = 0;
            }

            else{
                r = g = b = 255;
            }
            pixel = SDL_MapRGB(image_surface->format, r, g, b);
            put_pixel(image_surface,x,y,pixel);


        }
        
    }



}

void Contrast(SDL_Surface* image_surface, int threshold){
	// threshold goes from -100 to 100
double contrastLevel = pow((100.0 + threshold) / 100.0, 2); 

   
    double blue; 
    double green; 
    double red; 
    Uint32 pixel;

size_t width = image_surface->w;
        size_t height = image_surface->h;
        Uint8 r,g,b;
        SDL_Surface *image_surface_copy  = SDL_ConvertSurface(image_surface, image_surface->format, SDL_SWSURFACE);

   for (size_t i = 0; i < height; i++)
    {
        for (size_t j = 0; j < width  ; j++)
        {
		pixel = get_pixel(image_surface_copy,j,i);
                SDL_GetRGB(pixel, image_surface_copy->format, &r, &g, &b);


        blue = ((((b / 255.0) - 0.5) *  
                    contrastLevel) + 0.5) * 255.0; 

   
        green = ((((g / 255.0) - 0.5) *  
                    contrastLevel) + 0.5) * 255.0; 

   
        red = ((((r / 255.0) - 0.5) *  
                    contrastLevel) + 0.5) * 255.0; 

   
        if  (blue > 255) 
        { blue = 255; } 
        else if  (blue < 0) 
        { blue = 0; } 

   
        if (green > 255) 
        { green = 255; } 
        else if (green < 0) 
        { green = 0; } 

   
        if (red > 255) 
        { red = 255; } 
        else if (red < 0) 
        { red = 0; } 

   pixel = SDL_MapRGB(image_surface->format, red, green, blue);
                put_pixel(image_surface,j,i,pixel);
	}
    }
}
void Binarise(SDL_Surface *img)
{
    binarization(img, (float)1.0);
}
