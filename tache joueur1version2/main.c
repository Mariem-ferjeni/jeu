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
    int x, y, w, h;
    int vx, vy;
    bool sur_sol;
    int vie;
    SDL_Texture *texture;
} Player;

typedef struct {
    SDL_Rect rect;
    bool mobile;
    bool destructible;
    int dx;
    int x_origine, amplitude;
    int vie;
    SDL_Texture *texture;
} Platform;

typedef struct {
    int x, y, w, h;
    int vx;
    bool actif;
    SDL_Texture *texture;
} Enemy;

/* GLOBALS */
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
TTF_Font *font = NULL;

Player joueur, joueur2;
Enemy ennemis[5];
Platform platforms[30];

int nb_ennemis = 0;
int nb_platforms = 0;
int niveau_actuel = 1;

bool jeu_en_cours = true;
bool multijoueur = true;

/* TEXTURES */
SDL_Texture *background_texture = NULL;
SDL_Texture *platform_texture = NULL;
SDL_Texture *platform_destructible_texture = NULL;
SDL_Texture *ennemi_texture = NULL;

/* ================= TEXTURES ================= */
void charger_textures() {
    SDL_Surface *tmp;

    tmp = IMG_Load("background.png");
    if(tmp) {
        background_texture = SDL_CreateTextureFromSurface(renderer, tmp);
        SDL_FreeSurface(tmp);
    }

    tmp = IMG_Load("platform.png");
    if(tmp) {
        platform_texture = SDL_CreateTextureFromSurface(renderer, tmp);
        SDL_FreeSurface(tmp);
    }

    tmp = IMG_Load("platform_destructible.png");
    if(tmp) {
        platform_destructible_texture = SDL_CreateTextureFromSurface(renderer, tmp);
        SDL_FreeSurface(tmp);
    }

    tmp = IMG_Load("ennemi.png");
    if(tmp) {
        ennemi_texture = SDL_CreateTextureFromSurface(renderer, tmp);
        SDL_FreeSurface(tmp);
    }

    tmp = IMG_Load("joueur1.png");
    if(tmp) {
        joueur.texture = SDL_CreateTextureFromSurface(renderer, tmp);
        joueur.w = tmp->w;
        joueur.h = tmp->h;
        SDL_FreeSurface(tmp);
    } else {
        joueur.w = joueur.h = 32;
    }

    tmp = IMG_Load("joueur2.png");
    if(tmp) {
        joueur2.texture = SDL_CreateTextureFromSurface(renderer, tmp);
        joueur2.w = tmp->w;
        joueur2.h = tmp->h;
        SDL_FreeSurface(tmp);
    } else {
        joueur2.w = joueur2.h = 32;
    }
}

/* ================= NIVEAU ================= */
void init_niveau1() {
    nb_platforms = 0;
    nb_ennemis = 0;

    platforms[nb_platforms++] = (Platform){{0,550,2000,20},false,false,0,0,0,100,platform_texture};
    platforms[nb_platforms++] = (Platform){{600,400,80,20},true,false,2,600,100,100,platform_texture};

    joueur.x = 100; joueur.y = 500; joueur.vx = joueur.vy = 0; joueur.vie = 100;
    joueur2.x = 300; joueur2.y = 500; joueur2.vx = joueur2.vy = 0; joueur2.vie = 100;

    ennemis[nb_ennemis++] = (Enemy){800,418,32,32,1,true,ennemi_texture};
}

/* ================= UPDATE ================= */
void update_platforms() {
    for(int i=0;i<nb_platforms;i++){
        Platform *p=&platforms[i];
        if(p->mobile){
            p->rect.x += p->dx;
            if(p->rect.x > p->x_origine + p->amplitude || p->rect.x < p->x_origine - p->amplitude)
                p->dx = -p->dx;
        }
    }
}

void update_player(Player *p,int left,int right,int jump){
    p->vy += GRAVITE;

    if(left) p->vx = -VITESSE;
    else if(right) p->vx = VITESSE;
    else p->vx *= 0.9;

    p->x += p->vx;
    p->y += p->vy;

    if(p->y > HAUTEUR_MONDE - p->h){
        p->y = HAUTEUR_MONDE - p->h;
        p->vy = 0;
        p->sur_sol = true;
    }

    if(jump && p->sur_sol){
        p->vy = FORCE_SAUT;
        p->sur_sol = false;
    }
}

/* ================= ENNEMIS ================= */
void update_ennemis(){
    for(int i=0;i<nb_ennemis;i++){
        if(ennemis[i].actif){
            if(ennemis[i].x < joueur.x) ennemis[i].x += ennemis[i].vx;
            else ennemis[i].x -= ennemis[i].vx;
        }
    }
}

/* ================= RENDER ================= */
void render(){
    SDL_RenderClear(renderer);

    if(background_texture)
        SDL_RenderCopy(renderer, background_texture, NULL, NULL);

    SDL_Rect r1 = {joueur.x,joueur.y,joueur.w,joueur.h};
    SDL_Rect r2 = {joueur2.x,joueur2.y,joueur2.w,joueur2.h};

    if(joueur.texture) SDL_RenderCopy(renderer,joueur.texture,NULL,&r1);
    else { SDL_SetRenderDrawColor(renderer,0,255,0,255); SDL_RenderFillRect(renderer,&r1); }

    if(joueur2.texture) SDL_RenderCopy(renderer,joueur2.texture,NULL,&r2);
    else { SDL_SetRenderDrawColor(renderer,0,0,255,255); SDL_RenderFillRect(renderer,&r2); }

    SDL_RenderPresent(renderer);
}

/* ================= MAIN ================= */
int main(){
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    window = SDL_CreateWindow("Jeu SDL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);

    charger_textures();
    init_niveau1();

    while(jeu_en_cours){
        SDL_Event e;
        while(SDL_PollEvent(&e)){
            if(e.type==SDL_QUIT) jeu_en_cours=false;
        }

        /* 🔥 CLAVIER FIX */
        const Uint8 *state = SDL_GetKeyboardState(NULL);

        int left1  = state[SDL_SCANCODE_LEFT];
        int right1 = state[SDL_SCANCODE_RIGHT];
        int jump1  = state[SDL_SCANCODE_SPACE];

        int left2  = state[SDL_SCANCODE_Q];
        int right2 = state[SDL_SCANCODE_D];
        int jump2  = state[SDL_SCANCODE_Z];

        update_player(&joueur,left1,right1,jump1);
        if(multijoueur)
            update_player(&joueur2,left2,right2,jump2);

        update_ennemis();
        update_platforms();

        render();
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
