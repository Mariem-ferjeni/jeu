#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "minimap.h"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 600
#define LARGEUR_MONDE 2000
#define HAUTEUR_MONDE 1200
#define VITESSE 5
#define GRAVITE 0.8
#define FORCE_SAUT -12

typedef struct {
    int x, y;
    int w, h;
    int vx, vy;
    bool sur_sol;
    int vie;
    SDL_Texture *texture;
} Player;

typedef struct {
    SDL_Rect rect;
    bool mobile;
    bool destructible;
    int dx, dy;
    int x_origine, amplitude;
    int vie;
    SDL_Texture *texture;
} Platform;

typedef struct {
    int x, y;
    int w, h;
    int vx;
    bool actif;
    SDL_Texture *texture;
} Enemy;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
TTF_Font *font = NULL;
Minimap minimap;
Player joueur;
Player joueur2;
Enemy ennemis[5];
int nb_ennemis = 0;
Platform platforms[30];
int nb_platforms = 0;
int niveau_actuel = 1;
bool jeu_en_cours = true;
bool pause = false;
bool multijoueur = true;

SDL_Texture *background_texture = NULL;
SDL_Texture *platform_texture = NULL;
SDL_Texture *platform_destructible_texture = NULL;
SDL_Texture *ennemi_texture = NULL;

void charger_textures() {
    SDL_Surface *tmp;
    
    // Charger background
    tmp = IMG_Load("background.png");
    if(tmp) {
        background_texture = SDL_CreateTextureFromSurface(renderer, tmp);
        SDL_FreeSurface(tmp);
        printf("OK: background.png charge\n");
    } else {
        printf("NON: background.png non trouve\n");
    }
    
    // Charger texture platform normale
    tmp = IMG_Load("platform.png");
    if(tmp) {
        platform_texture = SDL_CreateTextureFromSurface(renderer, tmp);
        SDL_FreeSurface(tmp);
        printf("OK: platform.png charge\n");
    } else {
        printf("NON: platform.png non trouve - rectangle marron\n");
    }
    
    // Charger texture platform destructible
    tmp = IMG_Load("platform_destructible.png");
    if(tmp) {
        platform_destructible_texture = SDL_CreateTextureFromSurface(renderer, tmp);
        SDL_FreeSurface(tmp);
        printf("OK: platform_destructible.png charge\n");
    } else {
        printf("NON: platform_destructible.png non trouve - rectangle orange\n");
    }
    
    // Charger texture ennemi
    tmp = IMG_Load("ennemi.png");
    if(tmp) {
        ennemi_texture = SDL_CreateTextureFromSurface(renderer, tmp);
        SDL_FreeSurface(tmp);
        printf("OK: ennemi.png charge\n");
    } else {
        printf("NON: ennemi.png non trouve - rectangle rouge\n");
    }
    
    // Charger texture joueur 1
    tmp = IMG_Load("joueur1.png");
    if(tmp) {
        joueur.texture = SDL_CreateTextureFromSurface(renderer, tmp);
        joueur.w = tmp->w;
        joueur.h = tmp->h;
        SDL_FreeSurface(tmp);
        printf("OK: joueur1.png charge (%dx%d)\n", joueur.w, joueur.h);
    } else {
        printf("NON: joueur1.png non trouve - rectangle vert\n");
        joueur.texture = NULL;
        joueur.w = 32;
        joueur.h = 32;
    }
    
    // Charger texture joueur 2
    if(multijoueur) {
        tmp = IMG_Load("joueur2.png");
        if(tmp) {
            joueur2.texture = SDL_CreateTextureFromSurface(renderer, tmp);
            joueur2.w = tmp->w;
            joueur2.h = tmp->h;
            SDL_FreeSurface(tmp);
            printf("OK: joueur2.png charge (%dx%d)\n", joueur2.w, joueur2.h);
        } else {
            printf("NON: joueur2.png non trouve - rectangle bleu\n");
            joueur2.texture = NULL;
            joueur2.w = 32;
            joueur2.h = 32;
        }
    }
}

void init_niveau1() {
    niveau_actuel = 1;
    nb_platforms = 0;
    nb_ennemis = 0;
    
    platforms[nb_platforms++] = (Platform){{0, 550, 2000, 20}, false, false, 0,0,0,0, 100, platform_texture};
    platforms[nb_platforms++] = (Platform){{500, 500, 150, 20}, false, false, 0,0,0,0, 100, platform_texture};
    platforms[nb_platforms++] = (Platform){{800, 450, 100, 20}, false, false, 0,0,0,0, 100, platform_texture};
    platforms[nb_platforms++] = (Platform){{1200, 400, 120, 20}, false, false, 0,0,0,0, 100, platform_texture};
    platforms[nb_platforms++] = (Platform){{1500, 350, 100, 20}, false, false, 0,0,0,0, 100, platform_texture};
    platforms[nb_platforms++] = (Platform){{600, 400, 80, 20}, true, false, 2, 600, 100, 100, platform_texture};
    platforms[nb_platforms++] = (Platform){{1000, 300, 60, 20}, false, true, 0,0,0,0, 50, platform_destructible_texture};
    
    joueur.x = 100;
    joueur.y = 500;
    joueur.vx = 0;
    joueur.vy = 0;
    joueur.sur_sol = true;
    joueur.vie = 100;
    
    joueur2.x = 300;
    joueur2.y = 500;
    joueur2.vx = 0;
    joueur2.vy = 0;
    joueur2.sur_sol = true;
    joueur2.vie = 100;
    
    ennemis[nb_ennemis++] = (Enemy){800, 418, 32, 32, 1, true, ennemi_texture};
    ennemis[nb_ennemis++] = (Enemy){1300, 368, 32, 32, 1, true, ennemi_texture};
}

void init_niveau2() {
    niveau_actuel = 2;
    nb_platforms = 0;
    nb_ennemis = 0;
    
    platforms[nb_platforms++] = (Platform){{0, 550, 2000, 20}, false, false, 0,0,0,0, 100, platform_texture};
    for(int i = 0; i < 6; i++) {
        platforms[nb_platforms++] = (Platform){{i * 250, 500 - (i % 3) * 60, 100, 20}, false, false, 0,0,0,0, 100, platform_texture};
    }
    platforms[nb_platforms++] = (Platform){{500, 400, 80, 20}, true, false, 3, 500, 150, 100, platform_texture};
    
    joueur.x = 100;
    joueur.y = 500;
    joueur.vx = 0;
    joueur.vy = 0;
    joueur.sur_sol = true;
    joueur.vie = 100;
    
    joueur2.x = 200;
    joueur2.y = 500;
    joueur2.vx = 0;
    joueur2.vy = 0;
    joueur2.sur_sol = true;
    joueur2.vie = 100;
    
    ennemis[nb_ennemis++] = (Enemy){1000, 318, 32, 32, 2, true, ennemi_texture};
    ennemis[nb_ennemis++] = (Enemy){1500, 468, 32, 32, 2, true, ennemi_texture};
}

void update_platforms() {
    for(int i = 0; i < nb_platforms; i++) {
        Platform *p = &platforms[i];
        if(p->mobile) {
            p->rect.x += p->dx;
            if(p->dx > 0 && p->rect.x > p->x_origine + p->amplitude) p->dx = -p->dx;
            if(p->dx < 0 && p->rect.x < p->x_origine - p->amplitude) p->dx = -p->dx;
        }
    }
}

void update_player(Player *p, int left, int right, int jump) {
    p->vy += GRAVITE;
    if(left) p->vx = -VITESSE;
    else if(right) p->vx = VITESSE;
    else p->vx *= 0.9;
    
    p->x += p->vx;
    p->y += p->vy;
    
    if(p->x < 0) p->x = 0;
    if(p->x > LARGEUR_MONDE - p->w) p->x = LARGEUR_MONDE - p->w;
    if(p->y > HAUTEUR_MONDE - p->h) {
        p->y = HAUTEUR_MONDE - p->h;
        p->vy = 0;
        p->sur_sol = true;
    }
    if(p->y < 0) {
        p->y = 0;
        p->vy = 0;
    }
    
    p->sur_sol = false;
    
    SDL_Rect player_rect = {p->x, p->y, p->w, p->h};
    for(int i = 0; i < nb_platforms; i++) {
        Platform *plat = &platforms[i];
        if(plat->destructible && plat->vie <= 0) continue;
        
        SDL_Rect plat_rect = plat->rect;
        if(SDL_HasIntersection(&player_rect, &plat_rect)) {
            if(p->vy > 0 && p->y + p->h - p->vy <= plat_rect.y + 10) {
                p->y = plat_rect.y - p->h;
                p->vy = 0;
                p->sur_sol = true;
                if(plat->destructible) plat->vie -= 25;
            }
            else if(p->vy < 0 && p->y >= plat_rect.y + plat_rect.h) {
                p->y = plat_rect.y + plat_rect.h;
                p->vy = 0;
            }
            else {
                if(p->vx > 0) p->x = plat_rect.x - p->w;
                else if(p->vx < 0) p->x = plat_rect.x + plat_rect.w;
            }
        }
    }
    
    if(jump && p->sur_sol) {
        p->vy = FORCE_SAUT;
        p->sur_sol = false;
    }
}

void update_ennemis() {
    for(int i = 0; i < nb_ennemis; i++) {
        if(ennemis[i].actif) {
            if(ennemis[i].x < joueur.x) ennemis[i].x += ennemis[i].vx;
            else if(ennemis[i].x > joueur.x) ennemis[i].x -= ennemis[i].vx;
            
            if(ennemis[i].x < 0) ennemis[i].x = 0;
            if(ennemis[i].x > LARGEUR_MONDE - ennemis[i].w) ennemis[i].x = LARGEUR_MONDE - ennemis[i].w;
            
            SDL_Rect player_rect = {joueur.x, joueur.y, joueur.w, joueur.h};
            SDL_Rect enemy_rect = {ennemis[i].x, ennemis[i].y, ennemis[i].w, ennemis[i].h};
            
            if(SDL_HasIntersection(&player_rect, &enemy_rect)) {
                if(joueur.vy > 0 && joueur.y + joueur.h - joueur.vy <= ennemis[i].y + 20) {
                    ennemis[i].actif = false;
                    joueur.vy = -8;
                } else {
                    joueur.vie -= 10;
                    joueur.x -= (joueur.x < ennemis[i].x ? -50 : 50);
                    joueur.vy = -5;
                    if(joueur.vie <= 0) jeu_en_cours = false;
                }
            }
        }
    }
}

void sauvegarder_jeu() {
    FILE *f = fopen("savegame.dat", "wb");
    if(f) {
        fwrite(&joueur, sizeof(Player), 1, f);
        fwrite(&niveau_actuel, sizeof(int), 1, f);
        fwrite(platforms, sizeof(Platform), nb_platforms, f);
        fwrite(&nb_platforms, sizeof(int), 1, f);
        fclose(f);
        printf("Jeu sauvegarde!\n");
    }
}

void charger_jeu() {
    FILE *f = fopen("savegame.dat", "rb");
    if(f) {
        fread(&joueur, sizeof(Player), 1, f);
        fread(&niveau_actuel, sizeof(int), 1, f);
        fread(platforms, sizeof(Platform), 30, f);
        fread(&nb_platforms, sizeof(int), 1, f);
        fclose(f);
        printf("Jeu charge!\n");
    }
}

void afficher_menu_pause() {
    pause = true;
    int choix = 0;
    SDL_Event e;
    
    while(pause) {
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) { pause = false; jeu_en_cours = false; }
            if(e.type == SDL_KEYDOWN) {
                switch(e.key.keysym.sym) {
                    case SDLK_ESCAPE: pause = false; break;
                    case SDLK_UP: choix = (choix - 1 + 4) % 4; break;
                    case SDLK_DOWN: choix = (choix + 1) % 4; break;
                    case SDLK_RETURN:
                        if(choix == 0) pause = false;
                        else if(choix == 1) sauvegarder_jeu();
                        else if(choix == 2) charger_jeu();
                        else if(choix == 3) jeu_en_cours = false;
                        break;
                }
            }
        }
        
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
        SDL_Rect rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderFillRect(renderer, &rect);
        
        SDL_Color white = {255, 255, 255};
        SDL_Surface *surf = TTF_RenderText_Blended(font, "PAUSE", white);
        SDL_Texture *text = SDL_CreateTextureFromSurface(renderer, surf);
        rect = (SDL_Rect){SCREEN_WIDTH/2 - 50, 100, 100, 50};
        SDL_RenderCopy(renderer, text, NULL, &rect);
        SDL_FreeSurface(surf);
        SDL_DestroyTexture(text);
        
        const char *options[] = {"Continuer", "Sauvegarder", "Charger", "Quitter"};
        for(int i = 0; i < 4; i++) {
            surf = TTF_RenderText_Blended(font, options[i], white);
            text = SDL_CreateTextureFromSurface(renderer, surf);
            rect = (SDL_Rect){SCREEN_WIDTH/2 - 80, 200 + i * 60, 160, 40};
            SDL_RenderCopy(renderer, text, NULL, &rect);
            SDL_FreeSurface(surf);
            SDL_DestroyTexture(text);
        }
        
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
}

void render() {
    SDL_RenderClear(renderer);
    
    // Afficher background
    if(background_texture) {
        SDL_Rect dest = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, background_texture, NULL, &dest);
    } else {
        SDL_SetRenderDrawColor(renderer, 100, 150, 200, 255);
        SDL_RenderFillRect(renderer, NULL);
    }
    
    SDL_Rect camera = {joueur.x + joueur.w/2 - SCREEN_WIDTH/2, 
                       joueur.y + joueur.h/2 - SCREEN_HEIGHT/2, 
                       SCREEN_WIDTH, SCREEN_HEIGHT};
    
    if(camera.x < 0) camera.x = 0;
    if(camera.x > LARGEUR_MONDE - SCREEN_WIDTH) camera.x = LARGEUR_MONDE - SCREEN_WIDTH;
    if(camera.y < 0) camera.y = 0;
    if(camera.y > HAUTEUR_MONDE - SCREEN_HEIGHT) camera.y = HAUTEUR_MONDE - SCREEN_HEIGHT;
    
    // Afficher les platforms avec textures
    for(int i = 0; i < nb_platforms; i++) {
        Platform *p = &platforms[i];
        if(!p->destructible || (p->destructible && p->vie > 0)) {
            SDL_Rect rect = {p->rect.x - camera.x, p->rect.y - camera.y, p->rect.w, p->rect.h};
            
            if(p->texture) {
                SDL_RenderCopy(renderer, p->texture, NULL, &rect);
            } else {
                if(p->destructible) SDL_SetRenderDrawColor(renderer, 255, 200 - p->vie, 100, 255);
                else SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255);
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
    
    // Afficher les ennemis avec textures
    for(int i = 0; i < nb_ennemis; i++) {
        if(ennemis[i].actif) {
            SDL_Rect rect = {ennemis[i].x - camera.x, ennemis[i].y - camera.y, ennemis[i].w, ennemis[i].h};
            
            if(ennemis[i].texture) {
                SDL_RenderCopy(renderer, ennemis[i].texture, NULL, &rect);
            } else {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
    
    // Afficher joueur 1 avec texture
    SDL_Rect rect_j1 = {joueur.x - camera.x, joueur.y - camera.y, joueur.w, joueur.h};
    if(joueur.texture) {
        SDL_RenderCopy(renderer, joueur.texture, NULL, &rect_j1);
    } else {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &rect_j1);
    }
    
    // Afficher joueur 2 avec texture
    if(multijoueur) {
        SDL_Rect rect_j2 = {joueur2.x - camera.x, joueur2.y - camera.y, joueur2.w, joueur2.h};
        if(joueur2.texture) {
            SDL_RenderCopy(renderer, joueur2.texture, NULL, &rect_j2);
        } else {
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
            SDL_RenderFillRect(renderer, &rect_j2);
        }
    }
    
    // Minimap
    int x_ennemis[10], y_ennemis[10];
    int nb_e = 0;
    for(int i = 0; i < nb_ennemis; i++) {
        if(ennemis[i].actif) {
            x_ennemis[nb_e] = ennemis[i].x;
            y_ennemis[nb_e] = ennemis[i].y;
            nb_e++;
        }
    }
    
    update_minimap(&minimap, joueur.x, joueur.y, joueur2.x, joueur2.y, x_ennemis, y_ennemis, nb_e);
    afficher_minimap(&minimap, renderer, nb_e);
    
    // Afficher informations
    if(font) {
        char texte[100];
        sprintf(texte, "Vie: %d | Niveau: %d", joueur.vie, niveau_actuel);
        SDL_Surface *surf = TTF_RenderText_Blended(font, texte, (SDL_Color){255,255,255});
        SDL_Texture *text = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_Rect rect_vie = {10, 10, 250, 30};
        SDL_RenderCopy(renderer, text, NULL, &rect_vie);
        SDL_FreeSurface(surf);
        SDL_DestroyTexture(text);
    }
    
    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
    
    window = SDL_CreateWindow("Jeu Mini-Map", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    font = TTF_OpenFont("arial.ttf", 24);
    
    // Charger toutes les textures
    charger_textures();
    
    init_minimap(&minimap, renderer, LARGEUR_MONDE, HAUTEUR_MONDE, multijoueur);
    init_niveau1();
    
    int left1=0, right1=0, jump1=0;
    int left2=0, right2=0, jump2=0;
    Uint32 dernier_temps = SDL_GetTicks();
    
    while(jeu_en_cours) {
        Uint32 temps_actuel = SDL_GetTicks();
        if(temps_actuel - dernier_temps < 16) continue;
        dernier_temps = temps_actuel;
        
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) jeu_en_cours = false;
            if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) afficher_menu_pause();
            
            if(e.type == SDL_KEYDOWN) {
                switch(e.key.keysym.sym) {
                    case SDLK_LEFT: left1 = 1; break;
                    case SDLK_RIGHT: right1 = 1; break;
                    case SDLK_SPACE: jump1 = 1; break;
                    case SDLK_q: left2 = 1; break;
                    case SDLK_d: right2 = 1; break;
                    case SDLK_z: jump2 = 1; break;
                }
            }
            if(e.type == SDL_KEYUP) {
                switch(e.key.keysym.sym) {
                    case SDLK_LEFT: left1 = 0; break;
                    case SDLK_RIGHT: right1 = 0; break;
                    case SDLK_SPACE: jump1 = 0; break;
                    case SDLK_q: left2 = 0; break;
                    case SDLK_d: right2 = 0; break;
                    case SDLK_z: jump2 = 0; break;
                }
            }
        }
        
        update_player(&joueur, left1, right1, jump1);
        if(multijoueur) update_player(&joueur2, left2, right2, jump2);
        update_ennemis();
        update_platforms();
        
        if(joueur.x > LARGEUR_MONDE - 100 && niveau_actuel == 1) init_niveau2();
        
        render();
        SDL_Delay(16);
    }
    
    // Nettoyage
    if(background_texture) SDL_DestroyTexture(background_texture);
    if(platform_texture) SDL_DestroyTexture(platform_texture);
    if(platform_destructible_texture) SDL_DestroyTexture(platform_destructible_texture);
    if(ennemi_texture) SDL_DestroyTexture(ennemi_texture);
    if(joueur.texture) SDL_DestroyTexture(joueur.texture);
    if(joueur2.texture) SDL_DestroyTexture(joueur2.texture);
    
    free_minimap(&minimap, renderer);
    if(font) TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    
    return 0;
}
