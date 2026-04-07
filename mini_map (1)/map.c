#include "minimap.h"

void init_minimap(Minimap *m, int largeur_monde, int hauteur_monde, int multijoueur)
{
    // Dimensions
    m->largeur_monde = largeur_monde;
    m->hauteur_monde = hauteur_monde;
    m->largeur_minimap = 300;  // Largeur de la mini-map à l'écran
    m->actif = 1;               // Activée par défaut
    m->multijoueur = multijoueur;
    
    // Positions
    m->pos_minimap.x = 20;
    m->pos_minimap.y = 20;
    m->pos_minimap.w = m->largeur_minimap;
    m->pos_minimap.h = 150;
    
    m->pos_point.x = m->pos_minimap.x;
    m->pos_point.y = m->pos_minimap.y + m->pos_minimap.h/2;
    
    m->pos_point_j2.x = m->pos_minimap.x;
    m->pos_point_j2.y = m->pos_minimap.y + m->pos_minimap.h/2 + 10;
    
    for(int i=0; i<10; i++)
    {
        m->pos_ennemis[i].x = m->pos_minimap.x;
        m->pos_ennemis[i].y = m->pos_minimap.y;
    }
    
    // Police
    m->police = TTF_OpenFont("arial.ttf", 15);
    m->couleur_texte.r = 255;
    m->couleur_texte.g = 255;
    m->couleur_texte.b = 255;
    
    // Charger les images
    charger_images_minimap(m);
}

void charger_images_minimap(Minimap *m)
{
    // Créer la mini-map de base (rectangle gris)
    m->minimap = SDL_CreateRGBSurface(SDL_HWSURFACE, m->largeur_minimap, 150, 32, 0,0,0,0);
    SDL_FillRect(m->minimap, NULL, SDL_MapRGB(m->minimap->format, 50, 50, 50));
    
    // Ajouter un cadre blanc
    SDL_Rect cadre = {0, 0, m->largeur_minimap-1, 149};
    SDL_FillRect(m->minimap, &cadre, SDL_MapRGB(m->minimap->format, 255, 255, 255));
    
    // Créer les points
    m->point = SDL_CreateRGBSurface(SDL_HWSURFACE, 8, 8, 32, 0,0,0,0);
    SDL_FillRect(m->point, NULL, SDL_MapRGB(m->point->format, 0, 255, 0));  // Vert pour joueur 1
    
    m->point_j2 = SDL_CreateRGBSurface(SDL_HWSURFACE, 8, 8, 32, 0,0,0,0);
    SDL_FillRect(m->point_j2, NULL, SDL_MapRGB(m->point_j2->format, 0, 0, 255));  // Bleu pour joueur 2
    
    m->point_ennemi = SDL_CreateRGBSurface(SDL_HWSURFACE, 6, 6, 32, 0,0,0,0);
    SDL_FillRect(m->point_ennemi, NULL, SDL_MapRGB(m->point_ennemi->format, 255, 0, 0));  // Rouge pour ennemi
    
    // Fond (cadre décoratif)
    m->fond = SDL_CreateRGBSurface(SDL_HWSURFACE, m->largeur_minimap+10, 160, 32, 0,0,0,0);
    SDL_FillRect(m->fond, NULL, SDL_MapRGB(m->fond->format, 100, 100, 100));
    
    // Essayer de charger des images si elles existent
    SDL_Surface *temp;
    temp = IMG_Load("minimap_fond.png");
    if(temp)
    {
        SDL_FreeSurface(m->fond);
        m->fond = temp;
    }
    
    temp = IMG_Load("minimap_carte.png");
    if(temp)
    {
        SDL_FreeSurface(m->minimap);
        m->minimap = temp;
    }
    
    temp = IMG_Load("point_joueur.png");
    if(temp)
    {
        SDL_FreeSurface(m->point);
        m->point = temp;
    }
    
    temp = IMG_Load("point_joueur2.png");
    if(temp)
    {
        SDL_FreeSurface(m->point_j2);
        m->point_j2 = temp;
    }
    
    temp = IMG_Load("point_ennemi.png");
    if(temp)
    {
        SDL_FreeSurface(m->point_ennemi);
        m->point_ennemi = temp;
    }
}

void update_minimap(Minimap *m, int x_joueur, int y_joueur, int x_joueur2, int *x_ennemis, int *y_ennemis, int nb_ennemis)
{
    if(!m->actif) return;
    
    // Calculer l'échelle
    float echelle_x = (float)m->largeur_minimap / m->largeur_monde;
    float echelle_y = (float)m->pos_minimap.h / m->hauteur_monde;
    
    // Position du joueur 1
    m->pos_point.x = m->pos_minimap.x + (int)(x_joueur * echelle_x);
    m->pos_point.y = m->pos_minimap.y + (int)(y_joueur * echelle_y);
    
    // Limites
    if(m->pos_point.x < m->pos_minimap.x) 
        m->pos_point.x = m->pos_minimap.x;
    if(m->pos_point.x > m->pos_minimap.x + m->largeur_minimap - 8) 
        m->pos_point.x = m->pos_minimap.x + m->largeur_minimap - 8;
    if(m->pos_point.y < m->pos_minimap.y) 
        m->pos_point.y = m->pos_minimap.y;
    if(m->pos_point.y > m->pos_minimap.y + m->pos_minimap.h - 8) 
        m->pos_point.y = m->pos_minimap.y + m->pos_minimap.h - 8;
    
    // Position du joueur 2 (si multijoueur)
    if(m->multijoueur)
    {
        m->pos_point_j2.x = m->pos_minimap.x + (int)(x_joueur2 * echelle_x);
        m->pos_point_j2.y = m->pos_minimap.y + (int)(y_joueur * echelle_y) + 10;  // Décalé
        
        if(m->pos_point_j2.x < m->pos_minimap.x) 
            m->pos_point_j2.x = m->pos_minimap.x;
        if(m->pos_point_j2.x > m->pos_minimap.x + m->largeur_minimap - 8) 
            m->pos_point_j2.x = m->pos_minimap.x + m->largeur_minimap - 8;
    }
    
    // Positions des ennemis
    for(int i=0; i<nb_ennemis && i<10; i++)
    {
        m->pos_ennemis[i].x = m->pos_minimap.x + (int)(x_ennemis[i] * echelle_x);
        m->pos_ennemis[i].y = m->pos_minimap.y + (int)(y_ennemis[i] * echelle_y);
        
        if(m->pos_ennemis[i].x < m->pos_minimap.x) 
            m->pos_ennemis[i].x = m->pos_minimap.x;
        if(m->pos_ennemis[i].x > m->pos_minimap.x + m->largeur_minimap - 6) 
            m->pos_ennemis[i].x = m->pos_minimap.x + m->largeur_minimap - 6;
        if(m->pos_ennemis[i].y < m->pos_minimap.y) 
            m->pos_ennemis[i].y = m->pos_minimap.y;
        if(m->pos_ennemis[i].y > m->pos_minimap.y + m->pos_minimap.h - 6) 
            m->pos_ennemis[i].y = m->pos_minimap.y + m->pos_minimap.h - 6;
    }
}

void afficher_minimap(Minimap m, SDL_Surface *ecran)
{
    if(!m.actif) return;
    
    // Afficher le fond
    SDL_Rect pos_fond = {m.pos_minimap.x - 5, m.pos_minimap.y - 5, m.largeur_minimap+10, 160};
    SDL_BlitSurface(m.fond, NULL, ecran, &pos_fond);
    
    // Afficher la carte
    SDL_BlitSurface(m.minimap, NULL, ecran, &m.pos_minimap);
    
    // Afficher les ennemis
    for(int i=0; i<10; i++)
    {
        if(m.pos_ennemis[i].x != m.pos_minimap.x || m.pos_ennemis[i].y != m.pos_minimap.y)
        {
            SDL_BlitSurface(m.point_ennemi, NULL, ecran, &m.pos_ennemis[i]);
        }
    }
    
    // Afficher le joueur 2 (si multijoueur)
    if(m.multijoueur)
    {
        SDL_BlitSurface(m.point_j2, NULL, ecran, &m.pos_point_j2);
    }
    
    // Afficher le joueur 1 (par-dessus)
    SDL_BlitSurface(m.point, NULL, ecran, &m.pos_point);
    
    // Afficher les légendes
    if(m.police)
    {
        SDL_Surface *texte_j1 = TTF_RenderText_Blended(m.police, "J1", m.couleur_texte);
        SDL_Surface *texte_j2 = TTF_RenderText_Blended(m.police, "J2", m.couleur_texte);
        SDL_Surface *texte_en = TTF_RenderText_Blended(m.police, "Ennemi", m.couleur_texte);
        
        SDL_Rect pos_j1 = {m.pos_minimap.x + 10, m.pos_minimap.y + m.pos_minimap.h + 5};
        SDL_Rect pos_j2 = {m.pos_minimap.x + 60, m.pos_minimap.y + m.pos_minimap.h + 5};
        SDL_Rect pos_en = {m.pos_minimap.x + 110, m.pos_minimap.y + m.pos_minimap.h + 5};
        
        SDL_BlitSurface(texte_j1, NULL, ecran, &pos_j1);
        SDL_BlitSurface(texte_j2, NULL, ecran, &pos_j2);
        SDL_BlitSurface(texte_en, NULL, ecran, &pos_en);
        
        SDL_FreeSurface(texte_j1);
        SDL_FreeSurface(texte_j2);
        SDL_FreeSurface(texte_en);
    }
}

void activer_minimap(Minimap *m)
{
    m->actif = 1;
}

void desactiver_minimap(Minimap *m)
{
    m->actif = 0;
}

void toggle_minimap(Minimap *m)
{
    m->actif = !m->actif;
}

void free_minimap(Minimap *m)
{
    if(m->minimap) SDL_FreeSurface(m->minimap);
    if(m->point) SDL_FreeSurface(m->point);
    if(m->point_j2) SDL_FreeSurface(m->point_j2);
    if(m->point_ennemi) SDL_FreeSurface(m->point_ennemi);
    if(m->fond) SDL_FreeSurface(m->fond);
    if(m->police) TTF_CloseFont(m->police);
    
    m->minimap = NULL;
    m->point = NULL;
    m->point_j2 = NULL;
    m->point_ennemi = NULL;
    m->fond = NULL;
    m->police = NULL;
}
