#ifndef LEVEL_MANAGER_H
#define LEVEL_MANAGER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#define MAX_LEVELS 3
#define SCREEN_WIDTH 750
#define SCREEN_HEIGHT 550
#define GRAVITY 0.8
#define JUMP_POWER -12
#define ANIMATION_DELAY 100
#define MOVE_SPEED 5.0
#define ATTACK_DELAY 500
#define BOMB_SPEED 8
#define MAX_BARRES 3
#define REF_SOL_Y 450
#define REF_BARRE_HAUTEUR 20
#define REF_BARRE_LARGEUR 120
#define REF_PERSO_LARGEUR 80
#define REF_PERSO_HAUTEUR 80

typedef struct {
    SDL_Rect rect;
    SDL_Texture *walk_frames[8];
    SDL_Texture *jump_frames[5];
    SDL_Texture *attack_frames[5];
    SDL_Texture *still_texture;
    SDL_Texture *crouch_texture;
    SDL_Texture *fall_frames[6];
    int current_frame;
    int current_jump_frame;
    int current_attack_frame;
    int current_fall_frame;
    float velX, velY;
    int onGround;
    int direction;
    int jumping;
    int attacking;
    int crouching;
    int dying;
    int dying_animation_done;
    Uint32 last_anim_time;
    Uint32 last_jump_anim_time;
    Uint32 last_attack_time;
    Uint32 last_attack_used;
    Uint32 last_fall_time;
    int vie;
    int vie_max;
    int id;
    int frame_count;
    int jump_frame_count;
    int attack_frame_count;
    int fall_frame_count;
    int score;
} Personnage;

typedef struct {
    SDL_Rect rect;
    int active;
    int direction;
    int owner_id;
    int type;
} Projectile;

typedef struct {
    SDL_Rect rect;
    int vitesseX;
    int direction;
    int actif;
    int touched_by_p1;
    int touched_by_p2;
} BarreMobile;

typedef struct {
    int level_number;
    char background_path[100];
    int door_x;
    int door_y;
    int door_w;
    int door_h;
    int door_unlocked;
    int target_x_position;
} Level;

typedef struct {
    Level levels[MAX_LEVELS];
    int current_level;
    int level_transition;
    Uint32 transition_start_time;
    SDL_Texture *door_texture;
    int door_open;
    int game_completed;
} LevelManager;

void initLevelManager(LevelManager *lm, SDL_Renderer *renderer);
void loadLevel(LevelManager *lm, int level_num);
void updateLevel(LevelManager *lm, SDL_Renderer *renderer, int player_x, int player_y, int player_w, int player_h);
void renderDoor(LevelManager *lm, SDL_Renderer *renderer);
const char* getCurrentBackgroundPath(LevelManager *lm);
void cleanupLevelManager(LevelManager *lm);
void resetLevelManager(LevelManager *lm);

SDL_Texture* loadTexture(SDL_Renderer *renderer, const char *path);
void updateScale(SDL_Renderer *renderer);
void toggleFullscreen(SDL_Window *window, SDL_Renderer *renderer);
void initPersonnage1(Personnage *p, SDL_Renderer *renderer, int startX);
void initPersonnage2(Personnage *p, SDL_Renderer *renderer, int startX);
void initBarresMobiles(BarreMobile barres[]);
void updateBarresMobiles(BarreMobile barres[]);
void renderBarresMobiles(SDL_Renderer *renderer, BarreMobile barres[]);
void updatePersonnageAvecBarres(Personnage *p, Uint32 dt, BarreMobile barres[]);
void renderPersonnage(SDL_Renderer *renderer, Personnage *p);
void creerProjectile(Projectile projectiles[], int *nb_projectiles, int x, int y, int direction, int owner_id, int type);
void updateProjectiles(Projectile projectiles[], int *nb_projectiles, Personnage *p1, Personnage *p2);
void renderProjectiles(SDL_Renderer *renderer, Projectile projectiles[], int nb);
void renderGround(SDL_Renderer *renderer);
void renderInstructionsButton(SDL_Renderer *renderer, TTF_Font *font);
void renderInstructionsBox(SDL_Renderer *renderer);
void renderScores(SDL_Renderer *renderer, TTF_Font *font, Personnage *p1, Personnage *p2);
void renderVies(SDL_Renderer *renderer, Personnage *p1, Personnage *p2);
void renderGameOver(SDL_Renderer *renderer, int gagnant);
void jeuPrincipal(SDL_Window *window, SDL_Renderer *renderer);

#endif
