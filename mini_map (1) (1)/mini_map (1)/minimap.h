#ifndef MINIMAP_H
#define MINIMAP_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

extern TTF_Font *police;

struct Minimap {
    SDL_Texture *texture_carte;
    SDL_Texture *texture_joueur1;
    SDL_Texture *texture_joueur2;
    SDL_Texture *texture_ennemi;
    SDL_Rect rect_carte;
    SDL_Rect rect_joueur1;
    SDL_Rect rect_joueur2;
    SDL_Rect rect_ennemis[10];
    int largeur_monde;
    int hauteur_monde;
    int multijoueur;
};

void minimap_init(struct Minimap *m, SDL_Renderer *r, int l_monde, int h_monde, int multi);
void minimap_update(struct Minimap *m, int x1, int y1, int x2, int y2, int *ex, int *ey, int nb_ennemis);
void minimap_draw(struct Minimap *m, SDL_Renderer *r, int nb_ennemis);
void minimap_liberer(struct Minimap *m, SDL_Renderer *r);

#endif
