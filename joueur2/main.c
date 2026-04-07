#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include "personnage.h"
#include <stdio.h>
#include <stdbool.h>

int main(int argc, char *argv[]) {
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("Erreur SDL: %s\n", SDL_GetError());
        return -1;
    }
    
    if(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) == 0) {
        printf("Erreur IMG: %s\n", IMG_GetError());
        return -1;
    }
    
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0) {
        printf("Erreur audio: %s\n", Mix_GetError());
    }
    
    if(TTF_Init() == -1) {
        printf("Erreur TTF: %s\n", TTF_GetError());
    }
    
    SDL_Window *window = SDL_CreateWindow("Jeu Personnage", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600, SDL_WINDOW_SHOWN);
    
    if(!window) {
        printf("Erreur fenêtre\n");
        return -1;
    }
    
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    // Sons
    Mix_Music *musique = Mix_LoadMUS("bruit.mp3");
    Mix_Chunk *sonSaut = Mix_LoadWAV("jump.wav");
    TTF_Font *font = TTF_OpenFont("arial.ttf", 24);
    
    // Personnage
    Personnage perso;
    if(initPerso(&perso, renderer) != 0) {
        printf("Erreur personnage\n");
        return -1;
    }
    
    int ground_level = 550;
    int screen_width = 800;
    bool quit = false;
    SDL_Event event;
    Uint32 lastTime = SDL_GetTicks();
    
    // État des touches pour une meilleure réactivité
    bool leftPressed = false;
    bool rightPressed = false;
    
    printf("\n=== JEU ===\n");
    printf("A/←: Gauche | D/→: Droite | ESPACE: Saut | ESC: Quitter\n\n");
    
    while(!quit) {
        Uint32 currentTime = SDL_GetTicks();
        Uint32 dt = currentTime - lastTime;
        if(dt > 50) dt = 16;
        lastTime = currentTime;
        
        // Gestion des événements
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) quit = true;
            
            if(event.type == SDL_KEYDOWN) {
                switch(event.key.keysym.sym) {
                    case SDLK_RIGHT:
                    case SDLK_d:
                        rightPressed = true;
                        perso.direction = 1;
                        if(musique && Mix_PlayingMusic() == 0) Mix_PlayMusic(musique, -1);
                        break;
                    case SDLK_LEFT:
                    case SDLK_a:
                        leftPressed = true;
                        perso.direction = -1;
                        if(musique && Mix_PlayingMusic() == 0) Mix_PlayMusic(musique, -1);
                        break;
                    case SDLK_SPACE:
                    case SDLK_UP:
                    case SDLK_w:
                        if(!perso.jumping && perso.onGround) {
                            saut(&perso);
                            if(sonSaut) Mix_PlayChannel(-1, sonSaut, 0);
                        }
                        break;
                    case SDLK_ESCAPE:
                        quit = true;
                        break;
                }
            }
            
            if(event.type == SDL_KEYUP) {
                switch(event.key.keysym.sym) {
                    case SDLK_RIGHT:
                    case SDLK_d:
                        rightPressed = false;
                        if(!leftPressed) perso.direction = 0;
                        break;
                    case SDLK_LEFT:
                    case SDLK_a:
                        leftPressed = false;
                        if(!rightPressed) perso.direction = 0;
                        break;
                }
                // Arrêter la musique si plus de mouvement
                if(!leftPressed && !rightPressed && musique) Mix_HaltMusic();
            }
        }
        
        // Mise à jour
        movePerso(&perso, dt, screen_width, ground_level);
        updateJump(&perso, ground_level);
        animerPerso(&perso, dt);
        
        // Rendu
        SDL_SetRenderDrawColor(renderer, 100, 150, 200, 255);
        SDL_RenderClear(renderer);
        
        // Sol
        SDL_SetRenderDrawColor(renderer, 80, 60, 40, 255);
        SDL_Rect ground = {0, ground_level, screen_width, 50};
        SDL_RenderFillRect(renderer, &ground);
        
        SDL_SetRenderDrawColor(renderer, 50, 150, 50, 255);
        SDL_Rect grass = {0, ground_level - 5, screen_width, 5};
        SDL_RenderFillRect(renderer, &grass);
        
        // Personnage
        afficherPerso(renderer, &perso);
        
        // Score
        if(font) {
            SDL_Color white = {255, 255, 255};
            char scoreText[50];
            sprintf(scoreText, "Score: %d", perso.score);
            SDL_Surface *surf = TTF_RenderText_Blended(font, scoreText, white);
            if(surf) {
                SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
                SDL_Rect scoreRect = {10, 10, surf->w, surf->h};
                SDL_RenderCopy(renderer, tex, NULL, &scoreRect);
                SDL_FreeSurface(surf);
                SDL_DestroyTexture(tex);
            }
        }
        
        // Vies
        afficherVies(renderer, &perso, 700, 10, 30);
        
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    
    // Nettoyage
    if(musique) Mix_FreeMusic(musique);
    if(sonSaut) Mix_FreeChunk(sonSaut);
    if(font) TTF_CloseFont(font);
    cleanupPerso(&perso);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    
    return 0;
}
