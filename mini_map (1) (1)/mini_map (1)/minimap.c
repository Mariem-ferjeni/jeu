#include "minimap.h"
#include <SDL2/SDL_image.h>

void minimap_init(struct Minimap *m, SDL_Renderer *r, int l_monde, int h_monde, int multi)
{
    SDL_Surface *temp;
    SDL_Surface *redim;
    
    m->largeur_monde = l_monde;
    m->hauteur_monde = h_monde;
    m->multijoueur = multi;
    
    m->rect_carte.x = 1024 - 310;  
    m->rect_carte.y = 10;
    m->rect_carte.w = 300;
    m->rect_carte.h = 225;
    
    temp = IMG_Load("minimap_carte.png");
    if(temp) {
        redim = SDL_CreateRGBSurface(0, m->rect_carte.w, m->rect_carte.h, 32, 0,0,0,0);
        SDL_BlitScaled(temp, NULL, redim, NULL);
        m->texture_carte = SDL_CreateTextureFromSurface(r, redim);
        SDL_FreeSurface(redim);
        SDL_FreeSurface(temp);
    } else {
        m->texture_carte = NULL;
    }
    
    temp = IMG_Load("point_joueur1.png");
    if(temp) {
        redim = SDL_CreateRGBSurface(0, 12, 12, 32, 0,0,0,0);  
        SDL_BlitScaled(temp, NULL, redim, NULL);
        m->texture_joueur1 = SDL_CreateTextureFromSurface(r, redim);
        SDL_FreeSurface(redim);
        SDL_FreeSurface(temp);
    } else {
        m->texture_joueur1 = NULL;
    }
    
    if(multi) {
        temp = IMG_Load("point_joueur2.png");
        if(temp) {
            redim = SDL_CreateRGBSurface(0, 12, 12, 32, 0,0,0,0);
            SDL_BlitScaled(temp, NULL, redim, NULL);
            m->texture_joueur2 = SDL_CreateTextureFromSurface(r, redim);
            SDL_FreeSurface(redim);
            SDL_FreeSurface(temp);
        } else {
            m->texture_joueur2 = NULL;
        }
    }
    
    temp = IMG_Load("point_ennemi.png");
    if(temp) {
        redim = SDL_CreateRGBSurface(0, 10, 10, 32, 0,0,0,0);  // 10x10 au lieu de 6x6
        SDL_BlitScaled(temp, NULL, redim, NULL);
        m->texture_ennemi = SDL_CreateTextureFromSurface(r, redim);
        SDL_FreeSurface(redim);
        SDL_FreeSurface(temp);
    } else {
        m->texture_ennemi = NULL;
    }
    
    m->rect_joueur1.w = 12;
    m->rect_joueur1.h = 12;
    m->rect_joueur2.w = 12;
    m->rect_joueur2.h = 12;
    
    for(int i = 0; i < 10; i++) {
        m->rect_ennemis[i].w = 10;
        m->rect_ennemis[i].h = 10;
    }
}

void minimap_update(struct Minimap *m, int x1, int y1, int x2, int y2, int *ex, int *ey, int nb_ennemis)
{
    float scale_x = (float)m->rect_carte.w / m->largeur_monde;
    float scale_y = (float)m->rect_carte.h / m->hauteur_monde;
    
    m->rect_joueur1.x = m->rect_carte.x + (int)(x1 * scale_x);
    m->rect_joueur1.y = m->rect_carte.y + (int)(y1 * scale_y);
    
    if(m->multijoueur) {
        m->rect_joueur2.x = m->rect_carte.x + (int)(x2 * scale_x);
        m->rect_joueur2.y = m->rect_carte.y + (int)(y2 * scale_y);
    }
    
    for(int i = 0; i < nb_ennemis && i < 10; i++) {
        m->rect_ennemis[i].x = m->rect_carte.x + (int)(ex[i] * scale_x);
        m->rect_ennemis[i].y = m->rect_carte.y + (int)(ey[i] * scale_y);
    }
}

void minimap_draw(struct Minimap *m, SDL_Renderer *r, int nb_ennemis)
{
    
    if(m->texture_carte)
        SDL_RenderCopy(r, m->texture_carte, NULL, &m->rect_carte);
    else {
        SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(r, 0, 0, 0, 180);
        SDL_RenderFillRect(r, &m->rect_carte);
        SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
    }
    
    for(int i = 0; i < nb_ennemis && i < 10; i++) {
        if(m->texture_ennemi)
            SDL_RenderCopy(r, m->texture_ennemi, NULL, &m->rect_ennemis[i]);
        else {
            SDL_SetRenderDrawColor(r, 255, 0, 0, 255);
            SDL_RenderFillRect(r, &m->rect_ennemis[i]);
        }
    }
    
    if(m->multijoueur) {
        if(m->texture_joueur2)
            SDL_RenderCopy(r, m->texture_joueur2, NULL, &m->rect_joueur2);
        else {
            SDL_SetRenderDrawColor(r, 0, 0, 255, 255);
            SDL_RenderFillRect(r, &m->rect_joueur2);
        }
    }
    
    if(m->texture_joueur1)
        SDL_RenderCopy(r, m->texture_joueur1, NULL, &m->rect_joueur1);
    else {
        SDL_SetRenderDrawColor(r, 0, 255, 0, 255);
        SDL_RenderFillRect(r, &m->rect_joueur1);
    }
}

void minimap_liberer(struct Minimap *m, SDL_Renderer *r)
{
    if(m->texture_carte) SDL_DestroyTexture(m->texture_carte);
    if(m->texture_joueur1) SDL_DestroyTexture(m->texture_joueur1);
    if(m->texture_joueur2) SDL_DestroyTexture(m->texture_joueur2);
    if(m->texture_ennemi) SDL_DestroyTexture(m->texture_ennemi);
}
