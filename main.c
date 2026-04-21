#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "enigme.h"

#define LARGEUR_ECRAN 800
#define HAUTEUR_ECRAN 600

int main(int argc, char *argv[])
{
    SDL_Window *fenetre = NULL;
    SDL_Renderer *renderer = NULL;
    Enigme enigme = {0};
    SDL_Event event;
    int continuer = 1;
    int jeu_lance = 1;
    int menu_principal_actif = 1;
    
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
    
    fenetre = SDL_CreateWindow("Enigme - Quiz Game",
                                SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED,
                                LARGEUR_ECRAN, HAUTEUR_ECRAN,
                                SDL_WINDOW_SHOWN);
    
    renderer = SDL_CreateRenderer(fenetre, -1, SDL_RENDERER_ACCELERATED);
    
    enigme_init(&enigme, renderer);
    
    SDL_Color blanc = {255, 255, 255, 255};
    TTF_Font *police_menu = TTF_OpenFont("arial.ttf", 40);
    SDL_Surface *temp_surface;
    SDL_Texture *titre = NULL;
    SDL_Texture *jouer = NULL;
    SDL_Texture *quitter = NULL;
    
    if (police_menu) {
        temp_surface = TTF_RenderText_Blended(police_menu, "QUIZ GAME", blanc);
        titre = SDL_CreateTextureFromSurface(renderer, temp_surface);
        SDL_FreeSurface(temp_surface);
        
        temp_surface = TTF_RenderText_Blended(police_menu, "Appuyez sur ESPACE pour jouer", blanc);
        jouer = SDL_CreateTextureFromSurface(renderer, temp_surface);
        SDL_FreeSurface(temp_surface);
        
        temp_surface = TTF_RenderText_Blended(police_menu, "ECHAP pour quitter", blanc);
        quitter = SDL_CreateTextureFromSurface(renderer, temp_surface);
        SDL_FreeSurface(temp_surface);
    }
    
    SDL_Rect pos_titre = {250, 150, 300, 60};
    SDL_Rect pos_jouer = {200, 300, 400, 50};
    SDL_Rect pos_quitter = {250, 400, 300, 50};
    
    while (continuer)
    {
        while (menu_principal_actif && continuer)
        {
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT) {
                    continuer = 0;
                    menu_principal_actif = 0;
                }
                else if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym == SDLK_SPACE) {
                        menu_principal_actif = 0;
                        jeu_lance = 1;
                        enigme_init(&enigme, renderer);
                    }
                    else if (event.key.keysym.sym == SDLK_ESCAPE) {
                        continuer = 0;
                        menu_principal_actif = 0;
                    }
                }
            }
            
            SDL_SetRenderDrawColor(renderer, 30, 30, 60, 255);
            SDL_RenderClear(renderer);
            
            if (titre) SDL_RenderCopy(renderer, titre, NULL, &pos_titre);
            if (jouer) SDL_RenderCopy(renderer, jouer, NULL, &pos_jouer);
            if (quitter) SDL_RenderCopy(renderer, quitter, NULL, &pos_quitter);
            
            SDL_RenderPresent(renderer);
            SDL_Delay(16);
        }
        
        while (jeu_lance && continuer && !menu_principal_actif)
        {
            while (SDL_PollEvent(&event))
            {
                switch (event.type)
                {
                    case SDL_QUIT:
                        continuer = 0;
                        jeu_lance = 0;
                        break;
                    
                    case SDL_MOUSEBUTTONDOWN:
                        if (event.button.button == SDL_BUTTON_LEFT) {
                            enigme_gerer_clic(&enigme, event.button.x, event.button.y, &continuer, &jeu_lance);
                        }
                        break;
                    
                    case SDL_KEYDOWN:
                        if (event.key.keysym.sym == SDLK_ESCAPE) {
                            continuer = 0;
                            jeu_lance = 0;
                        }
                        else if (event.key.keysym.sym == SDLK_1 ||
                                 event.key.keysym.sym == SDLK_KP_1) {
                            if (!enigme.reponse_donnee && !enigme.enigme_terminee) {
                                enigme_verifier_reponse(&enigme, 1);
                            }
                        }
                        else if (event.key.keysym.sym == SDLK_2 ||
                                 event.key.keysym.sym == SDLK_KP_2) {
                            if (!enigme.reponse_donnee && !enigme.enigme_terminee) {
                                enigme_verifier_reponse(&enigme, 2);
                            }
                        }
                        else if (event.key.keysym.sym == SDLK_s && event.key.keysym.mod & KMOD_CTRL) {
                            enigme_sauvegarder(&enigme);
                            printf("Jeu sauvegarde (Ctrl+S)\n");
                        }
                        break;
                }
            }
            
            if (enigme.enigme_terminee) {
                SDL_Delay(3000);
                menu_principal_actif = 1;
                jeu_lance = 0;
                continue;
            }
            
            enigme_afficher(&enigme);
            SDL_RenderPresent(renderer);
            SDL_Delay(16);
        }
    }
    
    SDL_DestroyTexture(titre);
    SDL_DestroyTexture(jouer);
    SDL_DestroyTexture(quitter);
    TTF_CloseFont(police_menu);
    
    enigme_liberer(&enigme);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(fenetre);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    
    return 0;
}
