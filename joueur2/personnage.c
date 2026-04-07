#include "personnage.h"
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <math.h>

int initPerso(Personnage *p, SDL_Renderer *renderer) {
    p->texture_walk = NULL;
    p->texture_jump = NULL;
    p->texture_vies = NULL;
    
    // Charger les textures
    SDL_Surface *surf = IMG_Load("walk.png");
    if(surf) {
        p->texture_walk = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);
        printf("walk.png chargé\n");
    } else {
        printf("Erreur: walk.png - %s\n", IMG_GetError());
        return -1;
    }
    
    surf = IMG_Load("jump.png");
    if(surf) {
        p->texture_jump = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);
        printf("jump.png chargé\n");
    } else {
        printf("Erreur: jump.png - %s\n", IMG_GetError());
        return -1;
    }
    
    surf = IMG_Load("vies.png");
    if(surf) {
        p->texture_vies = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);
        printf("vies.png chargé\n");
    }
    
    // Dimensions
    SDL_QueryTexture(p->texture_walk, NULL, NULL, &p->frame_width, &p->frame_height);
    p->frame_width = p->frame_width / 6;
    p->frame_count = 6;
    p->frame = 0;
    
    p->position.x = 100;
    p->position.y = 400;
    p->position.w = p->frame_width;
    p->position.h = p->frame_height;
    
    // Paramètres de mouvement
    p->vitesse = 0.0f;
    p->acceleration = 0.8f;      // Accélération plus rapide
    p->friction = 0.85f;         // Friction plus forte (arrêt plus rapide)
    p->vitesse_max = 7.0f;
    
    p->jumping = false;
    p->onGround = true;
    p->direction = 0;
    p->score = 0;
    p->vies = 3;
    p->velY = 0;
    p->gravity = 0.9f;
    p->jumpPower = -12.0f;
    
    // Timer pour l'animation de saut
    p->jumpTimer = 0;
    p->jumpFrame = 0;
    
    printf("Personnage initialisé\n");
    return 0;
}

void movePerso(Personnage *p, Uint32 dt, int screen_width, int ground_level) {
    float dt_sec = dt / 1000.0f;
    if(dt_sec > 0.033f) dt_sec = 0.033f; // Limiter à 30 FPS max
    
    // Si une touche est enfoncée, accélération immédiate
    if(p->direction != 0) {
        p->vitesse += p->acceleration * p->direction * dt_sec * 60;
    } 
    // Sinon, friction pour arrêter rapidement
    else {
        p->vitesse *= p->friction;
        if(fabs(p->vitesse) < 0.1f) p->vitesse = 0;
    }
    
    // Limiter la vitesse
    if(p->vitesse > p->vitesse_max) p->vitesse = p->vitesse_max;
    if(p->vitesse < -p->vitesse_max) p->vitesse = -p->vitesse_max;
    
    // Déplacement
    p->position.x += p->vitesse;
    
    // Collisions
    if(p->position.x < 0) {
        p->position.x = 0;
        p->vitesse = 0;
    }
    if(p->position.x + p->position.w > screen_width) {
        p->position.x = screen_width - p->position.w;
        p->vitesse = 0;
    }
}

void saut(Personnage *p) {
    if(p->onGround && !p->jumping) {
        p->jumping = true;
        p->onGround = false;
        p->velY = p->jumpPower;
        p->jumpTimer = 0;
        printf("Saut!\n");
    }
}

void updateJump(Personnage *p, int ground_level) {
    if(p->jumping) {
        // Gravité
        p->velY += p->gravity;
        p->position.y += p->velY;
        
        // Animation de saut (changer de frame pendant le saut)
        p->jumpTimer++;
        if(p->jumpTimer > 3) {
            p->jumpTimer = 0;
            p->jumpFrame = (p->jumpFrame + 1) % 3;
        }
        
        // Atterrissage
        if(p->position.y >= ground_level - p->position.h) {
            p->position.y = ground_level - p->position.h;
            p->jumping = false;
            p->onGround = true;
            p->velY = 0;
            p->jumpFrame = 0;
        }
        
        // Plafond
        if(p->position.y < 0) {
            p->position.y = 0;
            p->velY = 0;
        }
    }
}

void animerPerso(Personnage *p, double dt) {
    static double time_since_last_frame = 0;
    
    // Pas d'animation pendant le saut (on utilise jump.png)
    if(p->jumping) return;
    
    // Calcul du délai en fonction de la vitesse
    float frame_delay = 80.0f / (fabs(p->vitesse) + 1);
    if(frame_delay < 40) frame_delay = 40;
    if(frame_delay > 150) frame_delay = 150;
    
    time_since_last_frame += dt;
    
    // Animation seulement si le personnage se déplace
    if(time_since_last_frame >= frame_delay && fabs(p->vitesse) > 0.5f) {
        p->frame = (p->frame + 1) % p->frame_count;
        time_since_last_frame = 0;
    }
}

void afficherPerso(SDL_Renderer *renderer, Personnage *p) {
    SDL_Texture *texture;
    SDL_Rect src;
    SDL_Rect dest = p->position;
    
    // Pendant le saut, utiliser jump.png
    if(p->jumping) {
        texture = p->texture_jump;
        // Si jump.png est une spritesheet, on peut animer pendant le saut
        int jumpFrameWidth = p->frame_width;
        src = (SDL_Rect){ p->jumpFrame * jumpFrameWidth, 0, p->frame_width, p->frame_height };
    } else {
        texture = p->texture_walk;
        src = (SDL_Rect){ p->frame * p->frame_width, 0, p->frame_width, p->frame_height };
    }
    
    if(!texture) return;
    
    // Afficher selon la direction
    if(p->direction == -1) {
        SDL_RenderCopyEx(renderer, texture, &src, &dest, 0, NULL, SDL_FLIP_HORIZONTAL);
    } else {
        SDL_RenderCopy(renderer, texture, &src, &dest);
    }
}

void afficherVies(SDL_Renderer *renderer, Personnage *p, int x, int y, int taille) {
    if(p->texture_vies) {
        for(int i = 0; i < p->vies; i++) {
            SDL_Rect dest = { x + i * (taille + 5), y, taille, taille };
            SDL_RenderCopy(renderer, p->texture_vies, NULL, &dest);
        }
    } else {
        for(int i = 0; i < p->vies; i++) {
            SDL_Rect vieRect = { x + i * (taille + 5), y, taille, taille };
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderFillRect(renderer, &vieRect);
        }
    }
}

void cleanupPerso(Personnage *p) {
    if(p->texture_walk) SDL_DestroyTexture(p->texture_walk);
    if(p->texture_jump) SDL_DestroyTexture(p->texture_jump);
    if(p->texture_vies) SDL_DestroyTexture(p->texture_vies);
}
