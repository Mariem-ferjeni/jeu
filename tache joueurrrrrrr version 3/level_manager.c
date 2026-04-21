#include "level_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


int show_instructions = 0;
int btn_instructions_hover = 0;
int niveau_actuel = 1;
int transition_phase = 0;
Uint32 transition_time = 0;
int is_fullscreen = 0;
int game_over_triggered = 0;
int cinematic_wait = 0;

float scale_x = 1.0f;
float scale_y = 1.0f;
int screen_width = SCREEN_WIDTH;
int screen_height = SCREEN_HEIGHT;

int sol_y = REF_SOL_Y;
int perso_w = REF_PERSO_LARGEUR;
int perso_h = REF_PERSO_HAUTEUR;
int barre_w = REF_BARRE_LARGEUR;
int barre_h = REF_BARRE_HAUTEUR;
float current_move_speed = MOVE_SPEED;
float current_bomb_speed = BOMB_SPEED;

SDL_Texture *g_bomb_texture = NULL;
SDL_Texture *g_ball_texture = NULL;
SDL_Texture *g_instructions_texture = NULL;
SDL_Texture *g_win_p1_texture = NULL;
SDL_Texture *g_win_p2_texture = NULL;
SDL_Texture *g_cinematic1 = NULL;
SDL_Texture *g_cinematic2 = NULL;
SDL_Texture *g_vies_texture = NULL;

Mix_Chunk *g_attackSound1 = NULL;
Mix_Chunk *g_attackSound2 = NULL;
Mix_Chunk *g_victorySound = NULL;
Mix_Chunk *g_jumpSound = NULL;
Mix_Chunk *g_runSound = NULL;
Mix_Music *g_bgMusic = NULL;


void initLevelManager(LevelManager *lm, SDL_Renderer *renderer) { (void)lm; (void)renderer; }
void loadLevel(LevelManager *lm, int level_num) { (void)lm; (void)level_num; }
void updateLevel(LevelManager *lm, SDL_Renderer *renderer, int px, int py, int pw, int ph) { (void)lm; (void)renderer; (void)px; (void)py; (void)pw; (void)ph; }
void renderDoor(LevelManager *lm, SDL_Renderer *renderer) { (void)lm; (void)renderer; }
const char* getCurrentBackgroundPath(LevelManager *lm) { (void)lm; return NULL; }
void resetLevelManager(LevelManager *lm) { (void)lm; }
void cleanupLevelManager(LevelManager *lm) { (void)lm; }

SDL_Texture* loadTexture(SDL_Renderer *renderer, const char *path) {
    SDL_Surface *surf = IMG_Load(path);
    if (!surf) return NULL;
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    return tex;
}

void updateScale(SDL_Renderer *renderer) {
    SDL_GetRendererOutputSize(renderer, &screen_width, &screen_height);
    scale_x = (float)screen_width / SCREEN_WIDTH;
    scale_y = (float)screen_height / SCREEN_HEIGHT;
    sol_y = (int)(REF_SOL_Y * scale_y);
    perso_w = (int)(REF_PERSO_LARGEUR * scale_x);
    perso_h = (int)(REF_PERSO_HAUTEUR * scale_y);
    barre_w = (int)(REF_BARRE_LARGEUR * scale_x);
    barre_h = (int)(REF_BARRE_HAUTEUR * scale_y);
    current_move_speed = MOVE_SPEED * scale_x;
    current_bomb_speed = BOMB_SPEED * scale_x;
}

void toggleFullscreen(SDL_Window *window, SDL_Renderer *renderer) {
    if (is_fullscreen) {
        SDL_SetWindowFullscreen(window, 0);
        is_fullscreen = 0;
    } else {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
        is_fullscreen = 1;
    }
    updateScale(renderer);
}

void initPersonnage1(Personnage *p, SDL_Renderer *renderer, int startX) {
    int i;
    p->id = 1;
    p->rect.x = (int)(startX * scale_x);
    p->rect.y = sol_y - perso_h;
    p->rect.w = perso_w;
    p->rect.h = perso_h;
    p->velX = 0; p->velY = 0;
    p->onGround = 1; p->direction = 1;
    p->jumping = 0; p->attacking = 0; p->crouching = 0;
    p->dying = 0; p->dying_animation_done = 0;
    p->current_frame = 0; p->current_jump_frame = 0;
    p->current_attack_frame = 0; p->current_fall_frame = 0;
    p->last_anim_time = 0; p->last_jump_anim_time = 0;
    p->last_attack_time = 0; p->last_attack_used = 0;
    p->last_fall_time = 0;
    p->vie = 100; p->vie_max = 100;
    p->score = 0;
    p->frame_count = 4; p->jump_frame_count = 4;
    p->attack_frame_count = 4; p->fall_frame_count = 6;
    
    for (i = 0; i < 8; i++) p->walk_frames[i] = NULL;
    for (i = 0; i < 5; i++) p->jump_frames[i] = NULL;
    for (i = 0; i < 5; i++) p->attack_frames[i] = NULL;
    for (i = 0; i < 6; i++) p->fall_frames[i] = NULL;
    
    p->walk_frames[0] = loadTexture(renderer, "assets/images/run.png");
    p->walk_frames[1] = loadTexture(renderer, "assets/images/run1.png");
    p->walk_frames[2] = loadTexture(renderer, "assets/images/run2.png");
    p->walk_frames[3] = loadTexture(renderer, "assets/images/run3.png");
    p->jump_frames[0] = loadTexture(renderer, "assets/images/jump2.png");
    p->jump_frames[1] = loadTexture(renderer, "assets/images/jump3.png");
    p->jump_frames[2] = loadTexture(renderer, "assets/images/jump4.png");
    p->jump_frames[3] = loadTexture(renderer, "assets/images/jump5.png");
    p->attack_frames[0] = loadTexture(renderer, "assets/images/j1attack1.png");
    p->attack_frames[1] = loadTexture(renderer, "assets/images/j1attack2.png");
    p->attack_frames[2] = loadTexture(renderer, "assets/images/j1attack3.png");
    p->attack_frames[3] = loadTexture(renderer, "assets/images/j1attack4.png");
    p->still_texture = loadTexture(renderer, "assets/images/still.png");
    if (!p->still_texture) p->still_texture = loadTexture(renderer, "assets/images/still1.png");
    p->crouch_texture = loadTexture(renderer, "assets/images/j1crouch.png");
    p->fall_frames[0] = loadTexture(renderer, "assets/images/j1fall1.png");
    p->fall_frames[1] = loadTexture(renderer, "assets/images/j1fall2.png");
    p->fall_frames[2] = loadTexture(renderer, "assets/images/j1fall3.png");
    p->fall_frames[3] = loadTexture(renderer, "assets/images/j1fall4.png");
    p->fall_frames[4] = loadTexture(renderer, "assets/images/j1fall5.png");
    p->fall_frames[5] = loadTexture(renderer, "assets/images/j1fall6.png");
    printf("Joueur 1 initialise\n");
}

void initPersonnage2(Personnage *p, SDL_Renderer *renderer, int startX) {
    int i;
    p->id = 2;
    p->rect.x = (int)(startX * scale_x);
    p->rect.y = sol_y - perso_h;
    p->rect.w = perso_w;
    p->rect.h = perso_h;
    p->velX = 0; p->velY = 0;
    p->onGround = 1; p->direction = 1;
    p->jumping = 0; p->attacking = 0; p->crouching = 0;
    p->dying = 0; p->dying_animation_done = 0;
    p->current_frame = 0; p->current_jump_frame = 0;
    p->current_attack_frame = 0; p->current_fall_frame = 0;
    p->last_anim_time = 0; p->last_jump_anim_time = 0;
    p->last_attack_time = 0; p->last_attack_used = 0;
    p->last_fall_time = 0;
    p->vie = 100; p->vie_max = 100;
    p->score = 0;
    p->frame_count = 7; p->jump_frame_count = 2;
    p->attack_frame_count = 4; p->fall_frame_count = 4;
    
    for (i = 0; i < 8; i++) p->walk_frames[i] = NULL;
    for (i = 0; i < 5; i++) p->jump_frames[i] = NULL;
    for (i = 0; i < 5; i++) p->attack_frames[i] = NULL;
    for (i = 0; i < 6; i++) p->fall_frames[i] = NULL;
    
    p->walk_frames[0] = loadTexture(renderer, "assets/images/j2run00.png");
    p->walk_frames[1] = loadTexture(renderer, "assets/images/j2run11.png");
    p->walk_frames[2] = loadTexture(renderer, "assets/images/j2run22.png");
    p->walk_frames[3] = loadTexture(renderer, "assets/images/j2run33.png");
    p->walk_frames[4] = loadTexture(renderer, "assets/images/j2run44.png");
    p->walk_frames[5] = loadTexture(renderer, "assets/images/j2run55.png");
    p->walk_frames[6] = loadTexture(renderer, "assets/images/j2run66.png");
    p->jump_frames[0] = loadTexture(renderer, "assets/images/j2jump2.png");
    p->jump_frames[1] = loadTexture(renderer, "assets/images/j2jump3.png");
    p->attack_frames[0] = loadTexture(renderer, "assets/images/j2attack1.png");
    p->attack_frames[1] = loadTexture(renderer, "assets/images/j2attack2.png");
    p->attack_frames[2] = loadTexture(renderer, "assets/images/j2attack3.png");
    p->attack_frames[3] = loadTexture(renderer, "assets/images/j2attack4.png");
    
    p->still_texture = loadTexture(renderer, "assets/images/j2run00.png");
    if (!p->still_texture) p->still_texture = loadTexture(renderer, "assets/images/still.png");
    p->crouch_texture = loadTexture(renderer, "assets/images/j2crouch.png");
    p->fall_frames[0] = loadTexture(renderer, "assets/images/j2fall1.png");
    p->fall_frames[1] = loadTexture(renderer, "assets/images/j2fall2.png");
    p->fall_frames[2] = loadTexture(renderer, "assets/images/j2fall3.png");
    p->fall_frames[3] = loadTexture(renderer, "assets/images/j2fall4.png");
    printf("Joueur 2 initialise\n");
}

void initBarresMobiles(BarreMobile barres[]) {
    barres[0].rect = (SDL_Rect){(int)(150 * scale_x), sol_y - (int)(100 * scale_y), barre_w, barre_h};
    barres[0].vitesseX = 2; barres[0].direction = 1; barres[0].actif = 1;
    barres[0].touched_by_p1 = 0; barres[0].touched_by_p2 = 0;
    
    barres[1].rect = (SDL_Rect){(int)(350 * scale_x), sol_y - (int)(140 * scale_y), barre_w, barre_h};
    barres[1].vitesseX = 2; barres[1].direction = -1; barres[1].actif = 1;
    barres[1].touched_by_p1 = 0; barres[1].touched_by_p2 = 0;
    
    barres[2].rect = (SDL_Rect){(int)(550 * scale_x), sol_y - (int)(120 * scale_y), barre_w, barre_h};
    barres[2].vitesseX = 2; barres[2].direction = 1; barres[2].actif = 1;
    barres[2].touched_by_p1 = 0; barres[2].touched_by_p2 = 0;
}

void updateBarresMobiles(BarreMobile barres[]) {
    for (int i = 0; i < MAX_BARRES; i++) {
        if (!barres[i].actif) continue;
        barres[i].rect.x += (int)(barres[i].vitesseX * barres[i].direction * scale_x);
        if (barres[i].rect.x < (int)(50 * scale_x)) {
            barres[i].rect.x = (int)(50 * scale_x);
            barres[i].direction = 1;
        }
        if (barres[i].rect.x + barres[i].rect.w > screen_width - (int)(50 * scale_x)) {
            barres[i].rect.x = screen_width - (int)(50 * scale_x) - barres[i].rect.w;
            barres[i].direction = -1;
        }
    }
}

void renderBarresMobiles(SDL_Renderer *renderer, BarreMobile barres[]) {
    for (int i = 0; i < MAX_BARRES; i++) {
        if (!barres[i].actif) continue;
        SDL_SetRenderDrawColor(renderer, 120, 120, 130, 255);
        SDL_RenderFillRect(renderer, &barres[i].rect);
        SDL_SetRenderDrawColor(renderer, 80, 80, 90, 255);
        SDL_RenderDrawRect(renderer, &barres[i].rect);
        SDL_SetRenderDrawColor(renderer, 200, 200, 220, 255);
        SDL_Rect top = {barres[i].rect.x, barres[i].rect.y, barres[i].rect.w, (int)(3 * scale_y)};
        SDL_RenderFillRect(renderer, &top);
        if (barres[i].touched_by_p1) {
            SDL_SetRenderDrawColor(renderer, 0, 100, 255, 200);
            SDL_Rect marker = {barres[i].rect.x + (int)(5 * scale_x), barres[i].rect.y - (int)(10 * scale_y), (int)(10 * scale_x), (int)(6 * scale_y)};
            SDL_RenderFillRect(renderer, &marker);
        }
        if (barres[i].touched_by_p2) {
            SDL_SetRenderDrawColor(renderer, 255, 50, 50, 200);
            SDL_Rect marker = {barres[i].rect.x + (int)(25 * scale_x), barres[i].rect.y - (int)(10 * scale_y), (int)(10 * scale_x), (int)(6 * scale_y)};
            SDL_RenderFillRect(renderer, &marker);
        }
    }
}

void updatePersonnageAvecBarres(Personnage *p, Uint32 dt, BarreMobile barres[]) {
    if (p->dying) {
        p->last_fall_time += dt;
        if (p->last_fall_time >= ANIMATION_DELAY) {
            p->current_fall_frame++;
            p->last_fall_time = 0;
            if (p->current_fall_frame >= p->fall_frame_count) p->dying_animation_done = 1;
        }
        return;
    }
    
    p->velY += GRAVITY;
    p->rect.y += (int)p->velY;
    p->rect.x += (int)p->velX;
    p->onGround = 0;
    
    for (int i = 0; i < MAX_BARRES; i++) {
        if (!barres[i].actif) continue;
        if (p->rect.x + p->rect.w > barres[i].rect.x &&
            p->rect.x < barres[i].rect.x + barres[i].rect.w &&
            p->rect.y + p->rect.h >= barres[i].rect.y &&
            p->rect.y + p->rect.h <= barres[i].rect.y + barres[i].rect.h + (int)(15 * scale_y) &&
            p->velY > 0) {
            p->rect.y = barres[i].rect.y - p->rect.h;
            p->velY = 0;
            p->onGround = 1;
            p->jumping = 0;
            if (p->id == 1 && !barres[i].touched_by_p1) {
                barres[i].touched_by_p1 = 1;
                p->score += 10;
                printf("Joueur 1 a touche la barre %d! Score: %d\n", i, p->score);
            }
            if (p->id == 2 && !barres[i].touched_by_p2) {
                barres[i].touched_by_p2 = 1;
                p->score += 10;
                printf("Joueur 2 a touche la barre %d! Score: %d\n", i, p->score);
            }
            break;
        }
    }
    
    if (p->rect.y + p->rect.h >= sol_y) {
        p->rect.y = sol_y - p->rect.h;
        p->velY = 0;
        p->onGround = 1;
        p->jumping = 0;
    }
    if (p->rect.y < 0) { p->rect.y = 0; if (p->velY < 0) p->velY = 0; }
    if (p->rect.x < 0) p->rect.x = 0;
    if (p->rect.x + p->rect.w > screen_width) p->rect.x = screen_width - p->rect.w;
    
    if (!p->crouching && p->onGround && p->velX != 0 && !p->attacking && !p->jumping) {
        p->last_anim_time += dt;
        if (p->last_anim_time >= ANIMATION_DELAY) {
            p->current_frame = (p->current_frame + 1) % p->frame_count;
            p->last_anim_time = 0;
        }
    } else if (!p->attacking && !p->jumping && !p->crouching) p->current_frame = 0;
    
    if (p->jumping && !p->onGround && !p->attacking) {
        p->last_jump_anim_time += dt;
        if (p->last_jump_anim_time >= ANIMATION_DELAY) {
            p->current_jump_frame = (p->current_jump_frame + 1) % p->jump_frame_count;
            p->last_jump_anim_time = 0;
        }
    }
    
    if (p->attacking) {
        p->last_attack_time += dt;
        if (p->last_attack_time >= ANIMATION_DELAY) {
            p->current_attack_frame++;
            p->last_attack_time = 0;
            if (p->current_attack_frame >= p->attack_frame_count) {
                p->attacking = 0;
                p->current_attack_frame = 0;
            }
        }
    }
}

void renderPersonnage(SDL_Renderer *renderer, Personnage *p) {
    SDL_Texture *texture = NULL;
    SDL_Rect dest = p->rect;
    if (p->dying) {
        if (p->current_fall_frame < p->fall_frame_count && p->fall_frames[p->current_fall_frame])
            texture = p->fall_frames[p->current_fall_frame];
        if (texture) {
            if (p->direction == -1) SDL_RenderCopyEx(renderer, texture, NULL, &dest, 0, NULL, SDL_FLIP_HORIZONTAL);
            else SDL_RenderCopy(renderer, texture, NULL, &dest);
        }
        return;
    }
    
    if (p->crouching && p->crouch_texture) {
        texture = p->crouch_texture;
        dest.h = (int)(60 * scale_y);
        dest.y = p->rect.y + (int)(20 * scale_y);
    } else if (p->attacking && p->attack_frames[p->current_attack_frame])
        texture = p->attack_frames[p->current_attack_frame];
    else if (p->jumping && !p->onGround && p->jump_frames[p->current_jump_frame])
        texture = p->jump_frames[p->current_jump_frame];
    else if (p->velX != 0 && p->onGround && p->walk_frames[p->current_frame])
        texture = p->walk_frames[p->current_frame];
    else if (p->still_texture)
        texture = p->still_texture;
    else if (p->walk_frames[0])
        texture = p->walk_frames[0];
    
    if (texture) {
        if (p->direction == -1) SDL_RenderCopyEx(renderer, texture, NULL, &dest, 0, NULL, SDL_FLIP_HORIZONTAL);
        else SDL_RenderCopy(renderer, texture, NULL, &dest);
    } else {
        SDL_SetRenderDrawColor(renderer, p->id == 1 ? 0 : 255, p->id == 1 ? 100 : 50, p->id == 1 ? 255 : 50, 255);
        SDL_RenderFillRect(renderer, &p->rect);
    }
    
    if (!p->dying) {
        float pourcentage = (float)p->vie / p->vie_max;
        int largeur_vie = (int)(p->rect.w * pourcentage);
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_Rect barre_vie = {p->rect.x, p->rect.y - (int)(12 * scale_y), largeur_vie, (int)(6 * scale_y)};
        SDL_RenderFillRect(renderer, &barre_vie);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_Rect contour = {p->rect.x, p->rect.y - (int)(12 * scale_y), p->rect.w, (int)(6 * scale_y)};
        SDL_RenderDrawRect(renderer, &contour);
    }
}

void creerProjectile(Projectile projectiles[], int *nb_projectiles, int x, int y, int direction, int owner_id, int type) {
    if (*nb_projectiles < 20) {
        projectiles[*nb_projectiles].rect.x = x + (direction == 1 ? (int)(70 * scale_x) : -(int)(40 * scale_x));
        projectiles[*nb_projectiles].rect.y = y + (int)(35 * scale_y);
        projectiles[*nb_projectiles].rect.w = (int)(30 * scale_x);
        projectiles[*nb_projectiles].rect.h = (int)(30 * scale_y);
        projectiles[*nb_projectiles].direction = direction;
        projectiles[*nb_projectiles].active = 1;
        projectiles[*nb_projectiles].owner_id = owner_id;
        projectiles[*nb_projectiles].type = type;
        (*nb_projectiles)++;
    }
}

void updateProjectiles(Projectile projectiles[], int *nb_projectiles, Personnage *p1, Personnage *p2) {
    for (int i = 0; i < *nb_projectiles; i++) {
        if (projectiles[i].active) {
            projectiles[i].rect.x += (int)(current_bomb_speed * projectiles[i].direction);
            if (projectiles[i].rect.x < -50 || projectiles[i].rect.x > screen_width + 50) {
                projectiles[i].active = 0;
            }
            if (projectiles[i].owner_id != 1 && !p1->dying && SDL_HasIntersection(&projectiles[i].rect, &p1->rect)) {
                p1->vie -= 15;
                projectiles[i].active = 0;
                printf("[Niveau %d] Joueur 1 touche! Vies: %d\n", niveau_actuel, p1->vie);
                if (p1->vie <= 0 && !p1->dying && !game_over_triggered) {
                    p1->dying = 1;
                    p1->current_fall_frame = 0;
                    p1->last_fall_time = 0;
                    game_over_triggered = 1;
                    if (g_victorySound) {
                        Mix_PlayChannel(-1, g_victorySound, 0);
                        printf(">>> SON DE VICTOIRE JOUE (Joueur 1 mort) <<<\n");
                    }
                    Mix_HaltMusic();
                }
            }
            if (projectiles[i].owner_id != 2 && !p2->dying && SDL_HasIntersection(&projectiles[i].rect, &p2->rect)) {
                p2->vie -= 15;
                projectiles[i].active = 0;
                printf("[Niveau %d] Joueur 2 touche! Vies: %d\n", niveau_actuel, p2->vie);
                if (p2->vie <= 0 && !p2->dying && !game_over_triggered) {
                    p2->dying = 1;
                    p2->current_fall_frame = 0;
                    p2->last_fall_time = 0;
                    game_over_triggered = 1;
                    if (g_victorySound) {
                        Mix_PlayChannel(-1, g_victorySound, 0);
                        printf(">>> SON DE VICTOIRE JOUE (Joueur 2 mort) <<<\n");
                    }
                    Mix_HaltMusic();
                }
            }
        }
    }
    int j = 0;
    for (int i = 0; i < *nb_projectiles; i++) {
        if (projectiles[i].active) projectiles[j++] = projectiles[i];
    }
    *nb_projectiles = j;
}

void renderProjectiles(SDL_Renderer *renderer, Projectile projectiles[], int nb) {
    for (int i = 0; i < nb; i++) {
        if (projectiles[i].active) {
            if (projectiles[i].type == 0 && g_bomb_texture) {
                SDL_RenderCopy(renderer, g_bomb_texture, NULL, &projectiles[i].rect);
            } else if (projectiles[i].type == 1 && g_ball_texture) {
                SDL_RenderCopy(renderer, g_ball_texture, NULL, &projectiles[i].rect);
            } else {
                SDL_SetRenderDrawColor(renderer, projectiles[i].type == 0 ? 0 : 255, 
                                       projectiles[i].type == 0 ? 0 : 200, 
                                       projectiles[i].type == 0 ? 0 : 0, 255);
                SDL_RenderFillRect(renderer, &projectiles[i].rect);
            }
        }
    }
}

void renderGround(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 34, 68, 34, 255);
    SDL_Rect ground = {0, sol_y, screen_width, screen_height - sol_y};
    SDL_RenderFillRect(renderer, &ground);
    SDL_SetRenderDrawColor(renderer, 50, 100, 50, 255);
    SDL_Rect border = {0, sol_y, screen_width, (int)(4 * scale_y)};
    SDL_RenderFillRect(renderer, &border);
}

void renderInstructionsButton(SDL_Renderer *renderer, TTF_Font *font) {
    int btn_x = screen_width - (int)(40 * scale_x);
    int btn_y = screen_height - (int)(40 * scale_y);
    int btn_w = (int)(30 * scale_x);
    int btn_h = (int)(30 * scale_y);
    SDL_Rect btn = {btn_x, btn_y, btn_w, btn_h};
    
    if (btn_instructions_hover) SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
    else SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &btn);
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
    SDL_RenderDrawRect(renderer, &btn);
    
    if (font) {
        SDL_Surface *surf = TTF_RenderText_Blended(font, "?", (SDL_Color){255, 255, 255, 255});
        if (surf) {
            SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_Rect textRect = {btn_x + (btn_w - surf->w)/2, btn_y + (btn_h - surf->h)/2, surf->w, surf->h};
            SDL_RenderCopy(renderer, tex, NULL, &textRect);
            SDL_FreeSurface(surf);
            SDL_DestroyTexture(tex);
        }
    }
}

void renderInstructionsBox(SDL_Renderer *renderer) {
    if (!g_instructions_texture) return;
    int box_w = (int)(200 * scale_x);
    int box_h = (int)(150 * scale_y);
    int box_x = screen_width - box_w - (int)(10 * scale_x);
    int box_y = screen_height - box_h - (int)(50 * scale_y);
    SDL_Rect box = {box_x, box_y, box_w, box_h};
    SDL_RenderCopy(renderer, g_instructions_texture, NULL, &box);
}

void renderScores(SDL_Renderer *renderer, TTF_Font *font, Personnage *p1, Personnage *p2) {
    if (!font) return;
    SDL_Color blue = {0, 100, 255, 255};
    SDL_Color red = {255, 50, 50, 255};
    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
    SDL_Rect scorePanel = {(int)(10 * scale_x), (int)(10 * scale_y), (int)(180 * scale_x), (int)(70 * scale_y)};
    SDL_RenderFillRect(renderer, &scorePanel);
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderDrawRect(renderer, &scorePanel);
    
    char scoreText1[50], scoreText2[50];
    sprintf(scoreText1, "J1 Score: %d", p1->score);
    sprintf(scoreText2, "J2 Score: %d", p2->score);
    
    SDL_Surface *surf1 = TTF_RenderText_Blended(font, scoreText1, blue);
    SDL_Surface *surf2 = TTF_RenderText_Blended(font, scoreText2, red);
    if (surf1) {
        SDL_Texture *tex1 = SDL_CreateTextureFromSurface(renderer, surf1);
        SDL_Rect r1 = {(int)(15 * scale_x), (int)(15 * scale_y), surf1->w, surf1->h};
        SDL_RenderCopy(renderer, tex1, NULL, &r1);
        SDL_FreeSurface(surf1);
        SDL_DestroyTexture(tex1);
    }
    if (surf2) {
        SDL_Texture *tex2 = SDL_CreateTextureFromSurface(renderer, surf2);
        SDL_Rect r2 = {(int)(15 * scale_x), (int)(45 * scale_y), surf2->w, surf2->h};
        SDL_RenderCopy(renderer, tex2, NULL, &r2);
        SDL_FreeSurface(surf2);
        SDL_DestroyTexture(tex2);
    }
}

void renderVies(SDL_Renderer *renderer, Personnage *p1, Personnage *p2) {
    if (!g_vies_texture) return;
    int icon_size = (int)(35 * scale_x);
    int spacing = (int)(5 * scale_x);
    int start_x = screen_width - (icon_size + spacing) * 5 - (int)(10 * scale_x);
    int y_pos = (int)(10 * scale_y);
    int vies1 = p1->vie / 20;
    int vies2 = p2->vie / 20;
    if (vies1 < 0) vies1 = 0;
    if (vies2 < 0) vies2 = 0;
    for (int i = 0; i < vies1 && i < 5; i++) {
        SDL_Rect dest = {start_x + i * (icon_size + spacing), y_pos, icon_size, icon_size};
        SDL_RenderCopy(renderer, g_vies_texture, NULL, &dest);
    }
    for (int i = 0; i < vies2 && i < 5; i++) {
        SDL_Rect dest = {start_x + i * (icon_size + spacing), y_pos + icon_size + (int)(5 * scale_y), icon_size, icon_size};
        SDL_RenderCopy(renderer, g_vies_texture, NULL, &dest);
    }
}

void renderGameOver(SDL_Renderer *renderer, int gagnant) {
    SDL_Texture *win_texture = NULL;
    if (gagnant == 1) win_texture = g_win_p1_texture;
    else if (gagnant == 2) win_texture = g_win_p2_texture;
    
    if (win_texture) {
        int img_w, img_h;
        SDL_QueryTexture(win_texture, NULL, NULL, &img_w, &img_h);
        int display_w = img_w, display_h = img_h;
        if (display_w > screen_width - (int)(100 * scale_x)) {
            float ratio = (float)display_h / display_w;
            display_w = screen_width - (int)(100 * scale_x);
            display_h = (int)(display_w * ratio);
        }
        if (display_h > screen_height - (int)(100 * scale_y)) {
            float ratio = (float)display_w / display_h;
            display_h = screen_height - (int)(100 * scale_y);
            display_w = (int)(display_h * ratio);
        }
        SDL_Rect dest = {screen_width/2 - display_w/2, screen_height/2 - display_h/2, display_w, display_h};
        SDL_RenderCopy(renderer, win_texture, NULL, &dest);
    } else {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
        SDL_Rect overlay = {0, 0, screen_width, screen_height};
        SDL_RenderFillRect(renderer, &overlay);
        TTF_Font *big_font = TTF_OpenFont("assets/arial.ttf", (int)(30 * scale_x));
        if (!big_font) big_font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", (int)(30 * scale_x));
        if (big_font) {
            char text[100];
            if (gagnant == 1) sprintf(text, "JOUEUR 1 GAGNE!");
            else if (gagnant == 2) sprintf(text, "JOUEUR 2 GAGNE!");
            else sprintf(text, "EGALITE!");
            SDL_Surface *surf = TTF_RenderText_Blended(big_font, text, (SDL_Color){255, 255, 0, 255});
            if (surf) {
                SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
                SDL_Rect r = {screen_width/2 - surf->w/2, screen_height/2 - 50, surf->w, surf->h};
                SDL_RenderCopy(renderer, tex, NULL, &r);
                SDL_FreeSurface(surf);
                SDL_DestroyTexture(tex);
            }
            TTF_CloseFont(big_font);
        }
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }
}

void jeuPrincipal(SDL_Window *window, SDL_Renderer *renderer) {
    LevelManager levelManager;
    SDL_Texture *bg1, *level1_img, *level2_img, *currentBackground;
    TTF_Font *font;
    Personnage joueur1, joueur2;
    BarreMobile barres[MAX_BARRES];
    Projectile projectiles[20];
    int nb_projectiles = 0, quit = 0, gameOver = 0, gagnant = 0;
    int wasMoving1 = 0, wasMoving2 = 0;
    int runChannel1 = -1, runChannel2 = -1;
    SDL_Event event;
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    Uint32 lastTime = SDL_GetTicks();
    
    updateScale(renderer);
    initLevelManager(&levelManager, renderer);
    
    bg1 = loadTexture(renderer, "assets/images/bg1.png");
    level1_img = loadTexture(renderer, "assets/images/level1.png");
    level2_img = loadTexture(renderer, "assets/images/level2.png");
    currentBackground = bg1;
    
    g_cinematic1 = loadTexture(renderer, "assets/images/level1cinematic.png");
    g_cinematic2 = loadTexture(renderer, "assets/images/level2cinematic.png");
    g_bomb_texture = loadTexture(renderer, "assets/images/bomb.png");
    g_ball_texture = loadTexture(renderer, "assets/images/ball.png");
    g_instructions_texture = loadTexture(renderer, "assets/images/instruction.png");
    g_win_p1_texture = loadTexture(renderer, "assets/images/gplayer1.png");
    g_win_p2_texture = loadTexture(renderer, "assets/images/gplayer2.png");
    g_vies_texture = loadTexture(renderer, "assets/images/vies.png");
    
    font = TTF_OpenFont("assets/arial.ttf", (int)(13 * scale_x));
    if (!font) font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", (int)(13 * scale_x));
    
    initPersonnage1(&joueur1, renderer, 100);
    initPersonnage2(&joueur2, renderer, 300);
    initBarresMobiles(barres);
    
    g_bgMusic = Mix_LoadMUS("assets/sounds/manhattanproject.mp3");
    if (g_bgMusic) Mix_PlayMusic(g_bgMusic, -1);
    
    g_attackSound1 = Mix_LoadWAV("assets/sounds/J1.mp3");
    if (!g_attackSound1) g_attackSound1 = Mix_LoadWAV("assets/sounds/j1.mp3");
    g_attackSound2 = Mix_LoadWAV("assets/sounds/J2.mp3");
    if (!g_attackSound2) g_attackSound2 = Mix_LoadWAV("assets/sounds/j2.mp3");
    g_victorySound = Mix_LoadWAV("assets/sounds/soundv.mp3");
    g_jumpSound = Mix_LoadWAV("assets/sounds/jump.wav");
    g_runSound = Mix_LoadWAV("assets/sounds/bruit.mp3");
    
    printf("\n========================================\n");
    printf("=== JEU DEMARRE ===\n");
    printf("JOUEUR 1: Q/D/Z | ESPACE | F | W\n");
    printf("JOUEUR 2: FLECHES | K | FLECHE BAS\n");
    printf("CTR/F11: Plein ecran | R: Recommencer | ESC: Quitter\n");
    printf("ESPACE: Passer les cinématiques\n");
    printf("========================================\n\n");
    
   
    cinematic_wait = 1;
    while (cinematic_wait && !quit) {
        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT) quit = 1;
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) cinematic_wait = 0;
        SDL_RenderClear(renderer);
        if (g_cinematic1) {
            SDL_Rect dest = {0, 0, screen_width, screen_height};
            SDL_RenderCopy(renderer, g_cinematic1, NULL, &dest);
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    
 
    while (!quit) {
        Uint32 currentTime = SDL_GetTicks();
        Uint32 dt = currentTime - lastTime;
        if (dt > 50) dt = 16;
        lastTime = currentTime;
        
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        int btn_x = screen_width - (int)(40 * scale_x);
        int btn_y = screen_height - (int)(40 * scale_y);
        int btn_w = (int)(30 * scale_x);
        int btn_h = (int)(30 * scale_y);
        btn_instructions_hover = (mouseX >= btn_x && mouseX <= btn_x + btn_w && mouseY >= btn_y && mouseY <= btn_y + btn_h);
        
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) quit = 1;
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
                if (event.key.keysym.sym == SDLK_r) {
                    gameOver = 0; gagnant = 0; niveau_actuel = 1; transition_phase = 0; game_over_triggered = 0;
                    initPersonnage1(&joueur1, renderer, 100);
                    initPersonnage2(&joueur2, renderer, 300);
                    initBarresMobiles(barres);
                    nb_projectiles = 0;
                    currentBackground = bg1;
                    Mix_HaltChannel(-1);
                    Mix_HaltMusic();
                    if (g_bgMusic) Mix_PlayMusic(g_bgMusic, -1);
                    printf("Partie reinitialisee!\n");
                }
                if (event.key.keysym.sym == SDLK_F11) {
                    toggleFullscreen(window, renderer);
                    initPersonnage1(&joueur1, renderer, 100);
                    initPersonnage2(&joueur2, renderer, 300);
                    initBarresMobiles(barres);
                }
            }
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
                updateScale(renderer);
                initPersonnage1(&joueur1, renderer, 100);
                initPersonnage2(&joueur2, renderer, 300);
                initBarresMobiles(barres);
            }
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                if (btn_instructions_hover) show_instructions = !show_instructions;
            }
        }
        
        if (!gameOver) {
        
            if (niveau_actuel == 1) {
                int toutes_touchees = 1;
                for (int i = 0; i < MAX_BARRES; i++) {
                    if (!barres[i].touched_by_p1 || !barres[i].touched_by_p2) {
                        toutes_touchees = 0;
                        break;
                    }
                }
                if (toutes_touchees && !transition_phase) {
                    niveau_actuel = 2;
                    transition_phase = 1;
                    transition_time = SDL_GetTicks();
                    currentBackground = level1_img;
                    initBarresMobiles(barres);
                    printf("+++ TRANSITION VERS NIVEAU 1 +++\n");
                }
            }
            
     
            if (niveau_actuel == 2 && !transition_phase && !joueur1.dying && !joueur2.dying) {
                int j1_a_droite = (joueur1.rect.x + joueur1.rect.w >= screen_width - (int)(50 * scale_x));
                int j2_a_droite = (joueur2.rect.x + joueur2.rect.w >= screen_width - (int)(50 * scale_x));
                if ((j1_a_droite || j2_a_droite) && keys[SDL_SCANCODE_SPACE]) {
                   
                    cinematic_wait = 1;
                    while (cinematic_wait && !quit) {
                        SDL_PollEvent(&event);
                        if (event.type == SDL_QUIT) quit = 1;
                        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
                        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) cinematic_wait = 0;
                        SDL_RenderClear(renderer);
                        if (g_cinematic2) {
                            SDL_Rect dest = {0, 0, screen_width, screen_height};
                            SDL_RenderCopy(renderer, g_cinematic2, NULL, &dest);
                        }
                        SDL_RenderPresent(renderer);
                        SDL_Delay(16);
                    }
                    niveau_actuel = 3;
                    currentBackground = level2_img;
                    initBarresMobiles(barres);
                    joueur1.rect.x = (int)(100 * scale_x);
                    joueur2.rect.x = (int)(300 * scale_x);
                    printf("+++ TRANSITION VERS NIVEAU 2 +++\n");
                }
            }
            
            if (transition_phase && niveau_actuel != 3) {
                if (SDL_GetTicks() - transition_time > 1500) transition_phase = 0;
            }
            
            if ((joueur1.dying && joueur1.dying_animation_done) || (joueur2.dying && joueur2.dying_animation_done)) {
                if (joueur1.dying && joueur2.dying) { gameOver = 1; gagnant = 0; }
                else if (joueur1.dying) { gameOver = 1; gagnant = 2; Mix_HaltMusic(); }
                else if (joueur2.dying) { gameOver = 1; gagnant = 1; Mix_HaltMusic(); }
            }
            
            
            if (!joueur1.dying) {
                joueur1.velX = 0;
                if (keys[SDL_SCANCODE_A]) { joueur1.velX = -current_move_speed; joueur1.direction = -1; }
                if (keys[SDL_SCANCODE_D]) { joueur1.velX = current_move_speed; joueur1.direction = 1; }
                joueur1.crouching = keys[SDL_SCANCODE_W];
                
                int moving = (joueur1.velX != 0);
                if (moving && joueur1.onGround && !joueur1.jumping && !joueur1.attacking && !joueur1.crouching) {
                    if (!wasMoving1 && g_runSound) { runChannel1 = Mix_PlayChannel(-1, g_runSound, -1); wasMoving1 = 1; }
                } else {
                    if (wasMoving1 && runChannel1 != -1) { Mix_HaltChannel(runChannel1); runChannel1 = -1; wasMoving1 = 0; }
                }
                
                if (keys[SDL_SCANCODE_SPACE] && joueur1.onGround && !joueur1.attacking && !joueur1.crouching && niveau_actuel != 2) {
                    joueur1.velY = JUMP_POWER;
                    joueur1.onGround = 0;
                    joueur1.jumping = 1;
                    if (g_jumpSound) Mix_PlayChannel(-1, g_jumpSound, 0);
                }
                if (keys[SDL_SCANCODE_F] && !joueur1.attacking && !joueur1.crouching && (SDL_GetTicks() - joueur1.last_attack_used > ATTACK_DELAY)) {
                    joueur1.attacking = 1;
                    joueur1.current_attack_frame = 0;
                    joueur1.last_attack_used = SDL_GetTicks();
                    if (g_attackSound1) Mix_PlayChannel(-1, g_attackSound1, 0);
                    creerProjectile(projectiles, &nb_projectiles, joueur1.rect.x, joueur1.rect.y, joueur1.direction, 1, 0);
                }
            }
            
        
            if (!joueur2.dying) {
                joueur2.velX = 0;
                if (keys[SDL_SCANCODE_LEFT]) { joueur2.velX = -current_move_speed; joueur2.direction = -1; }
                if (keys[SDL_SCANCODE_RIGHT]) { joueur2.velX = current_move_speed; joueur2.direction = 1; }
                joueur2.crouching = keys[SDL_SCANCODE_DOWN];
                
                int moving = (joueur2.velX != 0);
                if (moving && joueur2.onGround && !joueur2.jumping && !joueur2.attacking && !joueur2.crouching) {
                    if (!wasMoving2 && g_runSound) { runChannel2 = Mix_PlayChannel(-1, g_runSound, -1); wasMoving2 = 1; }
                } else {
                    if (wasMoving2 && runChannel2 != -1) { Mix_HaltChannel(runChannel2); runChannel2 = -1; wasMoving2 = 0; }
                }
                
                if (keys[SDL_SCANCODE_UP] && joueur2.onGround && !joueur2.attacking && !joueur2.crouching && niveau_actuel != 2) {
                    joueur2.velY = JUMP_POWER;
                    joueur2.onGround = 0;
                    joueur2.jumping = 1;
                    if (g_jumpSound) Mix_PlayChannel(-1, g_jumpSound, 0);
                }
                if (keys[SDL_SCANCODE_K] && !joueur2.attacking && !joueur2.crouching && (SDL_GetTicks() - joueur2.last_attack_used > ATTACK_DELAY)) {
                    joueur2.attacking = 1;
                    joueur2.current_attack_frame = 0;
                    joueur2.last_attack_used = SDL_GetTicks();
                    if (g_attackSound2) Mix_PlayChannel(-1, g_attackSound2, 0);
                    creerProjectile(projectiles, &nb_projectiles, joueur2.rect.x, joueur2.rect.y, joueur2.direction, 2, 1);
                }
            }
            
            updateBarresMobiles(barres);
            updatePersonnageAvecBarres(&joueur1, dt, barres);
            updatePersonnageAvecBarres(&joueur2, dt, barres);
            updateProjectiles(projectiles, &nb_projectiles, &joueur1, &joueur2);
        }
        
        SDL_RenderClear(renderer);
        
        if (currentBackground) {
            SDL_Rect bg_rect = {0, 0, screen_width, screen_height};
            SDL_RenderCopy(renderer, currentBackground, NULL, &bg_rect);
        } else {
            SDL_SetRenderDrawColor(renderer, 50, 100, 150, 255);
            SDL_RenderClear(renderer);
        }
        
        renderGround(renderer);
        if (!gameOver && (niveau_actuel == 1 || niveau_actuel == 2 || niveau_actuel == 3)) {
            renderBarresMobiles(renderer, barres);
        }
        renderPersonnage(renderer, &joueur1);
        renderPersonnage(renderer, &joueur2);
        renderProjectiles(renderer, projectiles, nb_projectiles);
        
        renderScores(renderer, font, &joueur1, &joueur2);
        renderVies(renderer, &joueur1, &joueur2);
        renderInstructionsButton(renderer, font);
        if (show_instructions && g_instructions_texture) renderInstructionsBox(renderer);
        
        if (font && !gameOver) {
            int font_size = (int)(16 * scale_x);
            TTF_Font *big_font = TTF_OpenFont("assets/arial.ttf", font_size);
            if (!big_font) big_font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", font_size);
            if (big_font) {
                char levelTitle[100];
                if (niveau_actuel == 1) sprintf(levelTitle, "NIVEAU 0: Touchez les 3 barres");
                else if (niveau_actuel == 2) sprintf(levelTitle, "NIVEAU 1: Allez a droite + ESPACE");
                else sprintf(levelTitle, "NIVEAU 2: COMBAT FINAL!");
                SDL_Surface *titleSurf = TTF_RenderText_Blended(big_font, levelTitle, (SDL_Color){255, 255, 0, 255});
                if (titleSurf) {
                    SDL_Texture *titleTex = SDL_CreateTextureFromSurface(renderer, titleSurf);
                    SDL_Rect titleRect = {screen_width/2 - titleSurf->w/2, (int)(10 * scale_y), titleSurf->w, titleSurf->h};
                    SDL_RenderCopy(renderer, titleTex, NULL, &titleRect);
                    SDL_FreeSurface(titleSurf);
                    SDL_DestroyTexture(titleTex);
                }
                
                char vieText[100];
                sprintf(vieText, "Vies: J1=%d  J2=%d", joueur1.vie/20, joueur2.vie/20);
                SDL_Surface *surf = TTF_RenderText_Blended(big_font, vieText, (SDL_Color){255, 255, 255, 255});
                if (surf) {
                    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
                    SDL_Rect r = {screen_width/2 - surf->w/2, (int)(80 * scale_y), surf->w, surf->h};
                    SDL_RenderCopy(renderer, tex, NULL, &r);
                    SDL_FreeSurface(surf);
                    SDL_DestroyTexture(tex);
                }
                
                int p1_progress = 0, p2_progress = 0;
                for (int i = 0; i < MAX_BARRES; i++) {
                    if (barres[i].touched_by_p1) p1_progress++;
                    if (barres[i].touched_by_p2) p2_progress++;
                }
                char progressText[100];
                sprintf(progressText, "Barres: J1 [%d/3]  J2 [%d/3]", p1_progress, p2_progress);
                SDL_Surface *surf2 = TTF_RenderText_Blended(big_font, progressText, (SDL_Color){255, 255, 255, 255});
                if (surf2) {
                    SDL_Texture *tex2 = SDL_CreateTextureFromSurface(renderer, surf2);
                    SDL_Rect r2 = {screen_width/2 - surf2->w/2, (int)(105 * scale_y), surf2->w, surf2->h};
                    SDL_RenderCopy(renderer, tex2, NULL, &r2);
                    SDL_FreeSurface(surf2);
                    SDL_DestroyTexture(tex2);
                }
                TTF_CloseFont(big_font);
            }
        }
        
        if (transition_phase && !gameOver && niveau_actuel != 3) {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            Uint32 elapsed = SDL_GetTicks() - transition_time;
            int alpha = (elapsed < 750) ? (elapsed * 255 / 750) : (255 - (elapsed - 750) * 255 / 750);
            if (alpha < 0) alpha = 0;
            if (alpha > 255) alpha = 255;
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);
            SDL_Rect overlay = {0, 0, screen_width, screen_height};
            SDL_RenderFillRect(renderer, &overlay);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        }
        
        if (gameOver) renderGameOver(renderer, gagnant);
        
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    if (bg1) SDL_DestroyTexture(bg1);
    if (level1_img) SDL_DestroyTexture(level1_img);
    if (level2_img) SDL_DestroyTexture(level2_img);
    if (g_cinematic1) SDL_DestroyTexture(g_cinematic1);
    if (g_cinematic2) SDL_DestroyTexture(g_cinematic2);
    if (g_bomb_texture) SDL_DestroyTexture(g_bomb_texture);
    if (g_ball_texture) SDL_DestroyTexture(g_ball_texture);
    if (g_instructions_texture) SDL_DestroyTexture(g_instructions_texture);
    if (g_win_p1_texture) SDL_DestroyTexture(g_win_p1_texture);
    if (g_win_p2_texture) SDL_DestroyTexture(g_win_p2_texture);
    if (g_vies_texture) SDL_DestroyTexture(g_vies_texture);
    if (font) TTF_CloseFont(font);
    if (g_bgMusic) Mix_FreeMusic(g_bgMusic);
    if (g_attackSound1) Mix_FreeChunk(g_attackSound1);
    if (g_attackSound2) Mix_FreeChunk(g_attackSound2);
    if (g_victorySound) Mix_FreeChunk(g_victorySound);
    if (g_jumpSound) Mix_FreeChunk(g_jumpSound);
    if (g_runSound) Mix_FreeChunk(g_runSound);
}
