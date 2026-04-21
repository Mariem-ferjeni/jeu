#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#define PLATFORM_FIXE 0
#define PLATFORM_MOBILE 1
#define PLATFORM_DESTRUCTIBLE 2

#define MODE_MONO 0
#define MODE_MULTI 1

typedef struct {
    SDL_Rect rect;
    int type;
    int active;
    int speed;
    int minX;
    int maxX;
    Uint32 lastDamageTime;
} Platform;

typedef struct {
    SDL_Texture *image;
    SDL_Rect camera1;
    SDL_Rect camera2;
    int levelWidth;
    int levelHeight;
    Platform platforms[50];
    int nbPlatforms;
    int currentLevel;
    Uint32 startTime;
    Uint32 pauseTime;
    int paused;
    TTF_Font *font;
    SDL_Rect guideButton;
    int guideVisible;
    SDL_Rect guideWindow;
    char guideText[800];
    int displayMode;
} Background;

void initBackground(Background *b, SDL_Renderer *r, int level, int w, int h);
void drawBackground(Background b, SDL_Renderer *r);
void updateCamera(Background *b, SDL_Rect player, int num);
void updatePlatforms(Background *b);
void drawPlatforms(Background b, SDL_Renderer *r, SDL_Rect cam);
void checkCollision(Background *b, SDL_Rect *p, int *onGround, int *vy);

#endif
