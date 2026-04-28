#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#define MODE_MONO 0
#define MODE_MULTI 1
#define MAX_GEMS 50

typedef struct {
    SDL_Rect rect;
    int active;
    int value;
    int color;
} Gem;

typedef struct {
    SDL_Rect rect;
    int active;
} Portal;

typedef struct {
    SDL_Texture *image;
    int imageWidth;
    int imageHeight;
    SDL_Rect camera1;
    SDL_Rect camera2;
    int levelWidth;
    int levelHeight;
    int currentLevel;
    unsigned int startTime;
    unsigned int pauseTime;
    int paused;
    TTF_Font *font;
    SDL_Rect guideButton;
    int guideVisible;
    SDL_Rect guideWindow;
    char guideText[800];
    int displayMode;
    int score1;
    int score2;
    Gem gems[MAX_GEMS];
    int nbGems;
    Portal portal;
    int levelComplete;
    unsigned int levelCompleteTime;
} Background;

void initBackground(Background *b, SDL_Renderer *r, int level, int w, int h);
void drawBackground(Background b, SDL_Renderer *r, SDL_Rect cam);
void drawGems(Background b, SDL_Renderer *r, SDL_Rect cam);
void drawPortal(Background b, SDL_Renderer *r, SDL_Rect cam);
void updateCamera(Background *b, SDL_Rect player, int num);
void checkGemCollection(Background *b, SDL_Rect *player, int playerNum);
int checkPortalCollision(Background *b, SDL_Rect *player);

#endif
