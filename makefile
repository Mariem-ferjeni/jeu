
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <time.h>
#include "ennemi.h"

int main(int argc, char *argv[])
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Event event;
    TTF_Font *police = NULL;
    SDL_Color couleurBlanche = {255, 255, 255, 255};
    SDL_Color couleurVerte = {0, 255, 0, 255};
    SDL_Color couleurRouge = {255, 0, 0, 255};
    SDL_Color couleurJaune = {255, 255, 0, 255};
    
    int continuer = 1;
    int niveauActuel = 1;
    int toucheNPrecedente = 0;
    int score = 0;
    int ecranFin = 0;
    int victoire = 0;
    int mouseX, mouseY;
    
    Coin coin;
    
    SDL_Rect meteorite;
    SDL_Texture* meteoriteTexture = NULL;
    int meteoriteActif = 1;
    int meteoriteVitesse = 4;
    
    SDL_Rect fumee;
    SDL_Texture* fumeeTexture = NULL;
    int fumeeActif = 0;
    int fumeeCompteur = 0;
    
    Mix_Chunk* sonCollision = NULL;
    Mix_Chunk* sonCoin = NULL;
    
    if(SDL_Init(SDL_INIT_VIDEO) < 0) return 1;
    if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) { SDL_Quit(); return 1; }
    if(TTF_Init() < 0) { printf("Erreur TTF: %s\n", TTF_GetError()); return 1; }
    
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Erreur audio: %s\n", Mix_GetError());
    }
    
    sonCollision = Mix_LoadWAV("touch.mp3");
    if(sonCollision == NULL) {
        printf("Erreur chargement touch.mp3: %s\n", Mix_GetError());
    }
    
    sonCoin = Mix_LoadWAV("coin.mp3");
    if(sonCoin == NULL) {
        printf("Erreur chargement coin.mp3: %s\n", Mix_GetError());
    } else {
        printf("coin.wav charge avec succes!\n");
    }
    
    Mix_Music* musiqueFond = Mix_LoadMUS("background.mp3");
    if(musiqueFond) {
        Mix_PlayMusic(musiqueFond, -1);
    } else {
        printf("Erreur chargement background.mp3: %s\n", Mix_GetError());
    }
    
    police = TTF_OpenFont("arial.ttf", 24);
    if(!police) police = TTF_OpenFont("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf", 24);
    
    window = SDL_CreateWindow("Enemy - IA Deplacement Automatique", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 500, SDL_WINDOW_SHOWN);
    if(!window) { IMG_Quit(); SDL_Quit(); return 1; }
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer) { SDL_DestroyWindow(window); IMG_Quit(); SDL_Quit(); return 1; }
    
    SDL_Texture* backgroundNiveau1 = NULL;
    SDL_Texture* backgroundNiveau2 = NULL;
    SDL_Texture* backgroundActuel = NULL;
    
    SDL_Surface* surface = IMG_Load("bg.png");
    if(surface) { backgroundNiveau1 = SDL_CreateTextureFromSurface(renderer, surface); SDL_FreeSurface(surface); }
    
    surface = IMG_Load("bg2.png");
    if(surface) { backgroundNiveau2 = SDL_CreateTextureFromSurface(renderer, surface); SDL_FreeSurface(surface); }
    
    backgroundActuel = backgroundNiveau1;
    
    surface = IMG_Load("coin.png");
    if(surface) { 
        coin.texture = SDL_CreateTextureFromSurface(renderer, surface); 
        SDL_FreeSurface(surface);
        printf("coin.png charge avec succes!\n");
    } else { 
        printf("ERREUR: coin.png non trouve! %s\n", IMG_GetError());
        coin.texture = NULL;
    }
    
    surface = IMG_Load("meteorite.png");
    if(surface) { 
        meteoriteTexture = SDL_CreateTextureFromSurface(renderer, surface); 
        SDL_FreeSurface(surface);
    }
    
    surface = IMG_Load("fumee.png");
    if(surface) { 
        fumeeTexture = SDL_CreateTextureFromSurface(renderer, surface); 
        SDL_FreeSurface(surface);
    }
    
    surface = IMG_Load("joueur.png");
    if(!surface) return 1;
    SDL_Texture* joueurTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    perso player;
    player.poshero.x = 700;
    player.poshero.y = 400;
    player.poshero.w = 60;
    player.poshero.h = 100;
    player.texture = joueurTexture;
    player.vie = 100;
    player.invincible = 0;
    player.lastHit = 0;
    
    coin.rect.x = 400;
    coin.rect.y = 430;
    coin.rect.w = 30;
    coin.rect.h = 30;
    coin.actif = 1;
    coin.respawnTimer = 0;
    coin.vitesseY = 0;
    coin.texture = coin.texture;
    
    Ennemi enemy;
    initEnnemi(&enemy);
    chargerTextures(&enemy, renderer);
    
    meteorite.x = rand() % 680;
    meteorite.y = -120;
    meteorite.w = 120;
    meteorite.h = 120;
    meteoriteActif = 1;
    
    fumee.w = 80;
    fumee.h = 80;
    fumee.x = 0;
    fumee.y = 0;
    fumeeActif = 0;
    fumeeCompteur = 0;
    
    printf("\n=== LEVEL 1 - MOUVEMENT RECTILIGNE ===\n");
    printf("Deplacez-vous avec les fleches\n");
    printf("L'ennemi a une IA: vous suit quand vous etes proche!\n");
    printf("Les COINS donnent +10 vie\n");
    printf("Evitez l'ennemi = -10 vie\n");
    printf("Appuyez sur N pour passer au niveau 2\n\n");
    
    while(continuer) {
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) continuer = 0;
            
            if(ecranFin == 0) {
                if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_n && !toucheNPrecedente) {
                    toucheNPrecedente = 1;
                    if(niveauActuel == 1) {
                        niveauActuel = 2;
                        backgroundActuel = backgroundNiveau2;
                        enemy.niveau = 2;
                        enemy.posEnnemi.x = 100;
                        enemy.posEnnemi.y = 350;
                        enemy.posDepart.x = 100;
                        enemy.posDepart.y = 350;
                        enemy.posArrivee.x = 700;
                        enemy.posArrivee.y = 350;
                        enemy.direction = 0;
                        enemy.vie = 100;
                        enemy.actif = 1;
                        enemy.angleVague = 0;
                        enemy.vitesse = enemy.vitesseOriginale;
                        chargerTextures(&enemy, renderer);
                        meteoriteActif = 0;
                        fumeeActif = 0;
                        fumeeCompteur = 0;
                        printf("=== NIVEAU 2 - MOUVEMENT SINUSOIDAL ===\n");
                    } else {
                        niveauActuel = 1;
                        backgroundActuel = backgroundNiveau1;
                        enemy.niveau = 1;
                        enemy.posEnnemi.x = 100;
                        enemy.posEnnemi.y = 400;
                        enemy.posDepart.x = 100;
                        enemy.posDepart.y = 400;
                        enemy.posArrivee.x = 700;
                        enemy.posArrivee.y = 400;
                        enemy.direction = 0;
                        enemy.vie = 100;
                        enemy.actif = 1;
                        enemy.vitesse = enemy.vitesseOriginale;
                        chargerTextures(&enemy, renderer);
                        meteoriteActif = 1;
                        meteorite.x = rand() % 680;
                        meteorite.y = -120;
                        fumeeActif = 0;
                        printf("=== NIVEAU 1 - MOUVEMENT RECTILIGNE ===\n");
                    }
                }
                
                if(event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_n) toucheNPrecedente = 0;
                
                const Uint8* touches = SDL_GetKeyboardState(NULL);
                if(touches[SDL_SCANCODE_LEFT]) player.poshero.x -= 8;
                if(touches[SDL_SCANCODE_RIGHT]) player.poshero.x += 8;
                if(touches[SDL_SCANCODE_UP]) player.poshero.y -= 8;
                if(touches[SDL_SCANCODE_DOWN]) player.poshero.y += 8;
                
                if(player.poshero.x < 0) player.poshero.x = 0;
                if(player.poshero.x + player.poshero.w > 800) player.poshero.x = 800 - player.poshero.w;
                if(player.poshero.y < 0) player.poshero.y = 0;
                if(player.poshero.y + player.poshero.h > 500) player.poshero.y = 500 - player.poshero.h;
                
                deplacerIA(&enemy, player.poshero.x, player.poshero.y);
                gererCollision(&player, &enemy, &coin);
                
                if(coin.actif == 0) {
                    if(sonCoin) {
                        Mix_PlayChannel(-1, sonCoin, 0);
                    }
                }
                
                if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                    mouseX = event.button.x;
                    mouseY = event.button.y;
                    if(enemy.actif && mouseX >= enemy.posEnnemi.x && mouseX <= enemy.posEnnemi.x + enemy.posEnnemi.w &&
                       mouseY >= enemy.posEnnemi.y && mouseY <= enemy.posEnnemi.y + enemy.posEnnemi.h) {
                        ennemietat(&enemy, 20);
                        if(sonCollision) Mix_PlayChannel(-1, sonCollision, 0);
                        if(enemy.vie <= 0) score += 10;
                        printf("Ennemi attaque! Vie: %d\n", enemy.vie);
                    }
                }
            }
            
            if(ecranFin == 1) {
                if(event.type == SDL_KEYDOWN) {
                    if(event.key.keysym.sym == SDLK_r) {
                        score = 0;
                        player.vie = 100;
                        player.poshero.x = 700;
                        player.poshero.y = 400;
                        ecranFin = 0;
                        victoire = 0;
                        niveauActuel = 1;
                        backgroundActuel = backgroundNiveau1;
                        initEnnemi(&enemy);
                        chargerTextures(&enemy, renderer);
                        meteoriteActif = 1;
                        meteorite.x = rand() % 680;
                        meteorite.y = -120;
                        fumeeActif = 0;
                        fumeeCompteur = 0;
                        coin.actif = 1;
                        coin.rect.x = 400;
                        coin.rect.y = 430;
                        coin.respawnTimer = 0;
                        printf("=== JEU RECOMMENCE ===\n");
                    }
                    if(event.key.keysym.sym == SDLK_q) continuer = 0;
                }
            }
        }
        
        if(ecranFin == 0) {
            updateEnnemi(&enemy);
            
            if(niveauActuel == 1 && meteoriteActif) {
                meteorite.y += meteoriteVitesse;
                
                if(meteorite.y > 500) {
                    meteorite.x = rand() % 680;
                    meteorite.y = -120;
                }
                
                if(meteorite.x < enemy.posEnnemi.x + enemy.posEnnemi.w &&
                   meteorite.x + meteorite.w > enemy.posEnnemi.x &&
                   meteorite.y < enemy.posEnnemi.y + enemy.posEnnemi.h &&
                   meteorite.y + meteorite.h > enemy.posEnnemi.y) {
                    ennemietat(&enemy, 15);
                    if(sonCollision) Mix_PlayChannel(-1, sonCollision, 0);
                    meteorite.x = rand() % 680;
                    meteorite.y = -120;
                    printf("Meteorite! Vie: %d\n", enemy.vie);
                }
            }
            
            if(niveauActuel == 2) {
                if(!fumeeActif && (rand() % 100) < 2) {
                    fumeeActif = 1;
                    fumee.x = enemy.posEnnemi.x + (enemy.posEnnemi.w - fumee.w) / 2;
                    fumee.y = enemy.posEnnemi.y + (enemy.posEnnemi.h - fumee.h) / 2;
                    fumeeCompteur = 0;
                    printf("Fumee apparait!\n");
                }
                
                if(fumeeActif) {
                    fumeeCompteur++;
                    if(fumeeCompteur == 1) {
                        ennemietat(&enemy, 5);
                        if(sonCollision) Mix_PlayChannel(-1, sonCollision, 0);
                        printf("Fumee degats! Vie: %d\n", enemy.vie);
                    }
                    if(fumeeCompteur > 30) {
                        fumeeActif = 0;
                        fumeeCompteur = 0;
                        printf("Fumee disparait\n");
                    }
                }
            }
            
            if(enemy.vie <= 0 && !victoire) { victoire = 1; ecranFin = 1; printf("VICTOIRE! Score: %d\n", score); }
            if(player.vie <= 0) { ecranFin = 1; printf("GAME OVER! Score: %d\n", score); }
        }
        
        SDL_RenderClear(renderer);
        
        if(backgroundActuel && ecranFin == 0) {
            SDL_RenderCopy(renderer, backgroundActuel, NULL, NULL);
        } else if(ecranFin == 0) {
            SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
            SDL_RenderClear(renderer);
        }
        
        if(ecranFin == 0) {
            if(coin.actif) {
                if(coin.texture) {
                    SDL_RenderCopy(renderer, coin.texture, NULL, &coin.rect);
                } else {
                    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
                    SDL_RenderFillRect(renderer, &coin.rect);
                }
            }
            
            if(niveauActuel == 1 && meteoriteActif && meteoriteTexture) {
                SDL_RenderCopy(renderer, meteoriteTexture, NULL, &meteorite);
            }
            
            if(niveauActuel == 2 && fumeeActif && fumeeTexture) {
                SDL_RenderCopy(renderer, fumeeTexture, NULL, &fumee);
            }
            
            SDL_RenderCopy(renderer, player.texture, NULL, &player.poshero);
            afficherEnnemi(enemy, renderer);
            
            SDL_Rect barreJoueur = {10, 10, player.vie * 2, 20};
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_RenderFillRect(renderer, &barreJoueur);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_Rect contourJ = {10, 10, 200, 20};
            SDL_RenderDrawRect(renderer, &contourJ);
            
            int barreLargeur = enemy.vie * 2;
            if(barreLargeur < 0) barreLargeur = 0;
            SDL_Rect barreEnnemi = {800 - barreLargeur - 10, 10, barreLargeur, 20};
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderFillRect(renderer, &barreEnnemi);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_Rect contourE = {800 - 210, 10, 200, 20};
            SDL_RenderDrawRect(renderer, &contourE);
            
            char texteScore[50], texteVie[50], texteNiveau[50];
            sprintf(texteScore, "SCORE: %d", score);
            sprintf(texteVie, "VIE: %d", player.vie);
            sprintf(texteNiveau, "NIVEAU: %d", niveauActuel);
            
            if(police) {
                afficherTexte(renderer, police, texteScore, 10, 40, couleurBlanche);
                afficherTexte(renderer, police, texteVie, 10, 70, couleurVerte);
                afficherTexte(renderer, police, texteNiveau, 10, 100, couleurJaune);
            }
        }
        else {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            if(police) {
                if(victoire) {
                    afficherTexte(renderer, police, "VICTOIRE!", 350, 200, couleurVerte);
                    char txt[50]; sprintf(txt, "SCORE FINAL: %d", score);
                    afficherTexte(renderer, police, txt, 320, 270, couleurBlanche);
                } else {
                    afficherTexte(renderer, police, "GAME OVER", 340, 200, couleurRouge);
                    char txt[50]; sprintf(txt, "SCORE FINAL: %d", score);
                    afficherTexte(renderer, police, txt, 320, 270, couleurBlanche);
                }
                afficherTexte(renderer, police, "Appuyez sur R pour recommencer", 260, 350, couleurBlanche);
                afficherTexte(renderer, police, "Appuyez sur Q pour quitter", 290, 400, couleurBlanche);
            }
        }
        
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    
    if(backgroundNiveau1) SDL_DestroyTexture(backgroundNiveau1);
    if(backgroundNiveau2) SDL_DestroyTexture(backgroundNiveau2);
    if(coin.texture) SDL_DestroyTexture(coin.texture);
    if(meteoriteTexture) SDL_DestroyTexture(meteoriteTexture);
    if(fumeeTexture) SDL_DestroyTexture(fumeeTexture);
    if(joueurTexture) SDL_DestroyTexture(joueurTexture);
    for(int i = 0; i < 5; i++) if(enemy.textures[i]) SDL_DestroyTexture(enemy.textures[i]);
    
    if(sonCollision) Mix_FreeChunk(sonCollision);
    if(sonCoin) Mix_FreeChunk(sonCoin);
    if(musiqueFond) Mix_FreeMusic(musiqueFond);
    if(police) TTF_CloseFont(police);
    
    Mix_CloseAudio();
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    
    return 0;
}
