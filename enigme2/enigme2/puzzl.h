#ifndef PUZZL_H_INCLUDED
#define PUZZL_H_INCLUDED

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define LARGEUR_ECRAN 1000
#define HAUTEUR_ECRAN 700
#define NB_PUZZLES 5
#define NB_PROPOSITIONS 3

typedef struct {
    SDL_Surface *image;
    SDL_Rect position;
    int identifiant;
    int est_correct;
    int est_placee;
} Proposition;

typedef struct {
    SDL_Surface *images_completes[NB_PUZZLES];
    SDL_Surface *emplacement_trou;
    Proposition propositions[NB_PROPOSITIONS];
    SDL_Rect zone_image_principale;
    SDL_Rect zone_trou;
    SDL_Rect zones_propositions[NB_PROPOSITIONS];
    int puzzle_courant;
    int bonne_proposition_index;
    int selection_courante;
    int mode_glisse;
    SDL_Rect position_glisse;
    SDL_Surface *image_glissee;
    int reussite;
    int termine;
    SDL_Surface *fond;
    SDL_Surface *bouton_suivant_normal;
    SDL_Surface *bouton_suivant_survol;
    SDL_Surface *bouton_retour_normal;
    SDL_Surface *bouton_retour_survol;
    SDL_Rect position_bouton_suivant;
    SDL_Rect position_bouton_retour;
    int survol_suivant;
    int survol_retour;
    Uint32 temps_depart;
    int duree_limite;
    int temps_restant;
    int chrono_actif;
    Mix_Chunk *son_reussite;
    Mix_Chunk *son_echec;
    Mix_Chunk *son_compte_a_rebours;
    Mix_Chunk *son_selection;
    Mix_Chunk *son_depot;
    TTF_Font *police_normale;
    SDL_Color couleur_blanc;
    SDL_Color couleur_vert;
    SDL_Color couleur_rouge;
    SDL_Color couleur_jaune;
    int jeu_actif;
    int afficher_message;
    Uint32 temps_message;
    float angle_message;
    float echelle_message;
    SDL_Surface *image_rot;
    SDL_Surface *image_succes;
    SDL_Surface *image_echec;
    int chrono_joue;
} EnsemblePuzzle;

void initialiser_puzzle(EnsemblePuzzle *p);
void generer_puzzle(EnsemblePuzzle *p);
void afficher_puzzle(EnsemblePuzzle p, SDL_Surface *ecran);
int gerer_evenement(EnsemblePuzzle *p, SDL_Event evenement, int *points, int *vies_restantes);
void mettre_a_jour_temps(EnsemblePuzzle *p);
void liberer_puzzle(EnsemblePuzzle *p);
void mettre_a_jour_rotzoom(EnsemblePuzzle *p);

#endif
