#include <SDL2/SDL.h>          // Bibliothèque principale SDL (fenêtre, rendu, événements)
#include <SDL2/SDL_image.h>    // Pour charger les images (PNG)
#include <stdio.h>             // Pour printf
#include <stdlib.h>            // Fonctions générales (malloc, etc.)
#include <time.h>              // Pour time()
/*#include <time.h> permet d’utiliser des fonctions liées au temps, notamment time() qui sert à initialiser le générateur aléatoire avec srand() pour éviter d’avoir les mêmes résultats à chaque exécution.*/
#include "ennemi.h"            // Ton fichier header (structures + fonctions)

int main(int argc, char *argv[]) {
    SDL_Window *window = NULL;      // Pointeur vers la fenêtre
    SDL_Renderer *renderer = NULL;  // Pointeur vers le renderer (affichage)
    SDL_Event event;                // Variable pour gérer les événements
    int continuer = 1;              // Variable de contrôle de la boucle principale
    
    // Initialisation de SDL (vidéo)
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur SDL: %s\n", SDL_GetError()); // Affiche erreur
        return 1;
    }
    
    // Initialisation de SDL_image pour PNG
    if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("Erreur IMG: %s\n", IMG_GetError());
        SDL_Quit(); // Libération SDL
        return 1;
    }
    
    // Création de la fenêtre
    window = SDL_CreateWindow("Enemy - ",
                              SDL_WINDOWPOS_CENTERED,  // Position X centrée
                              SDL_WINDOWPOS_CENTERED,  // Position Y centrée
                              800, 500,                // Taille fenêtre
                              SDL_WINDOW_SHOWN);       // Visible
    if(!window) {
        printf("Erreur fenêtre\n");
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
    
    // Création du renderer (moteur graphique)
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer) {
        printf("Erreur renderer\n");
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
    
    // Chargement du background (fond)
    SDL_Surface* surface = IMG_Load("bg.png"); // Charger image en surface
    SDL_Texture* background = NULL;
    if(surface) {
        background = SDL_CreateTextureFromSurface(renderer, surface); // Convertir en texture
        SDL_FreeSurface(surface); // Libérer la surface
        printf("Background chargé\n");
    } else {
        printf("bg.png non trouvé\n");
    }
    
    // Chargement de l'image du joueur
    surface = IMG_Load("joueur.png");
    if(!surface) {
        printf("ERREUR: joueur.png non trouvé!\n");
        return 1;
    }
    SDL_Texture* joueurTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    // Initialisation du joueur
    perso player;
    
    // Position du joueur à la fin de la trajectoire de l'ennemi
    player.poshero.x = 700;
    player.poshero.y = 400;
    player.poshero.w = 60;  // Largeur
    player.poshero.h = 100; // Hauteur
    
    player.texture = joueurTexture; // Associer texture
    player.vie = 100;               // Vie initiale
    player.invincible = 0;          // Pas invincible
    player.lastHit = 0;             // Aucun coup reçu
    
    // Initialisation de l'ennemi
    Ennemi enemy;
    initEnnemi(&enemy);                      // Initialiser les valeurs
    chargerTexturesEnnemi(&enemy, renderer); // Charger ses images
    
    // Affichage d'informations dans la console
    printf("\n========================================\n");
    printf("=== ENNEMI TRAJECTOIRE EN LIGNE ===\n");
    printf("L'ennemi se déplace horizontalement\n");
    printf("de x=100 à x=700 (va-et-vient)\n");
    printf("JOUEUR positionné à la FIN de la ligne (x=700, y=400)\n");
    printf("========================================\n");
    
    int mouseX, mouseY; // Position de la souris
    
    // Boucle principale du jeu
    while(continuer) {
        
        // Gestion des événements (clavier, souris, fermeture...)
        while(SDL_PollEvent(&event)) {
            
            if(event.type == SDL_QUIT) { // Si on ferme la fenêtre
                continuer = 0;
            }
            
            // Si clic gauche de la souris
            if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                mouseX = event.button.x; // Position X souris
                mouseY = event.button.y; // Position Y souris
                
                // Vérifier si le clic est sur l'ennemi
                if(enemy.actif &&
                   mouseX >= enemy.posEnnemi.x && mouseX <= enemy.posEnnemi.x + enemy.posEnnemi.w &&
                   mouseY >= enemy.posEnnemi.y && mouseY <= enemy.posEnnemi.y + enemy.posEnnemi.h) {
                    
                    ennemiPrendreDegats(&enemy, 20); // Infliger 20 dégâts
                }
            }
        }
        
        // Animation de l'ennemi
        animerEnnemi(&enemy);
        
        // Déplacement de l'ennemi (va-et-vient)
        deplacerLigne(&enemy);
        
        // Gestion des collisions joueur/ennemi
        gererCollision(&player, &enemy);
        
        // Vérifier si le joueur est mort
        if(player.vie <= 0) {
            printf("\n=== GAME OVER ===\n");
            break;
        }
        
        // Vérifier si l'ennemi est mort
        if(!enemy.actif) {
            printf("\n=== VICTOIRE! ===\n");
            break;
        }
        
        // Nettoyage de l'écran (avant redraw)
        SDL_RenderClear(renderer);
        
        // Afficher le background
        if(background) {
            SDL_RenderCopy(renderer, background, NULL, NULL);
        } else {
            // Si pas d'image, fond gris
            SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
            SDL_RenderClear(renderer);
        }
        
        // Afficher le joueur
        SDL_RenderCopy(renderer, player.texture, NULL, &player.poshero);
        
        // Afficher l'ennemi
        afficherEnnemi(enemy, renderer);
        
        // BARRE DE VIE JOUEUR (verte)
        SDL_Rect barreJoueur = {10, 10, player.vie * 2, 20}; // largeur = vie * 2
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &barreJoueur);
        
        // Contour blanc
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_Rect contourJ = {10, 10, 200, 20};
        SDL_RenderDrawRect(renderer, &contourJ);
        
        // BARRE DE VIE ENNEMI (rouge)
        int barreLargeur = enemy.vie * 2;
        if(barreLargeur < 0) barreLargeur = 0; // éviter largeur négative
        
        SDL_Rect barreEnnemi = {800 - barreLargeur - 10, 10, barreLargeur, 20};
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &barreEnnemi);
        
        // Contour blanc
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_Rect contourE = {800 - 210, 10, 200, 20};
        SDL_RenderDrawRect(renderer, &contourE);
        
        // Dessiner la trajectoire (points jaunes)
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 150);
        for(int x = 100; x <= 700; x += 10) {
            SDL_RenderDrawPoint(renderer, x, 400);
        }
        
        // Marqueur joueur (vert)
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_Rect marqueurJoueur = {700, 390, 10, 20};
        SDL_RenderFillRect(renderer, &marqueurJoueur);
        
        // Marqueur départ ennemi (rouge)
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_Rect marqueurDepart = {100, 390, 10, 20};
        SDL_RenderFillRect(renderer, &marqueurDepart);
        
        // Afficher tout à l'écran
        SDL_RenderPresent(renderer);
        
        SDL_Delay(16); // Pause ~16ms (≈ 60 FPS)
    }
    
    // Libération mémoire (important)
    if(background) SDL_DestroyTexture(background);
    if(joueurTexture) SDL_DestroyTexture(joueurTexture);
    
    // Libérer textures ennemi
    for(int i = 0; i < 5; i++) {
        if(enemy.textures[i]) SDL_DestroyTexture(enemy.textures[i]);
    }
    
    SDL_DestroyRenderer(renderer); // Détruire renderer
    SDL_DestroyWindow(window);     // Détruire fenêtre
    IMG_Quit();                   // Quitter SDL_image
    SDL_Quit();                   // Quitter SDL
    
    return 0; // Fin programme
}
