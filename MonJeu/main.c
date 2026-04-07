#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include "background.h"

int main(int argc, char* argv[])
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Erreur SDL_Init: %s\n", SDL_GetError());
        return 1;
    }

    if(TTF_Init() < 0)
    {
        printf("Erreur TTF_Init: %s\n", TTF_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "Jeu SDL2 avec MiniMap",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1280, 720,
        0
    );

    if(!window)
    {
        printf("Erreur SDL_CreateWindow: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer)
    {
        printf("Erreur SDL_CreateRenderer: %s\n", SDL_GetError());
        return 1;
    }

    /* ================= INIT BACKGROUND ================= */
    Background background;
    initBackground(&background, renderer);
    initTemps(&background);
    initGuide(&background, renderer);
    initMiniMap(&background, 2000, 1500);  // Taille du niveau

    /* ================= INIT JOUEUR ================= */
    SDL_Rect player = {100, 100, 50, 50};
    int vitesse = 5;

    int running = 1;
    SDL_Event event;

    while(running)
    {
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
                running = 0;

            if(event.type == SDL_KEYDOWN)
            {
                switch(event.key.keysym.sym)
                {
                    case SDLK_ESCAPE: running = 0; break;
                    case SDLK_RIGHT: player.x += vitesse; scrolling(&background, 0); break;
                    case SDLK_LEFT:  player.x -= vitesse; scrolling(&background, 1); break;
                    case SDLK_UP:    player.y -= vitesse; scrolling(&background, 2); break;
                    case SDLK_DOWN:  player.y += vitesse; scrolling(&background, 3); break;
                }
            }
        }

        /* ================= RENDER ================= */
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        afficherBackground(background, renderer);

        // Affiche le joueur
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &player);

        afficherGuide(background, renderer);
        afficherTemps(background, renderer);
        afficherMiniMap(background, renderer, player);

        SDL_RenderPresent(renderer);

        SDL_Delay(16); // ~60 FPS
    }

    /* ================= CLEAN ================= */
    SDL_DestroyTexture(background.image);
    SDL_DestroyTexture(background.guide);
    TTF_CloseFont(background.font);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_Quit();
    SDL_Quit();

    return 0;
}
