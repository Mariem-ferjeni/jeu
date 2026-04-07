#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

typedef struct
{
    SDL_Texture *image;    // background principal
    SDL_Rect camera;

    /* temps */
    Uint32 startTime;
    SDL_Rect posTemps;
    SDL_Color couleur;
    TTF_Font *font;

    /* guide */
    SDL_Texture *guide;
    SDL_Rect posGuide;

    /* mini-map */
    SDL_Rect miniMapRect;   // rectangle de la mini-map sur l'écran
    float scaleX;           // échelle largeur
    float scaleY;           // échelle hauteur
} Background;

/* background */
void initBackground(Background *b, SDL_Renderer *renderer);
void afficherBackground(Background b, SDL_Renderer *renderer);
void scrolling(Background *b, int direction);

/* temps */
void initTemps(Background *b);
void afficherTemps(Background b, SDL_Renderer *renderer);

/* guide */
void initGuide(Background *b, SDL_Renderer *renderer);
void afficherGuide(Background b, SDL_Renderer *renderer);

/* mini-map */
void initMiniMap(Background *b, int levelWidth, int levelHeight);
void afficherMiniMap(Background b, SDL_Renderer *renderer, SDL_Rect playerPos);

#endif
