#include "background.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void initBackground(Background *b, SDL_Renderer *r, int level, int w, int h)
{
    char path[100];
    if(level == 1) sprintf(path, "images/bg1.png");
    else sprintf(path, "images/bg2.png");
    
    SDL_Surface *s = IMG_Load(path);
    if(s == NULL) {
        s = IMG_Load("images/bg.png");
        if(s == NULL) {
            s = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
            if(level == 1) SDL_FillRect(s, NULL, SDL_MapRGB(s->format, 100, 150, 200));
            else SDL_FillRect(s, NULL, SDL_MapRGB(s->format, 80, 120, 80));
        }
    }
    
    b->image = SDL_CreateTextureFromSurface(r, s);
    SDL_FreeSurface(s);
    
    b->levelWidth = w;
    b->levelHeight = h;
    b->currentLevel = level;
    b->displayMode = MODE_MONO;
    b->camera1 = (SDL_Rect){0, 0, 640, 720};
    b->camera2 = (SDL_Rect){0, 0, 640, 720};
    
    srand(time(NULL));
    
    if(level == 1) {
        b->nbPlatforms = 15;
        b->platforms[0] = (Platform){{0, 680, 3000, 40}, PLATFORM_FIXE, 1, 0, 0, 0, 0};
        b->platforms[1] = (Platform){{200, 550, 120, 20}, PLATFORM_FIXE, 1, 0, 0, 0, 0};
        b->platforms[2] = (Platform){{500, 480, 120, 20}, PLATFORM_FIXE, 1, 0, 0, 0, 0};
        b->platforms[3] = (Platform){{800, 400, 120, 20}, PLATFORM_FIXE, 1, 0, 0, 0, 0};
        b->platforms[4] = (Platform){{1100, 350, 120, 20}, PLATFORM_FIXE, 1, 0, 0, 0, 0};
        b->platforms[5] = (Platform){{1400, 300, 120, 20}, PLATFORM_FIXE, 1, 0, 0, 0, 0};
        b->platforms[6] = (Platform){{1700, 350, 120, 20}, PLATFORM_FIXE, 1, 0, 0, 0, 0};
        b->platforms[7] = (Platform){{2000, 400, 120, 20}, PLATFORM_FIXE, 1, 0, 0, 0, 0};
        b->platforms[8] = (Platform){{2300, 450, 120, 20}, PLATFORM_FIXE, 1, 0, 0, 0, 0};
        b->platforms[9] = (Platform){{2600, 500, 120, 20}, PLATFORM_FIXE, 1, 0, 0, 0, 0};
        b->platforms[10] = (Platform){{350, 350, 100, 20}, PLATFORM_MOBILE, 1, 5, 200, 600, 0};
        b->platforms[11] = (Platform){{1200, 250, 100, 20}, PLATFORM_MOBILE, 1, 6, 1000, 1500, 0};
        b->platforms[12] = (Platform){{2100, 300, 100, 20}, PLATFORM_MOBILE, 1, 5, 1900, 2400, 0};
        b->platforms[13] = (Platform){{650, 600, 80, 20}, PLATFORM_DESTRUCTIBLE, 1, 0, 0, 0, 0};
        b->platforms[14] = (Platform){{1550, 500, 80, 20}, PLATFORM_DESTRUCTIBLE, 1, 0, 0, 0, 0};
    }
    else {
        b->nbPlatforms = 18;
        b->platforms[0] = (Platform){{0, 680, 3000, 40}, PLATFORM_FIXE, 1, 0, 0, 0, 0};
        b->platforms[1] = (Platform){{150, 580, 100, 20}, PLATFORM_FIXE, 1, 0, 0, 0, 0};
        b->platforms[2] = (Platform){{350, 500, 100, 20}, PLATFORM_FIXE, 1, 0, 0, 0, 0};
        b->platforms[3] = (Platform){{550, 420, 100, 20}, PLATFORM_FIXE, 1, 0, 0, 0, 0};
        b->platforms[4] = (Platform){{750, 340, 100, 20}, PLATFORM_FIXE, 1, 0, 0, 0, 0};
        b->platforms[5] = (Platform){{950, 260, 100, 20}, PLATFORM_FIXE, 1, 0, 0, 0, 0};
        b->platforms[6] = (Platform){{1150, 180, 100, 20}, PLATFORM_FIXE, 1, 0, 0, 0, 0};
        b->platforms[7] = (Platform){{1400, 250, 100, 20}, PLATFORM_FIXE, 1, 0, 0, 0, 0};
        b->platforms[8] = (Platform){{1650, 330, 100, 20}, PLATFORM_FIXE, 1, 0, 0, 0, 0};
        b->platforms[9] = (Platform){{1900, 410, 100, 20}, PLATFORM_FIXE, 1, 0, 0, 0, 0};
        b->platforms[10] = (Platform){{2150, 490, 100, 20}, PLATFORM_FIXE, 1, 0, 0, 0, 0};
        b->platforms[11] = (Platform){{2400, 570, 100, 20}, PLATFORM_FIXE, 1, 0, 0, 0, 0};
        b->platforms[12] = (Platform){{400, 300, 80, 20}, PLATFORM_MOBILE, 1, 7, 250, 650, 0};
        b->platforms[13] = (Platform){{1200, 150, 80, 20}, PLATFORM_MOBILE, 1, 8, 1050, 1450, 0};
        b->platforms[14] = (Platform){{2000, 200, 80, 20}, PLATFORM_MOBILE, 1, 7, 1850, 2250, 0};
        b->platforms[15] = (Platform){{800, 600, 80, 20}, PLATFORM_DESTRUCTIBLE, 1, 0, 0, 0, 0};
        b->platforms[16] = (Platform){{1600, 500, 80, 20}, PLATFORM_DESTRUCTIBLE, 1, 0, 0, 0, 0};
        b->platforms[17] = (Platform){{2300, 400, 80, 20}, PLATFORM_DESTRUCTIBLE, 1, 0, 0, 0, 0};
    }
    
    b->startTime = SDL_GetTicks();
    b->pauseTime = 0;
    b->paused = 0;
    b->font = TTF_OpenFont("font/arial.ttf", 28);
    if(!b->font) b->font = TTF_OpenFont("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf", 28);
    
    b->guideButton = (SDL_Rect){1240, 10, 30, 30};
    b->guideVisible = 0;
    b->guideWindow = (SDL_Rect){340, 200, 600, 320};
    sprintf(b->guideText, "JOUEUR 1:\n  Fleches\nJOUEUR 2:\n  Q/D/W\nM:Mode\nP:Pause\nN:Niveau\nEchap:Quitter");
}

void drawBackground(Background b, SDL_Renderer *r)
{
    SDL_RenderCopy(r, b.image, &b.camera1, NULL);
}

void updateCamera(Background *b, SDL_Rect player, int num)
{
    if(num == 1) {
        b->camera1.x = player.x + 20 - b->camera1.w/2;
        if(b->camera1.x < 0) b->camera1.x = 0;
        if(b->camera1.x > b->levelWidth - b->camera1.w)
            b->camera1.x = b->levelWidth - b->camera1.w;
    }
    else {
        b->camera2.x = player.x + 20 - b->camera2.w/2;
        if(b->camera2.x < 0) b->camera2.x = 0;
        if(b->camera2.x > b->levelWidth - b->camera2.w)
            b->camera2.x = b->levelWidth - b->camera2.w;
    }
}

void updatePlatforms(Background *b)
{
    int i;
    for(i = 0; i < b->nbPlatforms; i++) {
        if(!b->platforms[i].active) continue;
        if(b->platforms[i].type == PLATFORM_MOBILE) {
            b->platforms[i].rect.x += b->platforms[i].speed;
            if(b->platforms[i].rect.x >= b->platforms[i].maxX || 
               b->platforms[i].rect.x <= b->platforms[i].minX)
                b->platforms[i].speed = -b->platforms[i].speed;
        }
    }
}

void drawPlatforms(Background b, SDL_Renderer *r, SDL_Rect cam)
{
    int i;
    for(i = 0; i < b.nbPlatforms; i++) {
        if(!b.platforms[i].active) continue;
        SDL_Rect sr;
        sr.x = b.platforms[i].rect.x - cam.x;
        sr.y = b.platforms[i].rect.y;
        sr.w = b.platforms[i].rect.w;
        sr.h = b.platforms[i].rect.h;
        if(sr.x + sr.w > 0 && sr.x < 640) {
            if(b.platforms[i].type == PLATFORM_FIXE)
                SDL_SetRenderDrawColor(r, 120, 120, 120, 255);
            else if(b.platforms[i].type == PLATFORM_MOBILE)
                SDL_SetRenderDrawColor(r, 0, 150, 250, 255);
            else
                SDL_SetRenderDrawColor(r, 255, 120, 0, 255);
            SDL_RenderFillRect(r, &sr);
            SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
            SDL_RenderDrawRect(r, &sr);
        }
    }
}

void checkCollision(Background *b, SDL_Rect *p, int *onGround, int *vy)
{
    int i;
    *onGround = 0;
    for(i = 0; i < b->nbPlatforms; i++) {
        if(!b->platforms[i].active) continue;
        SDL_Rect plat = b->platforms[i].rect;
        if(p->x + p->w > plat.x && p->x < plat.x + plat.w &&
           p->y + p->h >= plat.y && p->y + p->h <= plat.y + 30 && *vy >= 0) {
            *onGround = 1;
            p->y = plat.y - p->h;
            *vy = 0;
            if(b->platforms[i].type == PLATFORM_DESTRUCTIBLE) {
                Uint32 now = SDL_GetTicks();
                if(now - b->platforms[i].lastDamageTime > 500) {
                    b->platforms[i].lastDamageTime = now;
                    b->platforms[i].active = 0;
                }
            }
            break;
        }
    }
}
