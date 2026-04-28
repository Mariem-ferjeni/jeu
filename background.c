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
    b->imageWidth = s->w;
    b->imageHeight = s->h;
    SDL_FreeSurface(s);
    
    b->levelWidth = b->imageWidth;
    b->levelHeight = b->imageHeight;
    
    b->currentLevel = level;
    b->displayMode = MODE_MONO;
    b->camera1 = (SDL_Rect){0, 0, 1280, 720};
    b->camera2 = (SDL_Rect){0, 0, 640, 720};
    
    b->startTime = SDL_GetTicks();
    b->pauseTime = 0;
    b->paused = 0;
    b->levelComplete = 0;
    b->levelCompleteTime = 0;
    
    b->font = TTF_OpenFont("font/arial.ttf", 28);
    if(!b->font) b->font = TTF_OpenFont("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf", 28);
    
    b->guideButton = (SDL_Rect){1240, 10, 30, 30};
    b->guideVisible = 0;
    b->guideWindow = (SDL_Rect){340, 200, 600, 320};
    
    if(level == 1) {
        sprintf(b->guideText, "JOUEUR 1 (VERT):\n  Z  S  Q  D\nJOUEUR 2 (JAUNE):\n  Fleches \nM:Changer mode\nP:Pause\nN:Niveau\nEchap:Quitter ");
    } else {
        sprintf(b->guideText, "JOUEUR 1 (VERT):\n  Z  S  Q  D\nJOUEUR 2 (JAUNE):\n  Fleches \nM:Changer mode\nP:Pause\nN:Niveau\nEchap:Quitter ");
    }
    
    b->score1 = 0;
    b->score2 = 0;
    
    srand(time(NULL));
    b->nbGems = 40;
    
    for(int i = 0; i < b->nbGems; i++) {
        b->gems[i].rect.x = 50 + rand() % (b->levelWidth - 100);
        b->gems[i].rect.y = 50 + rand() % (b->levelHeight - 100);
        b->gems[i].rect.w = 15;
        b->gems[i].rect.h = 15;
        b->gems[i].active = 1;
        
        int val = rand() % 3;
        if(val == 0) {
            b->gems[i].value = 10;
            b->gems[i].color = 0;
        } else if(val == 1) {
            b->gems[i].value = 20;
            b->gems[i].color = 1;
        } else {
            b->gems[i].value = 50;
            b->gems[i].color = 2;
        }
    }
    
    if(level == 1) {
        b->portal.rect.x = b->levelWidth - 100;
        b->portal.rect.y = b->levelHeight - 100;
        b->portal.rect.w = 60;
        b->portal.rect.h = 80;
        b->portal.active = 1;
    } else {
        b->portal.active = 0;
    }
}

void drawBackground(Background b, SDL_Renderer *r, SDL_Rect cam)
{
    SDL_Rect dest = {0, 0, cam.w, cam.h};
    SDL_RenderCopy(r, b.image, &cam, &dest);
}

void drawGems(Background b, SDL_Renderer *r, SDL_Rect cam)
{
    for(int i = 0; i < b.nbGems; i++) {
        if(!b.gems[i].active) continue;
        
        if(b.gems[i].rect.x + b.gems[i].rect.w > cam.x && 
           b.gems[i].rect.x < cam.x + cam.w &&
           b.gems[i].rect.y + b.gems[i].rect.h > cam.y && 
           b.gems[i].rect.y < cam.y + cam.h) {
            
            SDL_Rect gemRect = {
                b.gems[i].rect.x - cam.x,
                b.gems[i].rect.y - cam.y,
                b.gems[i].rect.w,
                b.gems[i].rect.h
            };
            
            switch(b.gems[i].color) {
                case 0: SDL_SetRenderDrawColor(r, 255, 0, 0, 255); break;
                case 1: SDL_SetRenderDrawColor(r, 0, 0, 255, 255); break;
                case 2: SDL_SetRenderDrawColor(r, 255, 215, 0, 255); break;
                default: SDL_SetRenderDrawColor(r, 255, 255, 255, 255); break;
            }
            
            SDL_RenderFillRect(r, &gemRect);
            SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
            SDL_RenderDrawRect(r, &gemRect);
        }
    }
}

void drawPortal(Background b, SDL_Renderer *r, SDL_Rect cam)
{
    if(!b.portal.active) return;
    
    if(b.portal.rect.x + b.portal.rect.w > cam.x && 
       b.portal.rect.x < cam.x + cam.w &&
       b.portal.rect.y + b.portal.rect.h > cam.y && 
       b.portal.rect.y < cam.y + cam.h) {
        
        SDL_Rect portalRect = {
            b.portal.rect.x - cam.x,
            b.portal.rect.y - cam.y,
            b.portal.rect.w,
            b.portal.rect.h
        };
        
        unsigned int time = SDL_GetTicks();
        int color = (time / 200) % 2;
        
        if(color == 0) {
            SDL_SetRenderDrawColor(r, 128, 0, 128, 255);
        } else {
            SDL_SetRenderDrawColor(r, 255, 0, 255, 255);
        }
        
        SDL_RenderFillRect(r, &portalRect);
        SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
        SDL_RenderDrawRect(r, &portalRect);
        
        SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
        SDL_RenderDrawLine(r, portalRect.x + 15, portalRect.y + portalRect.h/2,
                              portalRect.x + 35, portalRect.y + portalRect.h/2);
        SDL_RenderDrawLine(r, portalRect.x + 25, portalRect.y + portalRect.h/2 - 10,
                              portalRect.x + 35, portalRect.y + portalRect.h/2);
        SDL_RenderDrawLine(r, portalRect.x + 25, portalRect.y + portalRect.h/2 + 10,
                              portalRect.x + 35, portalRect.y + portalRect.h/2);
    }
}

void updateCamera(Background *b, SDL_Rect player, int num)
{
    if(num == 1) {
        b->camera1.x = player.x + 20 - b->camera1.w / 2;
        b->camera1.y = player.y + 20 - b->camera1.h / 2;
        
        if(b->camera1.x < 0) b->camera1.x = 0;
        if(b->camera1.x > b->imageWidth - b->camera1.w) 
            b->camera1.x = b->imageWidth - b->camera1.w;
        if(b->camera1.y < 0) b->camera1.y = 0;
        if(b->camera1.y > b->imageHeight - b->camera1.h) 
            b->camera1.y = b->imageHeight - b->camera1.h;
    }
    else {
        b->camera2.x = player.x + 20 - b->camera2.w / 2;
        b->camera2.y = player.y + 20 - b->camera2.h / 2;
        
        if(b->camera2.x < 0) b->camera2.x = 0;
        if(b->camera2.x > b->imageWidth - b->camera2.w) 
            b->camera2.x = b->imageWidth - b->camera2.w;
        if(b->camera2.y < 0) b->camera2.y = 0;
        if(b->camera2.y > b->imageHeight - b->camera2.h) 
            b->camera2.y = b->imageHeight - b->camera2.h;
    }
}

void checkGemCollection(Background *b, SDL_Rect *player, int playerNum)
{
    for(int i = 0; i < b->nbGems; i++) {
        if(!b->gems[i].active) continue;
        
        if(player->x < b->gems[i].rect.x + b->gems[i].rect.w &&
           player->x + player->w > b->gems[i].rect.x &&
           player->y < b->gems[i].rect.y + b->gems[i].rect.h &&
           player->y + player->h > b->gems[i].rect.y) {
            
            b->gems[i].active = 0;
            
            if(playerNum == 1) {
                b->score1 += b->gems[i].value;
            } else {
                b->score2 += b->gems[i].value;
            }
        }
    }
}

int checkPortalCollision(Background *b, SDL_Rect *player)
{
    if(!b->portal.active) return 0;
    
    if(player->x < b->portal.rect.x + b->portal.rect.w &&
       player->x + player->w > b->portal.rect.x &&
       player->y < b->portal.rect.y + b->portal.rect.h &&
       player->y + player->h > b->portal.rect.y) {
        
        b->portal.active = 0;
        return 1;
    }
    return 0;
}
