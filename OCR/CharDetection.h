#ifndef CHAR_DETECTION_
#define CHAR_DETECTION_

#include<stdlib.h>
#include"SDL/SDL.h"

//void DrawHorLine(int i);
//void DrawVerLine(int x, int start, int end);
//void CharacterLineFormation(int start, int end);
//void Boundry(SDL_Surface *img);
//void LineSegregation(int start, int end, int nb);
//void CharacterDetection(SDL_Surface *img);
SDL_Surface* Compression(SDL_Surface *img, int x, int y);
SDL_Surface** DetectCharacter(SDL_Surface *img);
int NbCharacters();

#endif