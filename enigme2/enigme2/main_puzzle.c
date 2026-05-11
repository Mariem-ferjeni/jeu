#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include "puzzl.h"

int main(int argc, char *argv[])
{
    SDL_Window *fenetre;
    SDL_Surface *ecran;
    EnsemblePuzzle puzzle;
    int score, vies, continuer, resultat;
    SDL_Event evenement;
    TTF_Font *police_temp;
    char texte_affichage[50];
    SDL_Surface *surface_texte;
    SDL_Rect position_texte;
    
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO);
    TTF_Init();
    IMG_Init(IMG_INIT_PNG);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);
    Mix_AllocateChannels(10);
    
    fenetre = SDL_CreateWindow("Puzzle - Oppenheimer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, LARGEUR_ECRAN, HAUTEUR_ECRAN, SDL_WINDOW_SHOWN);
    ecran = SDL_GetWindowSurface(fenetre);
    
    initialiser_puzzle(&puzzle);
    
    score = 0;
    vies = 3;
    continuer = 1;
    
    generer_puzzle(&puzzle);
    puzzle.jeu_actif = 1;
    
    while(continuer)
    {
        while(SDL_PollEvent(&evenement))
        {
            resultat = gerer_evenement(&puzzle, evenement, &score, &vies);
            if(resultat == 0) continuer = 0;
            if(resultat == 2)
            {
                generer_puzzle(&puzzle);
                puzzle.jeu_actif = 1;
            }
        }
        
        mettre_a_jour_temps(&puzzle);
        mettre_a_jour_rotzoom(&puzzle);
        
        if(vies <= 0)
        {
            continuer = 0;
        }
        
        afficher_puzzle(puzzle, ecran);
        
        police_temp = TTF_OpenFont("texxte.ttf", 24);
        if(police_temp == NULL) police_temp = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 24);
        if(police_temp)
        {
            sprintf(texte_affichage, "Score: %d   Vies: %d", score, vies);
            surface_texte = TTF_RenderText_Blended(police_temp, texte_affichage, puzzle.couleur_blanc);
            if(surface_texte)
            {
                position_texte.x = 20;
                position_texte.y = 20;
                position_texte.w = surface_texte->w;
                position_texte.h = surface_texte->h;
                SDL_BlitSurface(surface_texte, NULL, ecran, &position_texte);
                SDL_FreeSurface(surface_texte);
            }
            TTF_CloseFont(police_temp);
        }
        
        SDL_UpdateWindowSurface(fenetre);
        SDL_Delay(16);
    }
    
    liberer_puzzle(&puzzle);
    SDL_DestroyWindow(fenetre);
    Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0;
}
