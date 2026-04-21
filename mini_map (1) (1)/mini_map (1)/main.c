#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include "minimap.h"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 600
#define MONDE_W 2000
#define MONDE_H 1200
#define VITESSE 5
#define GRAVITE 0.8
#define FORCE_SAUT -12

typedef struct {
    int x, y, w, h;
    int type; 
    int actif;
} Obstacle;

SDL_Window *window;
SDL_Renderer *renderer;
TTF_Font *police;
struct Minimap minimap;

int j1_x = 100, j1_y = 500, j1_w = 50, j1_h = 50;
int j1_vx = 0, j1_vy = 0, j1_sol = 1, j1_vie = 100;
SDL_Texture *texture_j1 = NULL;

int j2_x = 300, j2_y = 500, j2_w = 50, j2_h = 50;
int j2_vx = 0, j2_vy = 0, j2_sol = 1, j2_vie = 100;
SDL_Texture *texture_j2 = NULL;

int ennemis_x[10] = {0}, ennemis_y[10] = {0}, ennemis_actif[10] = {0};
int ennemis_w = 50, ennemis_h = 50;
int nb_ennemis = 0;
SDL_Texture *texture_ennemi = NULL;

Obstacle obstacles[30];
int nb_obstacles = 0;
SDL_Texture *texture_spike = NULL;
SDL_Texture *texture_lave = NULL;
SDL_Texture *texture_aiguille = NULL;

int sol_y = 550;
int niveau = 1;
int jeu_actif = 1;
int multijoueur = 1;

SDL_Texture *bg1 = NULL, *bg2 = NULL, *bg_actuel = NULL;

SDL_Texture* charger_image_transparente(const char* chemin, int largeur, int hauteur)
{
    SDL_Surface *temp = IMG_Load(chemin);
    SDL_Surface *redim;
    SDL_Texture *texture = NULL;
    
    if(temp) {
        redim = SDL_CreateRGBSurface(0, largeur, hauteur, 32, 
                                      0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
        
        SDL_BlitScaled(temp, NULL, redim, NULL);
        
        SDL_SetSurfaceBlendMode(redim, SDL_BLENDMODE_BLEND);

        texture = SDL_CreateTextureFromSurface(renderer, redim);
 
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        
        SDL_FreeSurface(redim);
        SDL_FreeSurface(temp);
    }
    
    return texture;
}

void charger_toutes_les_images()
{
    SDL_Surface *temp;
    
    temp = IMG_Load("background_niveau1.png");
    if(temp) { bg1 = SDL_CreateTextureFromSurface(renderer, temp); SDL_FreeSurface(temp); }
    
    temp = IMG_Load("background_niveau2.png");
    if(temp) { bg2 = SDL_CreateTextureFromSurface(renderer, temp); SDL_FreeSurface(temp); }
    
    texture_j1 = charger_image_transparente("joueur1.png", 50, 50);
    if(!texture_j1) {
        SDL_Surface *surf = SDL_CreateRGBSurface(0, 50, 50, 32, 0,0,0,0);
        SDL_FillRect(surf, NULL, SDL_MapRGB(surf->format, 0, 255, 0));
        texture_j1 = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);
    }
    
    texture_j2 = charger_image_transparente("joueur2.png", 50, 50);
    if(!texture_j2) {
        SDL_Surface *surf = SDL_CreateRGBSurface(0, 50, 50, 32, 0,0,0,0);
        SDL_FillRect(surf, NULL, SDL_MapRGB(surf->format, 0, 0, 255));
        texture_j2 = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);
    }
    
    texture_ennemi = charger_image_transparente("ennemi.png", 50, 50);
    if(!texture_ennemi) {
        SDL_Surface *surf = SDL_CreateRGBSurface(0, 50, 50, 32, 0,0,0,0);
        SDL_FillRect(surf, NULL, SDL_MapRGB(surf->format, 255, 0, 0));
        texture_ennemi = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);
    }
    
    texture_spike = charger_image_transparente("spike.png", 60, 40);
    texture_lave = charger_image_transparente("lave.png", 120, 40);
    texture_aiguille = charger_image_transparente("aiguille.png", 50, 40);
    
    bg_actuel = bg1;
}

void niveau1_initialiser()
{
    niveau = 1;
    bg_actuel = bg1;
    
    j1_x = 100; j1_y = 500; j1_vie = 100;
    j2_x = 300; j2_y = 500; j2_vie = 100;
    j1_vy = 0; j2_vy = 0;
    
    ennemis_x[0] = 600; ennemis_y[0] = 500; ennemis_actif[0] = 1;
    ennemis_x[1] = 1000; ennemis_y[1] = 500; ennemis_actif[1] = 1;
    nb_ennemis = 2;
    
    nb_obstacles = 0;
    
    obstacles[nb_obstacles].x = 400; obstacles[nb_obstacles].y = 510;
    obstacles[nb_obstacles].w = 60; obstacles[nb_obstacles].h = 40;
    obstacles[nb_obstacles].type = 0; obstacles[nb_obstacles].actif = 1;
    nb_obstacles++;
    
    obstacles[nb_obstacles].x = 750; obstacles[nb_obstacles].y = 510;
    obstacles[nb_obstacles].w = 60; obstacles[nb_obstacles].h = 40;
    obstacles[nb_obstacles].type = 0; obstacles[nb_obstacles].actif = 1;
    nb_obstacles++;
    
    obstacles[nb_obstacles].x = 1200; obstacles[nb_obstacles].y = 510;
    obstacles[nb_obstacles].w = 120; obstacles[nb_obstacles].h = 40;
    obstacles[nb_obstacles].type = 1; obstacles[nb_obstacles].actif = 1;
    nb_obstacles++;
}

void niveau2_initialiser()
{
    niveau = 2;
    bg_actuel = bg2;
    
    j1_x = 100; j1_y = 500; j1_vie = 100;
    j2_x = 300; j2_y = 500; j2_vie = 100;
    j1_vy = 0; j2_vy = 0;
    
    ennemis_x[0] = 400; ennemis_y[0] = 500; ennemis_actif[0] = 1;
    ennemis_x[1] = 800; ennemis_y[1] = 500; ennemis_actif[1] = 1;
    ennemis_x[2] = 1300; ennemis_y[2] = 500; ennemis_actif[2] = 1;
    nb_ennemis = 3;
    
    nb_obstacles = 0;
    
    obstacles[nb_obstacles].x = 300; obstacles[nb_obstacles].y = 510;
    obstacles[nb_obstacles].w = 60; obstacles[nb_obstacles].h = 40;
    obstacles[nb_obstacles].type = 0; obstacles[nb_obstacles].actif = 1;
    nb_obstacles++;
    
    obstacles[nb_obstacles].x = 650; obstacles[nb_obstacles].y = 510;
    obstacles[nb_obstacles].w = 60; obstacles[nb_obstacles].h = 40;
    obstacles[nb_obstacles].type = 0; obstacles[nb_obstacles].actif = 1;
    nb_obstacles++;
    
    obstacles[nb_obstacles].x = 1000; obstacles[nb_obstacles].y = 510;
    obstacles[nb_obstacles].w = 50; obstacles[nb_obstacles].h = 40;
    obstacles[nb_obstacles].type = 2; obstacles[nb_obstacles].actif = 1;
    nb_obstacles++;
    
    obstacles[nb_obstacles].x = 1150; obstacles[nb_obstacles].y = 510;
    obstacles[nb_obstacles].w = 50; obstacles[nb_obstacles].h = 40;
    obstacles[nb_obstacles].type = 2; obstacles[nb_obstacles].actif = 1;
    nb_obstacles++;
    
    obstacles[nb_obstacles].x = 1500; obstacles[nb_obstacles].y = 510;
    obstacles[nb_obstacles].w = 140; obstacles[nb_obstacles].h = 40;
    obstacles[nb_obstacles].type = 1; obstacles[nb_obstacles].actif = 1;
    nb_obstacles++;
}

void update_joueur(int *x, int *y, int *vx, int *vy, int *sol, int w, int h, int gauche, int droite, int saut)
{
    *vy += GRAVITE;
    
    if(gauche) *vx = -VITESSE;
    else if(droite) *vx = VITESSE;
    else *vx *= 0.9;
    
    *x += *vx;
    *y += *vy;
    
    if(*x < 0) *x = 0;
    if(*x > MONDE_W - w) *x = MONDE_W - w;
    
    if(*y + h >= sol_y) {
        *y = sol_y - h;
        *vy = 0;
        *sol = 1;
    } else {
        *sol = 0;
    }
    
    if(*y < 0) { *y = 0; *vy = 0; }
    
    if(saut && *sol) {
        *vy = FORCE_SAUT;
        *sol = 0;
    }
}

void update_ennemis()
{
    for(int i = 0; i < nb_ennemis; i++) {
        if(ennemis_actif[i]) {
            if(ennemis_x[i] < j1_x) ennemis_x[i] += 2;
            else if(ennemis_x[i] > j1_x) ennemis_x[i] -= 2;
            
            SDL_Rect joueur = {j1_x, j1_y, j1_w, j1_h};
            SDL_Rect ennemi = {ennemis_x[i], ennemis_y[i], ennemis_w, ennemis_h};
            
            if(SDL_HasIntersection(&joueur, &ennemi)) {
                if(j1_vy > 0 && j1_y + j1_h - j1_vy <= ennemis_y[i] + 20) {
                    ennemis_actif[i] = 0;
                    j1_vy = -10;
                } else {
                    j1_vie -= 10;
                    if(j1_x < ennemis_x[i]) j1_x -= 50;
                    else j1_x += 50;
                    j1_vy = -8;
                    if(j1_vie <= 0) jeu_actif = 0;
                }
            }
            
            if(multijoueur) {
                SDL_Rect joueur2 = {j2_x, j2_y, j2_w, j2_h};
                if(SDL_HasIntersection(&joueur2, &ennemi)) {
                    if(j2_vy > 0 && j2_y + j2_h - j2_vy <= ennemis_y[i] + 20) {
                        ennemis_actif[i] = 0;
                        j2_vy = -10;
                    } else {
                        j2_vie -= 10;
                        if(j2_x < ennemis_x[i]) j2_x -= 50;
                        else j2_x += 50;
                        j2_vy = -8;
                    }
                }
            }
        }
    }
}

void update_obstacles()
{
    SDL_Rect joueur = {j1_x, j1_y, j1_w, j1_h};
    
    for(int i = 0; i < nb_obstacles; i++) {
        if(obstacles[i].actif) {
            SDL_Rect obs = {obstacles[i].x, obstacles[i].y, obstacles[i].w, obstacles[i].h};
            
            if(SDL_HasIntersection(&joueur, &obs)) {
                if(obstacles[i].type == 0) j1_vie -= 15;
                else if(obstacles[i].type == 1) j1_vie -= 25;
                else if(obstacles[i].type == 2) j1_vie -= 10;
                
                if(j1_x < obstacles[i].x) j1_x -= 40;
                else j1_x += 40;
                j1_vy = -8;
                if(j1_vie <= 0) jeu_actif = 0;
            }
        }
    }
    
    if(multijoueur) {
        SDL_Rect joueur2 = {j2_x, j2_y, j2_w, j2_h};
        for(int i = 0; i < nb_obstacles; i++) {
            if(obstacles[i].actif) {
                SDL_Rect obs = {obstacles[i].x, obstacles[i].y, obstacles[i].w, obstacles[i].h};
                if(SDL_HasIntersection(&joueur2, &obs)) {
                    if(obstacles[i].type == 0) j2_vie -= 15;
                    else if(obstacles[i].type == 1) j2_vie -= 25;
                    else if(obstacles[i].type == 2) j2_vie -= 10;
                    if(j2_x < obstacles[i].x) j2_x -= 40;
                    else j2_x += 40;
                    j2_vy = -8;
                }
            }
        }
    }
}

void afficher()
{
    SDL_Rect camera;
    camera.x = j1_x + j1_w/2 - SCREEN_WIDTH/2;
    camera.y = 0;
    camera.w = SCREEN_WIDTH;
    camera.h = SCREEN_HEIGHT;
    
    if(camera.x < 0) camera.x = 0;
    if(camera.x > MONDE_W - SCREEN_WIDTH) camera.x = MONDE_W - SCREEN_WIDTH;
    
    SDL_RenderClear(renderer);
    
    if(bg_actuel) {
        SDL_Rect bg_rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, bg_actuel, NULL, &bg_rect);
    } else {
        SDL_SetRenderDrawColor(renderer, 100, 150, 200, 255);
        SDL_RenderFillRect(renderer, NULL);
    }
    
    SDL_SetRenderDrawColor(renderer, 100, 80, 50, 255);
    SDL_Rect sol = {0 - camera.x, sol_y, MONDE_W, 10};
    SDL_RenderFillRect(renderer, &sol);
    
    for(int i = 0; i < nb_obstacles; i++) {
        if(obstacles[i].actif) {
            SDL_Rect rect = {obstacles[i].x - camera.x, obstacles[i].y, obstacles[i].w, obstacles[i].h};
            
            if(obstacles[i].type == 0 && texture_spike)
                SDL_RenderCopy(renderer, texture_spike, NULL, &rect);
            else if(obstacles[i].type == 1 && texture_lave)
                SDL_RenderCopy(renderer, texture_lave, NULL, &rect);
            else if(obstacles[i].type == 2 && texture_aiguille)
                SDL_RenderCopy(renderer, texture_aiguille, NULL, &rect);
            else {
                if(obstacles[i].type == 0) SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                else if(obstacles[i].type == 1) SDL_SetRenderDrawColor(renderer, 255, 100, 0, 255);
                else SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
    
    for(int i = 0; i < nb_ennemis; i++) {
        if(ennemis_actif[i]) {
            SDL_Rect rect = {ennemis_x[i] - camera.x, ennemis_y[i], ennemis_w, ennemis_h};
            if(texture_ennemi)
                SDL_RenderCopy(renderer, texture_ennemi, NULL, &rect);
            else {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
    
    if(multijoueur) {
        SDL_Rect rect_j2 = {j2_x - camera.x, j2_y, j2_w, j2_h};
        if(texture_j2)
            SDL_RenderCopy(renderer, texture_j2, NULL, &rect_j2);
        else {
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
            SDL_RenderFillRect(renderer, &rect_j2);
        }
    }
    
    SDL_Rect rect_j1 = {j1_x - camera.x, j1_y, j1_w, j1_h};
    if(texture_j1)
        SDL_RenderCopy(renderer, texture_j1, NULL, &rect_j1);
    else {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &rect_j1);
    }
    
    int ex[10] = {0}, ey[10] = {0};
    int nb_e = 0;
    for(int i = 0; i < nb_ennemis; i++) {
        if(ennemis_actif[i]) {
            ex[nb_e] = ennemis_x[i];
            ey[nb_e] = ennemis_y[i];
            nb_e++;
        }
    }
    minimap_update(&minimap, j1_x, j1_y, j2_x, j2_y, ex, ey, nb_e);
    minimap_draw(&minimap, renderer, nb_e);
    
    char texte[200];
    SDL_Color couleur = {255, 255, 255, 255};
    SDL_Surface *surface;
    SDL_Texture *texture_texte;
    
    if(multijoueur)
        sprintf(texte, "J1:%d  J2:%d  Niveau:%d  (N=changer niveau)", j1_vie, j2_vie, niveau);
    else
        sprintf(texte, "Vie:%d  Niveau:%d  (N=changer niveau)", j1_vie, niveau);
    
    surface = TTF_RenderText_Blended(police, texte, couleur);
    if(surface) {
        texture_texte = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect rect_texte = {10, 10, 500, 30};
        SDL_RenderCopy(renderer, texture_texte, NULL, &rect_texte);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture_texte);
    }
    
    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[])
{
    SDL_Event event;
    int gauche1=0, droite1=0, saut1=0;
    int gauche2=0, droite2=0, saut2=0;
    int quit = 0;
    
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
    
    window = SDL_CreateWindow("Jeu avec Mini-map", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    police = TTF_OpenFont("arial.ttf", 24);
    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    
    charger_toutes_les_images();
    minimap_init(&minimap, renderer, MONDE_W, MONDE_H, multijoueur);
    niveau1_initialiser();
    
    while(!quit && jeu_actif) {
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) quit = 1;
            
            if(event.type == SDL_KEYDOWN) {
                switch(event.key.keysym.sym) {
                    case SDLK_LEFT: gauche1 = 1; break;
                    case SDLK_RIGHT: droite1 = 1; break;
                    case SDLK_SPACE: saut1 = 1; break;
                    case SDLK_q: gauche2 = 1; break;
                    case SDLK_d: droite2 = 1; break;
                    case SDLK_z: saut2 = 1; break;
                    case SDLK_n:
                        if(niveau == 1) niveau2_initialiser();
                        else niveau1_initialiser();
                        break;
                }
            }
            
            if(event.type == SDL_KEYUP) {
                switch(event.key.keysym.sym) {
                    case SDLK_LEFT: gauche1 = 0; break;
                    case SDLK_RIGHT: droite1 = 0; break;
                    case SDLK_SPACE: saut1 = 0; break;
                    case SDLK_q: gauche2 = 0; break;
                    case SDLK_d: droite2 = 0; break;
                    case SDLK_z: saut2 = 0; break;
                }
            }
        }
        
        update_joueur(&j1_x, &j1_y, &j1_vx, &j1_vy, &j1_sol, j1_w, j1_h, gauche1, droite1, saut1);
        if(multijoueur)
            update_joueur(&j2_x, &j2_y, &j2_vx, &j2_vy, &j2_sol, j2_w, j2_h, gauche2, droite2, saut2);
        
        update_ennemis();
        update_obstacles();
        
        if(j1_vie <= 0) jeu_actif = 0;
        
        afficher();
        SDL_Delay(16);
    }
    
    if(bg1) SDL_DestroyTexture(bg1);
    if(bg2) SDL_DestroyTexture(bg2);
    if(texture_j1) SDL_DestroyTexture(texture_j1);
    if(texture_j2) SDL_DestroyTexture(texture_j2);
    if(texture_ennemi) SDL_DestroyTexture(texture_ennemi);
    if(texture_spike) SDL_DestroyTexture(texture_spike);
    if(texture_lave) SDL_DestroyTexture(texture_lave);
    if(texture_aiguille) SDL_DestroyTexture(texture_aiguille);
    
    minimap_liberer(&minimap, renderer);
    if(police) TTF_CloseFont(police);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    
    return 0;
}
