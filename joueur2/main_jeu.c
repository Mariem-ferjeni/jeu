#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define GRAVITY 0.6
#define JUMP_POWER -12
#define ANIMATION_DELAY 100
#define MOVE_SPEED 5.5

typedef struct {
    SDL_Rect rect;
    SDL_Texture *walk_frames[4];
    SDL_Texture *jump_frames[5];
    SDL_Texture *still_texture;
    int current_frame;
    int current_jump_frame;
    float velX, velY;
    int onGround;
    int direction;
    int jumping;
    Uint32 last_anim_time;
    Uint32 last_jump_anim_time;
    int was_moving;
    int texture_loaded;
} Personnage;

SDL_Texture* loadTexture(SDL_Renderer *renderer, const char *path) {
    char full_path[256];
    sprintf(full_path, "assets/images/%s", path);
    SDL_Surface *surf = IMG_Load(full_path);
    if (!surf) {
        printf("Erreur chargement %s: %s\n", full_path, IMG_GetError());
        return NULL;
    }
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    return tex;
}

void initPersonnage(Personnage *p, SDL_Renderer *renderer) {
    p->rect.x = 100;
    p->rect.y = SCREEN_HEIGHT - 200;
    p->rect.w = 150;
    p->rect.h = 150;
    p->velX = 0;
    p->velY = 0;
    p->onGround = 1;
    p->direction = 1;
    p->jumping = 0;
    p->current_frame = 0;
    p->current_jump_frame = 0;
    p->last_anim_time = 0;
    p->last_jump_anim_time = 0;
    p->was_moving = 0;
    p->texture_loaded = 0;
    
    p->walk_frames[0] = loadTexture(renderer, "run.png");
    p->walk_frames[1] = loadTexture(renderer, "run1.png");
    p->walk_frames[2] = loadTexture(renderer, "run2.png");
    p->walk_frames[3] = NULL;
    
    p->jump_frames[0] = loadTexture(renderer, "jump2.png");
    p->jump_frames[1] = loadTexture(renderer, "jump3.png");
    p->jump_frames[2] = loadTexture(renderer, "jump4.png");
    p->jump_frames[3] = loadTexture(renderer, "jump5.png");
    p->jump_frames[4] = NULL;
    
    p->still_texture = loadTexture(renderer, "still.png");
    if (!p->still_texture) {
        p->still_texture = loadTexture(renderer, "still1.png");
    }
    
    if (p->walk_frames[0] || p->walk_frames[1] || p->walk_frames[2] || 
        p->jump_frames[0] || p->still_texture) {
        p->texture_loaded = 1;
        printf("Personnage initialisé avec succès!\n");
    } else {
        printf("ERREUR: Aucune texture chargée pour le personnage!\n");
    }
}

void updatePersonnage(Personnage *p, int screen_width, int ground_y, Uint32 dt) {
    p->velY += GRAVITY;
    p->rect.y += p->velY;
    p->rect.x += p->velX;
    
    if (p->rect.y + p->rect.h >= ground_y) {
        p->rect.y = ground_y - p->rect.h;
        p->velY = 0;
        p->onGround = 1;
        p->jumping = 0;
        p->current_jump_frame = 0;
    } else {
        p->onGround = 0;
        if (p->velY < 0) p->jumping = 1;
    }
    
    if (p->rect.x < 0) p->rect.x = 0;
    if (p->rect.x + p->rect.w > screen_width) p->rect.x = screen_width - p->rect.w;
    
    if (p->onGround && p->velX != 0) {
        p->last_anim_time += dt;
        if (p->last_anim_time >= ANIMATION_DELAY) {
            p->current_frame = (p->current_frame + 1) % 3;
            p->last_anim_time = 0;
        }
    } else {
        p->current_frame = 0;
    }
    
    if (p->jumping && !p->onGround) {
        p->last_jump_anim_time += dt;
        if (p->last_jump_anim_time >= ANIMATION_DELAY) {
            p->current_jump_frame = (p->current_jump_frame + 1) % 4;
            p->last_jump_anim_time = 0;
        }
    }
}

void renderPersonnage(SDL_Renderer *renderer, Personnage *p) {
    SDL_Texture *texture = NULL;
    
    if (!p->texture_loaded) {
        SDL_SetRenderDrawColor(renderer, 100, 100, 255, 255);
        SDL_RenderFillRect(renderer, &p->rect);
        return;
    }
    
    if (p->jumping && !p->onGround) {
        if (p->jump_frames[p->current_jump_frame]) {
            texture = p->jump_frames[p->current_jump_frame];
        } else if (p->jump_frames[0]) {
            texture = p->jump_frames[0];
        }
    } else if (p->velX != 0 && p->onGround) {
        if (p->walk_frames[p->current_frame]) {
            texture = p->walk_frames[p->current_frame];
        } else if (p->walk_frames[0]) {
            texture = p->walk_frames[0];
        }
    } else {
        if (p->still_texture) {
            texture = p->still_texture;
        }
    }
    
    if (texture) {
        SDL_Rect dest = p->rect;
        if (p->direction == -1) {
            SDL_RenderCopyEx(renderer, texture, NULL, &dest, 0, NULL, SDL_FLIP_HORIZONTAL);
        } else {
            SDL_RenderCopy(renderer, texture, NULL, &dest);
        }
    } else {
        SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255);
        SDL_RenderFillRect(renderer, &p->rect);
    }
}

// Fonction pour afficher les vies avec vies.png (plus grandes)
void renderVies(SDL_Renderer *renderer, int vies, int x, int y, int taille) {
    SDL_Texture *vieTex = loadTexture(renderer, "vies.png");
    if (vieTex) {
        // Récupérer les dimensions originales
        int w, h;
        SDL_QueryTexture(vieTex, NULL, NULL, &w, &h);
        
        // Redimensionner à la taille demandée
        for (int i = 0; i < vies; i++) {
            SDL_Rect dest = {x + i * (taille + 12), y, taille, taille};
            SDL_RenderCopy(renderer, vieTex, NULL, &dest);
        }
        SDL_DestroyTexture(vieTex);
    } else {
        // Fallback si l'image n'existe pas
        for (int i = 0; i < vies; i++) {
            SDL_Rect dest = {x + i * (taille + 8), y, taille, taille};
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderFillRect(renderer, &dest);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(renderer, &dest);
        }
    }
}

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("Erreur SDL: %s\n", SDL_GetError());
        return 1;
    }
    
    if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) == 0) {
        printf("Erreur IMG: %s\n", IMG_GetError());
        return 1;
    }
    
    if (TTF_Init() == -1) {
        printf("Erreur TTF: %s\n", TTF_GetError());
    }
    
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0) {
        printf("Erreur audio: %s\n", Mix_GetError());
    }
    
    Mix_AllocateChannels(16);
    
    SDL_Window *window = SDL_CreateWindow("Mon Jeu",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    
    if (!window) {
        printf("Erreur création fenêtre\n");
        return 1;
    }
    
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    SDL_Texture *background = loadTexture(renderer, "bg1.png");
    if (!background) {
        printf("Fond bg1.png non trouvé, utilisation fond bleu\n");
    }
    
    TTF_Font *font = TTF_OpenFont("assets/arial.ttf", 24);
    if (!font) font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 24);
    
    Personnage perso;
    initPersonnage(&perso, renderer);
    
    // ========== CHARGEMENT DES SONS ==========
    printf("\n=== CHARGEMENT DES SONS ===\n");
    
    // Musique de fond (sound2.wav)
    Mix_Music *bgMusic = Mix_LoadMUS("assets/sounds/sound2.wav");
    if (bgMusic) {
        Mix_PlayMusic(bgMusic, -1);
        printf("✓ Musique de fond: sound2.wav\n");
    } else {
        printf("✗ sound2.wav non trouvé\n");
    }
    
    // Son de course (bruit.mp3)
    Mix_Chunk *runSound = Mix_LoadWAV("assets/sounds/bruit.mp3");
    if (runSound) {
        printf("✓ Son de course: bruit.mp3\n");
        Mix_VolumeChunk(runSound, 64);
    } else {
        printf("✗ bruit.mp3 non trouvé\n");
    }
    
    // Son de saut (jump.wav)
    Mix_Chunk *jumpSound = Mix_LoadWAV("assets/sounds/jump.wav");
    if (jumpSound) {
        printf("✓ Son de saut: jump.wav\n");
        Mix_VolumeChunk(jumpSound, 80);
    } else {
        printf("✗ jump.wav non trouvé\n");
    }
    
    int wasMoving = 0;
    int runChannel = -1;
    
    int quit = 0;
    SDL_Event event;
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    int ground_y = SCREEN_HEIGHT - 170;
    int score = 0;
    int vies = 3;
    Uint32 startTime = SDL_GetTicks();
    Uint32 lastTime = SDL_GetTicks();
    
    printf("\n=== JEU DEMARRE ===\n");
    printf("Controles:\n");
    printf("  A / ← : Gauche (plus rapide)\n");
    printf("  D / → : Droite (plus rapide)\n");
    printf("  ESPACE / ↑ : Sauter\n");
    printf("  ESC : Quitter\n\n");
    
    while (!quit) {
        Uint32 currentTime = SDL_GetTicks();
        Uint32 dt = currentTime - lastTime;
        if (dt > 50) dt = 16;
        lastTime = currentTime;
        
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) quit = 1;
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
        }
        
        int isMoving = 0;
        perso.velX = 0;
        
        if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A]) {
            perso.velX = -MOVE_SPEED;
            perso.direction = -1;
            isMoving = 1;
        }
        if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D]) {
            perso.velX = MOVE_SPEED;
            perso.direction = 1;
            isMoving = 1;
        }
        
        // Gestion son de course
        if (isMoving && perso.onGround && !perso.jumping) {
            if (!wasMoving && runSound) {
                runChannel = Mix_PlayChannel(-1, runSound, -1);
                wasMoving = 1;
            }
        } else {
            if (wasMoving) {
                if (runChannel != -1) {
                    Mix_HaltChannel(runChannel);
                    runChannel = -1;
                }
                wasMoving = 0;
            }
        }
        
        // Saut
        if ((keys[SDL_SCANCODE_SPACE] || keys[SDL_SCANCODE_UP]) && perso.onGround) {
            perso.velY = JUMP_POWER;
            perso.onGround = 0;
            perso.jumping = 1;
            
            if (jumpSound) {
                Mix_PlayChannel(-1, jumpSound, 0);
            }
            
            if (wasMoving && runChannel != -1) {
                Mix_HaltChannel(runChannel);
                runChannel = -1;
                wasMoving = 0;
            }
        }
        
        updatePersonnage(&perso, SCREEN_WIDTH, ground_y, dt);
        score = (currentTime - startTime) / 1000;
        
        SDL_RenderClear(renderer);
        
        if (background) {
            SDL_RenderCopy(renderer, background, NULL, NULL);
        } else {
            SDL_SetRenderDrawColor(renderer, 100, 150, 200, 255);
            SDL_RenderClear(renderer);
        }
        
        // Sol rouge brique
        SDL_SetRenderDrawColor(renderer, 180, 60, 40, 255);
        SDL_Rect ground = {0, ground_y, SCREEN_WIDTH, 170};
        SDL_RenderFillRect(renderer, &ground);
        
        // Lignes de briques
        SDL_SetRenderDrawColor(renderer, 140, 40, 30, 255);
        for (int i = 0; i < SCREEN_WIDTH; i += 80) {
            SDL_Rect brickLine = {i, ground_y, 70, 8};
            SDL_RenderFillRect(renderer, &brickLine);
        }
        
        // Sol noir
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_Rect blackGround = {0, ground_y + 160, SCREEN_WIDTH, 60};
        SDL_RenderFillRect(renderer, &blackGround);
        
        // Personnage
        renderPersonnage(renderer, &perso);
        
        // Score
        if (font) {
            SDL_Color white = {255, 255, 255};
            char scoreText[50];
            sprintf(scoreText, "Score: %d", score);
            SDL_Surface *surf = TTF_RenderText_Blended(font, scoreText, white);
            if (surf) {
                SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
                SDL_Rect textRect = {10, 10, surf->w, surf->h};
                SDL_RenderCopy(renderer, tex, NULL, &textRect);
                SDL_FreeSurface(surf);
                SDL_DestroyTexture(tex);
            }
        }
        
        // Afficher les vies avec vies.png (plus grandes, taille 45)
        renderVies(renderer, vies, SCREEN_WIDTH - 110, 10, 45);
        
        // Instructions
        if (font) {
            SDL_Color gray = {200, 200, 200};
            char instText[] = "A/D: move (faster) | SPACE: jump | ESC: quit";
            SDL_Surface *surf = TTF_RenderText_Blended(font, instText, gray);
            if (surf) {
                SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
                SDL_Rect textRect = {10, SCREEN_HEIGHT - 40, surf->w, surf->h};
                SDL_RenderCopy(renderer, tex, NULL, &textRect);
                SDL_FreeSurface(surf);
                SDL_DestroyTexture(tex);
            }
        }
        
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    
    // Nettoyage
    if (background) SDL_DestroyTexture(background);
    for (int i = 0; i < 4; i++) {
        if (perso.walk_frames[i]) SDL_DestroyTexture(perso.walk_frames[i]);
    }
    for (int i = 0; i < 5; i++) {
        if (perso.jump_frames[i]) SDL_DestroyTexture(perso.jump_frames[i]);
    }
    if (perso.still_texture) SDL_DestroyTexture(perso.still_texture);
    if (font) TTF_CloseFont(font);
    if (bgMusic) Mix_FreeMusic(bgMusic);
    if (runSound) Mix_FreeChunk(runSound);
    if (jumpSound) Mix_FreeChunk(jumpSound);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    
    printf("\nJeu terminé.\n");
    return 0;
}
