#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include "background.h"

void drawTimer(Background b, SDL_Renderer *r)
{
    unsigned int current;
    if(b.paused) current = b.pauseTime;
    else current = SDL_GetTicks() - b.startTime;
    unsigned int sec = current / 1000;
    int min = sec / 60;
    int s = sec % 60;
    char txt[30];
    sprintf(txt, "Temps: %02d:%02d", min, s);
    if(b.font) {
        SDL_Surface *sf = TTF_RenderText_Solid(b.font, txt, (SDL_Color){255,255,255});
        if(sf) {
            SDL_Texture *tex = SDL_CreateTextureFromSurface(r, sf);
            SDL_Rect pos = {20, 20, sf->w, sf->h};
            SDL_RenderCopy(r, tex, NULL, &pos);
            SDL_FreeSurface(sf);
            SDL_DestroyTexture(tex);
        }
    }
}

void drawScores(Background b, SDL_Renderer *r)
{
    if(!b.font) return;
    
    char scoreTxt[100];
    SDL_Color color1 = {0, 255, 0};
    SDL_Color color2 = {255, 255, 0};
    
    if(b.displayMode == MODE_MONO) {
        sprintf(scoreTxt, "Score: %d", b.score1);
        SDL_Surface *sf = TTF_RenderText_Solid(b.font, scoreTxt, color1);
        if(sf) {
            SDL_Texture *tex = SDL_CreateTextureFromSurface(r, sf);
            SDL_Rect pos = {20, 100, sf->w, sf->h};
            SDL_RenderCopy(r, tex, NULL, &pos);
            SDL_FreeSurface(sf);
            SDL_DestroyTexture(tex);
        }
    } else {
        sprintf(scoreTxt, "J1: %d", b.score1);
        SDL_Surface *sf1 = TTF_RenderText_Solid(b.font, scoreTxt, color1);
        if(sf1) {
            SDL_Texture *tex1 = SDL_CreateTextureFromSurface(r, sf1);
            SDL_Rect pos1 = {20, 100, sf1->w, sf1->h};
            SDL_RenderCopy(r, tex1, NULL, &pos1);
            SDL_FreeSurface(sf1);
            SDL_DestroyTexture(tex1);
        }
        
        sprintf(scoreTxt, "J2: %d", b.score2);
        SDL_Surface *sf2 = TTF_RenderText_Solid(b.font, scoreTxt, color2);
        if(sf2) {
            SDL_Texture *tex2 = SDL_CreateTextureFromSurface(r, sf2);
            SDL_Rect pos2 = {120, 100, sf2->w, sf2->h};
            SDL_RenderCopy(r, tex2, NULL, &pos2);
            SDL_FreeSurface(sf2);
            SDL_DestroyTexture(tex2);
        }
    }
}

void drawGuide(Background b, SDL_Renderer *r)
{
    SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
    SDL_RenderFillRect(r, &b.guideButton);
    SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
    SDL_RenderDrawRect(r, &b.guideButton);
    if(b.font) {
        SDL_Surface *sf = TTF_RenderText_Solid(b.font, "?", (SDL_Color){255,255,255});
        if(sf) {
            SDL_Texture *tex = SDL_CreateTextureFromSurface(r, sf);
            SDL_Rect pos = {b.guideButton.x + 7, b.guideButton.y + 5, 16, 20};
            SDL_RenderCopy(r, tex, NULL, &pos);
            SDL_FreeSurface(sf);
            SDL_DestroyTexture(tex);
        }
    }
    if(b.guideVisible) {
        SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(r, 0, 0, 0, 200);
        SDL_Rect full = {0, 0, 1280, 720};
        SDL_RenderFillRect(r, &full);
        SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
        SDL_RenderFillRect(r, &b.guideWindow);
        SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
        SDL_RenderDrawRect(r, &b.guideWindow);
        if(b.font) {
            SDL_Surface *sf = TTF_RenderText_Blended_Wrapped(b.font, b.guideText, (SDL_Color){255,255,255}, 560);
            if(sf) {
                SDL_Texture *tex = SDL_CreateTextureFromSurface(r, sf);
                SDL_Rect pos = {b.guideWindow.x + 20, b.guideWindow.y + 20, sf->w, sf->h};
                SDL_RenderCopy(r, tex, NULL, &pos);
                SDL_FreeSurface(sf);
                SDL_DestroyTexture(tex);
            }
        }
    }
}

int handleGuideClick(Background *b, int mx, int my)
{
    if(mx >= b->guideButton.x && mx <= b->guideButton.x + 30 && 
       my >= b->guideButton.y && my <= b->guideButton.y + 30) {
        b->guideVisible = !b->guideVisible;
        return 1;
    }
    return 0;
}

int main(int argc, char* argv[])
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0) return 1;
    if(TTF_Init() < 0) return 1;
    
    SDL_Window *win = SDL_CreateWindow("Jeu - Collection de Gemmes", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, 0);
    if(!win) return 1;
    
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if(!ren) return 1;
    
    int curLevel = 1;
    Background game;
    
    char path[100];
    sprintf(path, "images/bg%d.png", curLevel);
    SDL_Surface *tempSurface = IMG_Load(path);
    int levelW = 3000, levelH = 3000;
    if(tempSurface) {
        levelW = tempSurface->w;
        levelH = tempSurface->h;
        SDL_FreeSurface(tempSurface);
    }
    
    initBackground(&game, ren, curLevel, levelW, levelH);
    
    SDL_Rect p1 = {levelW/2 - 20, levelH/2 - 20, 40, 40};
    SDL_Rect p2 = {levelW/2 - 20, levelH/2 - 20, 40, 40};
    int speed = 5;
    int running = 1;
    SDL_Event e;
    const unsigned char *keys = SDL_GetKeyboardState(NULL);
    
    while(running) {
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) running = 0;
            if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                handleGuideClick(&game, e.button.x, e.button.y);
            }
            if(e.type == SDL_KEYDOWN) {
                switch(e.key.keysym.sym) {
                    case SDLK_ESCAPE: running = 0; break;
                    case SDLK_m:
                        game.displayMode = (game.displayMode == MODE_MONO) ? MODE_MULTI : MODE_MONO;
                        break;
                    case SDLK_p:
                        if(game.paused) {
                            game.startTime = SDL_GetTicks() - game.pauseTime;
                            game.paused = 0;
                        } else {
                            game.pauseTime = SDL_GetTicks() - game.startTime;
                            game.paused = 1;
                        }
                        break;
                    case SDLK_n:
                        curLevel = (curLevel == 1) ? 2 : 1;
                        sprintf(path, "images/bg%d.png", curLevel);
                        tempSurface = IMG_Load(path);
                        levelW = 3000;
                        levelH = 3000;
                        if(tempSurface) {
                            levelW = tempSurface->w;
                            levelH = tempSurface->h;
                            SDL_FreeSurface(tempSurface);
                        }
                        initBackground(&game, ren, curLevel, levelW, levelH);
                        p1.x = levelW/2 - 20;
                        p1.y = levelH/2 - 20;
                        p2.x = levelW/2 - 20;
                        p2.y = levelH/2 - 20;
                        break;
                }
            }
        }
        
        if(!game.paused) {
            if(keys[SDL_SCANCODE_D]) p1.x += speed;
            if(keys[SDL_SCANCODE_A]) p1.x -= speed;
            if(keys[SDL_SCANCODE_S]) p1.y += speed;
            if(keys[SDL_SCANCODE_W]) p1.y -= speed;
            
            if(keys[SDL_SCANCODE_RIGHT]) p2.x += speed;
            if(keys[SDL_SCANCODE_LEFT]) p2.x -= speed;
            if(keys[SDL_SCANCODE_DOWN]) p2.y += speed;
            if(keys[SDL_SCANCODE_UP]) p2.y -= speed;
            
            checkGemCollection(&game, &p1, 1);
            checkGemCollection(&game, &p2, 2);
            
            if(game.currentLevel == 1 && checkPortalCollision(&game, &p1)) {
                game.levelComplete = 1;
                game.levelCompleteTime = SDL_GetTicks();
            }
            
            updateCamera(&game, p1, 1);
            
            if(p1.x < game.camera1.x) p1.x = game.camera1.x;
            if(p1.y < game.camera1.y) p1.y = game.camera1.y;
            if(p1.x + p1.w > game.camera1.x + game.camera1.w) 
                p1.x = game.camera1.x + game.camera1.w - p1.w;
            if(p1.y + p1.h > game.camera1.y + game.camera1.h) 
                p1.y = game.camera1.y + game.camera1.h - p1.h;
            
            if(p1.x < 0) p1.x = 0;
            if(p1.y < 0) p1.y = 0;
            if(p1.x + p1.w > game.levelWidth) p1.x = game.levelWidth - p1.w;
            if(p1.y + p1.h > game.levelHeight) p1.y = game.levelHeight - p1.h;
            
            if(game.displayMode == MODE_MULTI) {
                updateCamera(&game, p2, 2);
                
                if(p2.x < game.camera2.x) p2.x = game.camera2.x;
                if(p2.y < game.camera2.y) p2.y = game.camera2.y;
                if(p2.x + p2.w > game.camera2.x + game.camera2.w) 
                    p2.x = game.camera2.x + game.camera2.w - p2.w;
                if(p2.y + p2.h > game.camera2.y + game.camera2.h) 
                    p2.y = game.camera2.y + game.camera2.h - p2.h;
                
                if(p2.x < 0) p2.x = 0;
                if(p2.y < 0) p2.y = 0;
                if(p2.x + p2.w > game.levelWidth) p2.x = game.levelWidth - p2.w;
                if(p2.y + p2.h > game.levelHeight) p2.y = game.levelHeight - p2.h;
            }
            
            updateCamera(&game, p1, 1);
            if(game.displayMode == MODE_MULTI) updateCamera(&game, p2, 2);
        }
        
        if(game.levelComplete && !game.paused) {
            if(SDL_GetTicks() - game.levelCompleteTime > 2000) {
                game.currentLevel = 2;
                game.levelComplete = 0;
                
                char path2[100];
                sprintf(path2, "images/bg2.png");
                SDL_Surface *tempSurface2 = IMG_Load(path2);
                int levelW2 = 3000, levelH2 = 3000;
                if(tempSurface2) {
                    levelW2 = tempSurface2->w;
                    levelH2 = tempSurface2->h;
                    SDL_FreeSurface(tempSurface2);
                }
                
                initBackground(&game, ren, 2, levelW2, levelH2);
                
                p1.x = levelW2/2 - 20;
                p1.y = levelH2/2 - 20;
                p2.x = levelW2/2 - 20;
                p2.y = levelH2/2 - 20;
            }
        }
        
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderClear(ren);
        
        if(game.displayMode == MODE_MONO) {
            drawBackground(game, ren, game.camera1);
            drawGems(game, ren, game.camera1);
            drawPortal(game, ren, game.camera1);
            
            SDL_Rect sp = {p1.x - game.camera1.x, p1.y - game.camera1.y, 40, 40};
            SDL_SetRenderDrawColor(ren, 0, 255, 0, 255);
            SDL_RenderFillRect(ren, &sp);
            SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
            SDL_RenderDrawRect(ren, &sp);
        }
        else {
            SDL_Rect vp1 = {0, 0, 640, 720};
            SDL_RenderSetViewport(ren, &vp1);
            drawBackground(game, ren, game.camera1);
            drawGems(game, ren, game.camera1);
            drawPortal(game, ren, game.camera1);
            
            SDL_Rect sp1 = {p1.x - game.camera1.x, p1.y - game.camera1.y, 40, 40};
            SDL_SetRenderDrawColor(ren, 0, 255, 0, 255);
            SDL_RenderFillRect(ren, &sp1);
            SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
            SDL_RenderDrawRect(ren, &sp1);
            
            SDL_Rect vp2 = {640, 0, 640, 720};
            SDL_RenderSetViewport(ren, &vp2);
            drawBackground(game, ren, game.camera2);
            drawGems(game, ren, game.camera2);
            drawPortal(game, ren, game.camera2);
            
            SDL_Rect sp2 = {p2.x - game.camera2.x, p2.y - game.camera2.y, 40, 40};
            SDL_SetRenderDrawColor(ren, 255, 255, 0, 255);
            SDL_RenderFillRect(ren, &sp2);
            SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
            SDL_RenderDrawRect(ren, &sp2);
            
            SDL_Rect full = {0, 0, 1280, 720};
            SDL_RenderSetViewport(ren, &full);
            SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
            SDL_RenderDrawLine(ren, 640, 0, 640, 720);
        }
        
        drawGuide(game, ren);
        drawTimer(game, ren);
        drawScores(game, ren);
        
        if(game.font && !game.paused) {
            char mode[20];
            sprintf(mode, "Mode: %s | Niv: %d", game.displayMode == MODE_MONO ? "SOLO" : "MULTI", game.currentLevel);
            SDL_Surface *sf = TTF_RenderText_Solid(game.font, mode, (SDL_Color){255,255,255});
            if(sf) {
                SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, sf);
                SDL_Rect pos = {20, 60, sf->w, sf->h};
                SDL_RenderCopy(ren, tex, NULL, &pos);
                SDL_FreeSurface(sf);
                SDL_DestroyTexture(tex);
            }
        }
        
        if(game.levelComplete && game.font) {
            SDL_Surface *sf = TTF_RenderText_Solid(game.font, "NIVEAU COMPLETE ! Passage au niveau 2...", (SDL_Color){255,255,0});
            if(sf) {
                SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, sf);
                SDL_Rect pos = {640 - sf->w/2, 360 - sf->h/2, sf->w, sf->h};
                SDL_RenderCopy(ren, tex, NULL, &pos);
                SDL_FreeSurface(sf);
                SDL_DestroyTexture(tex);
            }
        }
        
        if(game.paused && game.font) {
            SDL_Surface *sf = TTF_RenderText_Solid(game.font, "PAUSE", (SDL_Color){255,0,0});
            if(sf) {
                SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, sf);
                SDL_Rect pos = {640 - sf->w/2, 360 - sf->h/2, sf->w, sf->h};
                SDL_RenderCopy(ren, tex, NULL, &pos);
                SDL_FreeSurface(sf);
                SDL_DestroyTexture(tex);
            }
        }
        
        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }
    
    SDL_DestroyTexture(game.image);
    TTF_CloseFont(game.font);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
