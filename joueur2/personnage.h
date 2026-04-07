#ifndef PERSONNAGE_H
#define PERSONNAGE_H

#include <SDL2/SDL.h>
#include <stdbool.h>

typedef struct {
    SDL_Rect position;
    SDL_Texture *texture_walk;
    SDL_Texture *texture_jump;
    SDL_Texture *texture_vies;
    int frame;
    int frame_count;
    int frame_width;
    int frame_height;
    float vitesse;
    float acceleration;
    float friction;
    float vitesse_max;
    bool jumping;
    bool onGround;
    int direction;
    int score;
    int vies;
    float velY;
    float gravity;
    float jumpPower;
    int jumpTimer;
    int jumpFrame;
} Personnage;

int initPerso(Personnage *p, SDL_Renderer *renderer);
void movePerso(Personnage *p, Uint32 dt, int screen_width, int ground_level);
void animerPerso(Personnage *p, double dt);
void afficherPerso(SDL_Renderer *renderer, Personnage *p);
void afficherVies(SDL_Renderer *renderer, Personnage *p, int x, int y, int taille);
void saut(Personnage *p);
void updateJump(Personnage *p, int ground_level);
void cleanupPerso(Personnage *p);

#endif
