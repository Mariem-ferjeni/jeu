/*#include <SDL2/SDL.h>
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
    Enigme enigme;
    SDL_Event event;
    int continuer = 1;
    int jeu_lance = 1;
    int menu_principal_actif = 1;
    int choix_menu = 0;
    
    // Initialisation de SDL2
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Erreur SDL_Init: %s\n", SDL_GetError());
        return 1;
    }
    
    // Initialisation de SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        printf("Erreur IMG_Init: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }
    
    // Initialisation de SDL_ttf
    if (TTF_Init() < 0)
    {
        printf("Erreur TTF_Init: %s\n", TTF_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
    
    // Création de la fenêtre
    fenetre = SDL_CreateWindow("Enigme - Quiz Game",
                                SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED,
                                LARGEUR_ECRAN, HAUTEUR_ECRAN,
                                SDL_WINDOW_SHOWN);
    if (fenetre == NULL)
    {
        printf("Erreur SDL_CreateWindow: %s\n", SDL_GetError());
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
    
    // Création du renderer
    renderer = SDL_CreateRenderer(fenetre, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
    {
        printf("Erreur SDL_CreateRenderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(fenetre);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
    
    // Initialisation de l'énigme
    enigme_init(&enigme, renderer);
    
    // Menu principal
    SDL_Color blanc = {255, 255, 255, 255};
    TTF_Font *police_menu = TTF_OpenFont("arial.ttf", 40);
    SDL_Surface *temp_surface;
    SDL_Texture *titre = NULL;
    SDL_Texture *nouvelle_partie = NULL;
    SDL_Texture *charger_partie = NULL;
    SDL_Texture *quitter = NULL;
    
    if (police_menu) {
        temp_surface = TTF_RenderText_Blended(police_menu, "QUIZ GAME", blanc);
        titre = SDL_CreateTextureFromSurface(renderer, temp_surface);
        SDL_FreeSurface(temp_surface);
        
        temp_surface = TTF_RenderText_Blended(police_menu, "Nouvelle Partie", blanc);
        nouvelle_partie = SDL_CreateTextureFromSurface(renderer, temp_surface);
        SDL_FreeSurface(temp_surface);
        
        temp_surface = TTF_RenderText_Blended(police_menu, "Charger Partie", blanc);
        charger_partie = SDL_CreateTextureFromSurface(renderer, temp_surface);
        SDL_FreeSurface(temp_surface);
        
        temp_surface = TTF_RenderText_Blended(police_menu, "Quitter", blanc);
        quitter = SDL_CreateTextureFromSurface(renderer, temp_surface);
        SDL_FreeSurface(temp_surface);
    }
    
    SDL_Rect pos_titre = {250, 100, 300, 60};
    SDL_Rect pos_nouvelle = {300, 250, 200, 50};
    SDL_Rect pos_charger = {300, 330, 200, 50};
    SDL_Rect pos_quitter = {300, 410, 200, 50};
    
    // Boucle principale
    while (continuer)
    {
        // Menu principal
        while (menu_principal_actif && continuer)
        {
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                {
                    continuer = 0;
                    menu_principal_actif = 0;
                }
                else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
                {
                    int x = event.button.x;
                    int y = event.button.y;
                    
                    if (x >= pos_nouvelle.x && x <= pos_nouvelle.x + 200 &&
                        y >= pos_nouvelle.y && y <= pos_nouvelle.y + 50)
                    {
                        // Nouvelle partie
                        menu_principal_actif = 0;
                        jeu_lance = 1;
                        enigme_init(&enigme, renderer);
                        printf("Nouvelle partie demarree!\n");
                    }
                    else if (x >= pos_charger.x && x <= pos_charger.x + 200 &&
                             y >= pos_charger.y && y <= pos_charger.y + 50)
                    {
                        // Charger partie
                        menu_principal_actif = 0;
                        jeu_lance = 1;
                        enigme_init(&enigme, renderer);
                        enigme_charger(&enigme);
                        printf("Partie chargee!\n");
                    }
                    else if (x >= pos_quitter.x && x <= pos_quitter.x + 200 &&
                             y >= pos_quitter.y && y <= pos_quitter.y + 50)
                    {
                        continuer = 0;
                        menu_principal_actif = 0;
                    }
                }
                else if (event.type == SDL_KEYDOWN)
                {
                    if (event.key.keysym.sym == SDLK_ESCAPE)
                    {
                        continuer = 0;
                        menu_principal_actif = 0;
                    }
                }
            }
            
            // Afficher le menu principal
            SDL_SetRenderDrawColor(renderer, 30, 30, 60, 255);
            SDL_RenderClear(renderer);
            
            if (titre) SDL_RenderCopy(renderer, titre, NULL, &pos_titre);
            if (nouvelle_partie) SDL_RenderCopy(renderer, nouvelle_partie, NULL, &pos_nouvelle);
            if (charger_partie) SDL_RenderCopy(renderer, charger_partie, NULL, &pos_charger);
            if (quitter) SDL_RenderCopy(renderer, quitter, NULL, &pos_quitter);
            
            SDL_RenderPresent(renderer);
            SDL_Delay(16);
        }
        
        // Boucle du jeu
        while (jeu_lance && continuer && !menu_principal_actif)
        {
            // Gestion des événements
            while (SDL_PollEvent(&event))
            {
                switch (event.type)
                {
                    case SDL_QUIT:
                        continuer = 0;
                        jeu_lance = 0;
                        break;
                    
                    case SDL_MOUSEBUTTONDOWN:
                        if (event.button.button == SDL_BUTTON_LEFT)
                        {
                            enigme_gerer_clic(&enigme, event.button.x, event.button.y, &continuer, &jeu_lance);
                        }
                        break;
                    
                    case SDL_KEYDOWN:
                        if (event.key.keysym.sym == SDLK_ESCAPE)
                        {
                            // Ouvrir/fermer le menu pause
                            enigme.menu_actif = !enigme.menu_actif;
                        }
                        else if (event.key.keysym.sym == SDLK_y || event.key.keysym.sym == SDLK_o)
                        {
                            // Touche Y ou O pour OUI
                            if (!enigme.reponse_donnee && !enigme.menu_actif && !enigme.enigme_terminee)
                            {
                                enigme_verifier_reponse(&enigme, 1);
                            }
                        }
                        else if (event.key.keysym.sym == SDLK_n)
                        {
                            // Touche N pour NON
                            if (!enigme.reponse_donnee && !enigme.menu_actif && !enigme.enigme_terminee)
                            {
                                enigme_verifier_reponse(&enigme, 0);
                            }
                        }
                        else if (event.key.keysym.sym == SDLK_RETURN && enigme.resultat_affiche)
                        {
                            // Passer à la question suivante
                            if (enigme.vies > 0 && !enigme.enigme_terminee)
                            {
                                enigme_reinitialiser(&enigme);
                            }
                        }
                        else if (event.key.keysym.sym == SDLK_s && event.key.keysym.mod & KMOD_CTRL)
                        {
                            // Ctrl+S pour sauvegarder
                            enigme_sauvegarder(&enigme);
                            printf("Jeu sauvegarde (Ctrl+S)\n");
                        }
                        break;
                }
            }
            
            // Vérifier si la partie est terminée
            if (enigme.enigme_terminee)
            {
                SDL_Delay(3000);  // Attendre 3 secondes
                menu_principal_actif = 1;
                jeu_lance = 0;
                continue;
            }
            
            // Afficher le jeu
            enigme_afficher(&enigme);
            
            // Mettre à jour l'écran
            SDL_RenderPresent(renderer);
            
            // Petite pause pour éviter de surcharger le CPU
            SDL_Delay(16);
        }
    }
    
    // Nettoyage
    SDL_DestroyTexture(titre);
    SDL_DestroyTexture(nouvelle_partie);
    SDL_DestroyTexture(charger_partie);
    SDL_DestroyTexture(quitter);
    TTF_CloseFont(police_menu);
    
    enigme_liberer(&enigme);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(fenetre);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    
    printf("Jeu ferme. Merci d'avoir joue!\n");
    
    return 0;
}*/
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
    Enigme enigme;
    SDL_Event event;
    int continuer = 1;
    int jeu_lance = 1;
    int menu_principal_actif = 1;
    
    // Initialisation SDL
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
    
    // Création fenêtre
    fenetre = SDL_CreateWindow("Enigme - Quiz Game",
                                SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED,
                                LARGEUR_ECRAN, HAUTEUR_ECRAN,
                                SDL_WINDOW_SHOWN);
    
    renderer = SDL_CreateRenderer(fenetre, -1, SDL_RENDERER_ACCELERATED);
    
    // Initialisation du jeu
    enigme_init(&enigme, renderer);
    
    // Menu principal simple
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
    
    // Boucle principale
    while (continuer)
    {
        // Menu principal
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
        
        // Boucle du jeu
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
                        else if (event.key.keysym.sym == SDLK_y || event.key.keysym.sym == SDLK_o) {
                            if (!enigme.reponse_donnee && !enigme.enigme_terminee) {
                                enigme_verifier_reponse(&enigme, 1);
                            }
                        }
                        else if (event.key.keysym.sym == SDLK_n) {
                            if (!enigme.reponse_donnee && !enigme.enigme_terminee) {
                                enigme_verifier_reponse(&enigme, 0);
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
    
    // Nettoyage
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
