#include "level_manager.h"
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    
    srand(time(NULL));
    
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) return 1;
    if (IMG_Init(IMG_INIT_PNG) == 0) return 1;
    if (TTF_Init() == -1) return 1;
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0) return 1;
    
    SDL_Window *window = SDL_CreateWindow("Jeu 2 Joueurs - Aventure",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        750, 550, SDL_WINDOW_RESIZABLE);
    if (!window) return 1;
    
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) return 1;
    
    jeuPrincipal(window, renderer);
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    
    return 0;
}
