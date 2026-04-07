#ifndef ENNEMI_H_INCLUDED   // Vérifie si ENNEMI_H_INCLUDED n'est pas encore défini
#define ENNEMI_H_INCLUDED   // Définit ENNEMI_H_INCLUDED pour éviter les inclusions multiples

#include <stdio.h>          // Bibliothèque standard pour printf
#include <SDL2/SDL.h>      // Bibliothèque principale SDL
#include <SDL2/SDL_image.h>// Bibliothèque pour charger les images 
#include <stdlib.h>        // Pour malloc, free, rand, srand
#include <time.h>          // Pour utiliser time() (initialisation aléatoire)

// Structure représentant un ennemi
typedef struct {
    SDL_Texture *textures[5];   // Tableau de 5 textures (images pour animation)
    int frameActuelle;          // Index de la frame actuelle affichée
    int compteurAnimation;      // Compteur pour contrôler la vitesse d'animation
    SDL_Rect posEnnemi;         // Rectangle représentant position + taille de l'ennemi
    SDL_Rect posDepart;         // Position de départ du déplacement
    SDL_Rect posArrivee;        // Position d'arrivée (limite du déplacement)
    int direction;              // Direction de déplacement (0 = droite, 1 = gauche)
    int vitesse;                // Vitesse de déplacement (pixels par frame)
    int vie;                    // Points de vie de l'ennemi
    int actif;                  // Etat (1 = vivant, 0 = mort/inactif)
    int degats;                 // Dégâts que l'ennemi inflige au joueur
} Ennemi;

// Structure représentant le joueur (perso)
typedef struct {
    SDL_Rect poshero;       // Rectangle position + taille du joueur
    SDL_Texture* texture;   // Texture du joueur
    int vie;                // Points de vie du joueur
    int invincible;         // Etat invincible (1 = oui, 0 = non)
    Uint32 lastHit;         // Temps du dernier coup reçu (en millisecondes)
} perso;

// Prototype des fonctions (déclarations)

// Initialise l'ennemi (position, vie, vitesse, etc.)
void initEnnemi(Ennemi *e);

// Charge les textures (images) de l'ennemi
void chargerTexturesEnnemi(Ennemi *e, SDL_Renderer *renderer);

// Affiche l'ennemi à l'écran
void afficherEnnemi(Ennemi e, SDL_Renderer *renderer);

// Déplace l'ennemi horizontalement (va-et-vient)
void deplacerLigne(Ennemi *e);

// Gère l'animation (changement de frames)
void animerEnnemi(Ennemi *e);

// Applique des dégâts à l'ennemi
void ennemiPrendreDegats(Ennemi *e, int degats);

// Détecte collision Bounding Box (rectangle vs rectangle)
int collisionBB(perso p, Ennemi e);

// Gère la collision joueur-ennemi (perte de vie)
void gererCollision(perso *p, Ennemi *e);

#endif // Fin de la condition de protection du header
