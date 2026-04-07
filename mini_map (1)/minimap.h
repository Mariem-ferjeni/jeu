#ifndef MINIMAP_H
#define MINIMAP_H

#include <SDL2/SDL.h>
#include <stdbool.h>

typedef struct {
    SDL_Texture *map;        
    SDL_Texture *fond;       
    SDL_Texture *point_j1;
    SDL_Texture *point_j2;
    SDL_Texture *point_ennemi[10];
    SDL_Rect pos_fond;
    SDL_Rect pos_map;
    SDL_Rect pos_j1;
    SDL_Rect pos_j2;
    SDL_Rect pos_ennemi[10];
    int largeur_monde;
    int hauteur_monde;
    bool actif;
    bool multijoueur;
} Minimap;

void init_minimap(Minimap *m, SDL_Renderer *renderer, int largeur_monde, int hauteur_monde, bool multijoueur);
void update_minimap(Minimap *m, int x1, int y1, int x2, int y2, int *xe, int *ye, int nb);
void afficher_minimap(Minimap *m, SDL_Renderer *renderer, int nb);
void free_minimap(Minimap *m, SDL_Renderer *renderer);

#endif
