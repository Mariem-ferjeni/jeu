#include "background.h"
#include <stdio.h>

/* ================= BACKGROUND ================= */

void initBackground(Background *b, SDL_Renderer *renderer)
{
    SDL_Surface *surface = IMG_Load("images/background.png");

    if(surface == NULL)
    {
        printf("Erreur chargement background\n");
        return;
    }

    b->image = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    b->camera.x = 0;
    b->camera.y = 0;
    b->camera.w = 1280;
    b->camera.h = 720;
}

void afficherBackground(Background b, SDL_Renderer *renderer)
{
    SDL_RenderCopy(renderer, b.image, &b.camera, NULL);
}

void scrolling(Background *b, int direction)
{
    int vitesse = 10;

    if(direction == 0) b->camera.x += vitesse;
    if(direction == 1) b->camera.x -= vitesse;
    if(direction == 2) b->camera.y -= vitesse;
    if(direction == 3) b->camera.y += vitesse;
}

/* ================= TEMPS ================= */

void initTemps(Background *b)
{
    b->startTime = SDL_GetTicks();
    b->posTemps.x = 20;
    b->posTemps.y = 20;

    b->couleur.r = 255;
    b->couleur.g = 255;
    b->couleur.b = 255;

    b->font = TTF_OpenFont("font/arial.ttf",30);
    if(b->font == NULL)
        printf("Erreur chargement font\n");
}

void afficherTemps(Background b, SDL_Renderer *renderer)
{
    Uint32 temps = (SDL_GetTicks() - b.startTime) / 1000;
    int minutes = temps / 60;
    int secondes = temps % 60;

    char texte[20];
    sprintf(texte,"%02d:%02d",minutes,secondes);

    SDL_Surface *surface = TTF_RenderText_Solid(b.font, texte, b.couleur);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect pos = { b.posTemps.x, b.posTemps.y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &pos);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

/* ================= GUIDE ================= */

void initGuide(Background *b, SDL_Renderer *renderer)
{
    SDL_Surface *surface = IMG_Load("images/guide.png");
    if(surface == NULL)
    {
        printf("Erreur chargement guide\n");
        return;
    }

    b->guide = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    b->posGuide.x = 1000;
    b->posGuide.y = 20;
    b->posGuide.w = 200;
    b->posGuide.h = 100;
}

void afficherGuide(Background b, SDL_Renderer *renderer)
{
    SDL_RenderCopy(renderer, b.guide, NULL, &b.posGuide);
}

/* ================= MINI-MAP ================= */

void initMiniMap(Background *b, int levelWidth, int levelHeight)
{
    // Mini-map affichée en haut à droite
    b->miniMapRect.x = 1050;
    b->miniMapRect.y = 550;
    b->miniMapRect.w = 200;
    b->miniMapRect.h = 150;

    // calcul de l'échelle
    b->scaleX = (float)b->miniMapRect.w / levelWidth;
    b->scaleY = (float)b->miniMapRect.h / levelHeight;
}

void afficherMiniMap(Background b, SDL_Renderer *renderer, SDL_Rect playerPos)
{
    // fond mini-map
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
    SDL_RenderFillRect(renderer, &b.miniMapRect);

    // bordure mini-map
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &b.miniMapRect);

    // position du joueur sur la mini-map
    SDL_Rect miniPlayer;
    miniPlayer.w = 5;
    miniPlayer.h = 5;
    miniPlayer.x = b.miniMapRect.x + playerPos.x * b.scaleX;
    miniPlayer.y = b.miniMapRect.y + playerPos.y * b.scaleY;

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // joueur en rouge
    SDL_RenderFillRect(renderer, &miniPlayer);
}
