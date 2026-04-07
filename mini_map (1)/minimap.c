#include "minimap.h"
#include <SDL2/SDL_image.h>
#include <stdio.h>

void init_minimap(Minimap *m, SDL_Renderer *renderer, int largeur_monde, int hauteur_monde, bool multijoueur) {
    m->largeur_monde = largeur_monde;
    m->hauteur_monde = hauteur_monde;
    m->actif = true;
    m->multijoueur = multijoueur;
    
    int minimap_largeur = 200;
    int minimap_hauteur = 150;
    int marge = 10;
    int pos_x = 1024 - minimap_largeur - marge;
    int pos_y = marge;
    
    SDL_Surface *tmp;
    SDL_Surface *redim;
    
    // Charger le fond
    tmp = IMG_Load("minimap_fond.png");
    if(tmp) {
        redim = SDL_CreateRGBSurface(0, minimap_largeur + 10, minimap_hauteur + 30, 32, 0,0,0,0);
        SDL_BlitScaled(tmp, NULL, redim, NULL);
        m->fond = SDL_CreateTextureFromSurface(renderer, redim);
        SDL_FreeSurface(redim);
        SDL_FreeSurface(tmp);
        printf("minimap_fond.png charge\n");
    } else {
        redim = SDL_CreateRGBSurface(0, minimap_largeur + 10, minimap_hauteur + 30, 32, 0,0,0,0);
        SDL_FillRect(redim, NULL, SDL_MapRGB(redim->format, 50, 50, 50));
        m->fond = SDL_CreateTextureFromSurface(renderer, redim);
        SDL_FreeSurface(redim);
        printf("Fond par defaut cree\n");
    }
    
    // Charger la carte
    tmp = IMG_Load("minimap_carte.png");
    if(tmp) {
        redim = SDL_CreateRGBSurface(0, minimap_largeur, minimap_hauteur, 32, 0,0,0,0);
        SDL_BlitScaled(tmp, NULL, redim, NULL);
        m->map = SDL_CreateTextureFromSurface(renderer, redim);
        SDL_FreeSurface(redim);
        SDL_FreeSurface(tmp);
        printf("minimap_carte.png charge\n");
    } else {
        redim = SDL_CreateRGBSurface(0, minimap_largeur, minimap_hauteur, 32, 0,0,0,0);
        SDL_FillRect(redim, NULL, SDL_MapRGB(redim->format, 60, 60, 60));
        m->map = SDL_CreateTextureFromSurface(renderer, redim);
        SDL_FreeSurface(redim);
        printf("Carte par defaut cree\n");
    }
    
    // Charger point_joueur1.png (JOUEUR 1)
    tmp = IMG_Load("point_joueur1.png");
    if(tmp) {
        redim = SDL_CreateRGBSurface(0, 8, 8, 32, 0,0,0,0);
        SDL_BlitScaled(tmp, NULL, redim, NULL);
        m->point_j1 = SDL_CreateTextureFromSurface(renderer, redim);
        SDL_FreeSurface(redim);
        SDL_FreeSurface(tmp);
        printf("point_joueur1.png charge et redimensionne\n");
    } else {
        printf("ERREUR: point_joueur1.png non trouve - utilisation rectangle vert\n");
        tmp = SDL_CreateRGBSurface(0, 8, 8, 32, 0,0,0,0);
        SDL_FillRect(tmp, NULL, SDL_MapRGB(tmp->format, 0, 255, 0));
        m->point_j1 = SDL_CreateTextureFromSurface(renderer, tmp);
        SDL_FreeSurface(tmp);
    }
    
    // Charger point_joueur2.png (JOUEUR 2)
    if(multijoueur) {
        tmp = IMG_Load("point_joueur2.png");
        if(tmp) {
            redim = SDL_CreateRGBSurface(0, 8, 8, 32, 0,0,0,0);
            SDL_BlitScaled(tmp, NULL, redim, NULL);
            m->point_j2 = SDL_CreateTextureFromSurface(renderer, redim);
            SDL_FreeSurface(redim);
            SDL_FreeSurface(tmp);
            printf("point_joueur2.png charge et redimensionne\n");
        } else {
            printf("ERREUR: point_joueur2.png non trouve - utilisation rectangle bleu\n");
            tmp = SDL_CreateRGBSurface(0, 8, 8, 32, 0,0,0,0);
            SDL_FillRect(tmp, NULL, SDL_MapRGB(tmp->format, 0, 0, 255));
            m->point_j2 = SDL_CreateTextureFromSurface(renderer, tmp);
            SDL_FreeSurface(tmp);
        }
    }
    
    // Charger point_ennemi.png
    for(int i = 0; i < 10; i++) {
        tmp = IMG_Load("point_ennemi.png");
        if(tmp) {
            redim = SDL_CreateRGBSurface(0, 6, 6, 32, 0,0,0,0);
            SDL_BlitScaled(tmp, NULL, redim, NULL);
            m->point_ennemi[i] = SDL_CreateTextureFromSurface(renderer, redim);
            SDL_FreeSurface(redim);
            SDL_FreeSurface(tmp);
        } else {
            tmp = SDL_CreateRGBSurface(0, 6, 6, 32, 0,0,0,0);
            SDL_FillRect(tmp, NULL, SDL_MapRGB(tmp->format, 255, 0, 0));
            m->point_ennemi[i] = SDL_CreateTextureFromSurface(renderer, tmp);
            SDL_FreeSurface(tmp);
        }
    }
    
    // Positions
    m->pos_fond = (SDL_Rect){pos_x - 5, pos_y - 5, minimap_largeur + 10, minimap_hauteur + 30};
    m->pos_map  = (SDL_Rect){pos_x, pos_y, minimap_largeur, minimap_hauteur};
    m->pos_j1.w = 8; m->pos_j1.h = 8;
    m->pos_j2.w = 8; m->pos_j2.h = 8;
    
    for(int i = 0; i < 10; i++) {
        m->pos_ennemi[i].w = 6; m->pos_ennemi[i].h = 6;
    }
}

void update_minimap(Minimap *m, int x1, int y1, int x2, int y2, int *xe, int *ye, int nb) {
    if(!m->actif) return;
    
    float sx = (float)m->pos_map.w / m->largeur_monde;
    float sy = (float)m->pos_map.h / m->hauteur_monde;
    
    // Position joueur 1
    m->pos_j1.x = m->pos_map.x + (int)(x1 * sx);
    m->pos_j1.y = m->pos_map.y + (int)(y1 * sy);
    
    if(m->pos_j1.x < m->pos_map.x) m->pos_j1.x = m->pos_map.x;
    if(m->pos_j1.x > m->pos_map.x + m->pos_map.w - m->pos_j1.w) 
        m->pos_j1.x = m->pos_map.x + m->pos_map.w - m->pos_j1.w;
    if(m->pos_j1.y < m->pos_map.y) m->pos_j1.y = m->pos_map.y;
    if(m->pos_j1.y > m->pos_map.y + m->pos_map.h - m->pos_j1.h) 
        m->pos_j1.y = m->pos_map.y + m->pos_map.h - m->pos_j1.h;
    
    // Position joueur 2
    if(m->multijoueur) {
        m->pos_j2.x = m->pos_map.x + (int)(x2 * sx);
        m->pos_j2.y = m->pos_map.y + (int)(y2 * sy);
        
        if(m->pos_j2.x < m->pos_map.x) m->pos_j2.x = m->pos_map.x;
        if(m->pos_j2.x > m->pos_map.x + m->pos_map.w - m->pos_j2.w) 
            m->pos_j2.x = m->pos_map.x + m->pos_map.w - m->pos_j2.w;
        if(m->pos_j2.y < m->pos_map.y) m->pos_j2.y = m->pos_map.y;
        if(m->pos_j2.y > m->pos_map.y + m->pos_map.h - m->pos_j2.h) 
            m->pos_j2.y = m->pos_map.y + m->pos_map.h - m->pos_j2.h;
    }
    
    // Positions ennemis
    for(int i = 0; i < nb && i < 10; i++) {
        m->pos_ennemi[i].x = m->pos_map.x + (int)(xe[i] * sx);
        m->pos_ennemi[i].y = m->pos_map.y + (int)(ye[i] * sy);
        
        if(m->pos_ennemi[i].x < m->pos_map.x) m->pos_ennemi[i].x = m->pos_map.x;
        if(m->pos_ennemi[i].x > m->pos_map.x + m->pos_map.w - m->pos_ennemi[i].w) 
            m->pos_ennemi[i].x = m->pos_map.x + m->pos_map.w - m->pos_ennemi[i].w;
        if(m->pos_ennemi[i].y < m->pos_map.y) m->pos_ennemi[i].y = m->pos_map.y;
        if(m->pos_ennemi[i].y > m->pos_map.y + m->pos_map.h - m->pos_ennemi[i].h) 
            m->pos_ennemi[i].y = m->pos_map.y + m->pos_map.h - m->pos_ennemi[i].h;
    }
}

void afficher_minimap(Minimap *m, SDL_Renderer *renderer, int nb) {
    if(!m->actif) return;
    
    // Afficher le fond
    SDL_RenderCopy(renderer, m->fond, NULL, &m->pos_fond);
    
    // Afficher la carte
    SDL_RenderCopy(renderer, m->map, NULL, &m->pos_map);
    
    // Afficher les ennemis
    for(int i = 0; i < nb && i < 10; i++) {
        SDL_RenderCopy(renderer, m->point_ennemi[i], NULL, &m->pos_ennemi[i]);
    }
    
    // Afficher le joueur 2 (si multijoueur)
    if(m->multijoueur) {
        SDL_RenderCopy(renderer, m->point_j2, NULL, &m->pos_j2);
    }
    
    // Afficher le joueur 1 (par-dessus)
    SDL_RenderCopy(renderer, m->point_j1, NULL, &m->pos_j1);
}

void free_minimap(Minimap *m, SDL_Renderer *renderer) {
    if(m->fond) SDL_DestroyTexture(m->fond);
    if(m->map) SDL_DestroyTexture(m->map);
    if(m->point_j1) SDL_DestroyTexture(m->point_j1);
    if(m->point_j2) SDL_DestroyTexture(m->point_j2);
    for(int i = 0; i < 10; i++) {
        if(m->point_ennemi[i]) SDL_DestroyTexture(m->point_ennemi[i]);
    }
}
