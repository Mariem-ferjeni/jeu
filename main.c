#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include "background.h"

void drawTimer(Background b, SDL_Renderer *r)
{
    Uint32 current;
    if(b.paused) current = b.pauseTime;
    else current = SDL_GetTicks() - b.startTime;
    Uint32 sec = current / 1000;
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
    
    SDL_Window *win = SDL_CreateWindow("Jeu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, 0);
    if(!win) return 1;
    
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if(!ren) return 1;
    
    int levelW = 3000, levelH = 720, curLevel = 1;
    Background game;
    initBackground(&game, ren, curLevel, levelW, levelH);
    
    SDL_Rect p1 = {100, 600, 40, 40};
    SDL_Rect p2 = {500, 600, 40, 40};
    int vy1 = 0, vy2 = 0, onGround1 = 0, onGround2 = 0;
    int gravity = 1, jump = -18, speed = 8;
    int running = 1;
    SDL_Event e;
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    while(running) {
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) running = 0;
            if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
                handleGuideClick(&game, e.button.x, e.button.y);
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
                        initBackground(&game, ren, curLevel, levelW, levelH);
                        p1.x = 100; p1.y = 600;
                        p2.x = 500; p2.y = 600;
                        vy1 = 0; vy2 = 0;
                        onGround1 = 1; onGround2 = 1;
                        break;
                }
            }
        }
        
        updatePlatforms(&game);
        
        vy1 += gravity;
        p1.y += vy1;
        if(keys[SDL_SCANCODE_RIGHT] && p1.x + p1.w < levelW) p1.x += speed;
        if(keys[SDL_SCANCODE_LEFT] && p1.x > 0) p1.x -= speed;
        if(keys[SDL_SCANCODE_UP] && onGround1) {
            vy1 = jump;
            onGround1 = 0;
        }
        checkCollision(&game, &p1, &onGround1, &vy1);
        if(p1.y + p1.h >= levelH) {
            p1.y = levelH - p1.h;
            onGround1 = 1;
            vy1 = 0;
        }
        if(p1.x < 0) p1.x = 0;
        if(p1.x + p1.w > levelW) p1.x = levelW - p1.w;
        
        if(game.displayMode == MODE_MULTI) {
            vy2 += gravity;
            p2.y += vy2;
            if(keys[SDL_SCANCODE_D] && p2.x + p2.w < levelW) p2.x += speed;
            if(keys[SDL_SCANCODE_A] && p2.x > 0) p2.x -= speed;
            if(keys[SDL_SCANCODE_W] && onGround2) {
                vy2 = jump;
                onGround2 = 0;
            }
            checkCollision(&game, &p2, &onGround2, &vy2);
            if(p2.y + p2.h >= levelH) {
                p2.y = levelH - p2.h;
                onGround2 = 1;
                vy2 = 0;
            }
            if(p2.x < 0) p2.x = 0;
            if(p2.x + p2.w > levelW) p2.x = levelW - p2.w;
        }
        
        updateCamera(&game, p1, 1);
        if(game.displayMode == MODE_MULTI) updateCamera(&game, p2, 2);
        
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderClear(ren);
        
        if(game.displayMode == MODE_MONO) {
            drawBackground(game, ren);
            drawPlatforms(game, ren, game.camera1);
            SDL_Rect sp = {p1.x - game.camera1.x, p1.y, 40, 40};
            SDL_SetRenderDrawColor(ren, 0, 255, 0, 255);
            SDL_RenderFillRect(ren, &sp);
        }
        else {
            SDL_Rect vp1 = {0, 0, 640, 720};
            SDL_RenderSetViewport(ren, &vp1);
            drawBackground(game, ren);
            drawPlatforms(game, ren, game.camera1);
            SDL_Rect sp1 = {p1.x - game.camera1.x, p1.y, 40, 40};
            SDL_SetRenderDrawColor(ren, 0, 255, 0, 255);
            SDL_RenderFillRect(ren, &sp1);
            
            SDL_Rect vp2 = {640, 0, 640, 720};
            SDL_RenderSetViewport(ren, &vp2);
            drawBackground(game, ren);
            drawPlatforms(game, ren, game.camera2);
            SDL_Rect sp2 = {p2.x - game.camera2.x, p2.y, 40, 40};
            SDL_SetRenderDrawColor(ren, 255, 255, 0, 255);
            SDL_RenderFillRect(ren, &sp2);
            
            SDL_Rect full = {0, 0, 1280, 720};
            SDL_RenderSetViewport(ren, &full);
        }
        
        drawGuide(game, ren);
        drawTimer(game, ren);
        
        if(game.font) {
            char mode[20];
            sprintf(mode, "Mode: %s", game.displayMode == MODE_MONO ? "MONO" : "MULTI");
            SDL_Surface *sf = TTF_RenderText_Solid(game.font, mode, (SDL_Color){255,255,255});
            if(sf) {
                SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, sf);
                SDL_Rect pos = {20, 60, sf->w, sf->h};
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
