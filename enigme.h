/*#ifndef ENIGME_H_INCLUDED
#define ENIGME_H_INCLUDED

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#define MAX_QUESTIONS 100
#define TEMPS_PAR_QUESTION 10000  // 10 secondes

typedef struct {
    char question[256];
    int reponse;        // 1 = Vrai/Oui, 0 = Faux/Non
    int niveau;         // niveau de difficulté
    int deja_vu;        // 1 = déjà posée, 0 = pas encore posée
} Question;

typedef struct
{
    // Tableau de questions
    Question questions[MAX_QUESTIONS];
    int nb_questions;
    int question_actuelle;
    
    // Gestion du temps
    Uint32 temps_debut;
    int temps_restant;
    int chrono_actif;
    SDL_Rect barre_temps;
    int largeur_barre_initiale;
    
    // Score et vies
    int score;
    int vies;
    int niveau_actuel;
    
    // Sons
    Mix_Chunk *son_selection;   // Son quand on clique sur OUI/NON
    Mix_Chunk *son_erreur;      // Son quand réponse fausse
    Mix_Chunk *son_succes;      // Son quand réponse correcte
    Mix_Chunk *son_temps_ecoule; // Son quand temps écoulé
    
    // Polices
    TTF_Font *police_question;
    TTF_Font *police_reponse;
    TTF_Font *police_score;
    
    // Textures
    SDL_Texture *fond;
    SDL_Texture *texte_question;
    SDL_Texture *btn_oui;
    SDL_Texture *btn_non;
    SDL_Texture *msg_reussi;
    SDL_Texture *msg_echoue;
    SDL_Texture *menu_barre;
    SDL_Texture *btn_reprendre;
    SDL_Texture *btn_sauvegarder;
    SDL_Texture *btn_quitter;
    SDL_Texture *texte_score;
    SDL_Texture *texte_vies;
    SDL_Texture *texte_niveau;
    
    // Positions
    SDL_Rect pos_fond;
    SDL_Rect pos_question;
    SDL_Rect pos_btn_oui;
    SDL_Rect pos_btn_non;
    SDL_Rect pos_msg;
    SDL_Rect pos_menu;
    SDL_Rect pos_btn_reprendre;
    SDL_Rect pos_btn_sauvegarder;
    SDL_Rect pos_btn_quitter;
    SDL_Rect pos_score;
    SDL_Rect pos_vies;
    SDL_Rect pos_niveau;
    SDL_Rect pos_barre_temps;
    
    // États
    int en_cours;
    int resultat_affiche;
    int menu_actif;
    int reponse_donnee;
    int enigme_terminee;
    
    SDL_Renderer *renderer;
} Enigme;

// Initialisation et nettoyage
void enigme_init(Enigme *e, SDL_Renderer *renderer);
void enigme_liberer(Enigme *e);

// Chargement des questions depuis fichier
int enigme_charger_questions(Enigme *e, const char *nom_fichier);

// Génération et logique
void enigme_generer_question_aleatoire(Enigme *e);
int enigme_verifier_reponse(Enigme *e, int reponse_choisie);
void enigme_reinitialiser(Enigme *e);
void enigme_reinitialiser_dejavu(Enigme *e);

// Gestion du chronomètre
void enigme_demarrer_chrono(Enigme *e);
void enigme_mettre_a_jour_chrono(Enigme *e);
int enigme_chrono_termine(Enigme *e);

// Sons
void enigme_charger_sons(Enigme *e);
void enigme_jouer_son_selection(Enigme *e);
void enigme_jouer_son_erreur(Enigme *e);
void enigme_jouer_son_succes(Enigme *e);

// Affichage
void enigme_afficher(Enigme *e);
void enigme_afficher_resultat(Enigme *e);
void enigme_afficher_menu(Enigme *e);
void enigme_afficher_barre_temps(Enigme *e);
void enigme_mettre_a_jour_affichage_score(Enigme *e);

// Gestion des événements
void enigme_gerer_clic(Enigme *e, int x, int y, int *continuer, int *en_jeu);

// Sauvegarde
void enigme_sauvegarder(Enigme *e);
void enigme_charger(Enigme *e);

#endif*/
#ifndef ENIGME_H_INCLUDED
#define ENIGME_H_INCLUDED

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#define MAX_QUESTIONS 100
#define TEMPS_PAR_QUESTION 10000  // 10 secondes

typedef struct {
    char question[256];
    int reponse;        // 1 = Vrai/Oui, 0 = Faux/Non
    int niveau;         // niveau de difficulté
    int deja_vu;        // 1 = déjà posée, 0 = pas encore posée
} Question;

typedef struct
{
    // Tableau de questions
    Question questions[MAX_QUESTIONS];
    int nb_questions;
    int question_actuelle;
    
    // Gestion du temps
    Uint32 temps_debut;
    int temps_restant;
    int chrono_actif;
    SDL_Rect barre_temps;
    int largeur_barre_initiale;
    
    // Score et vies
    int score;
    int vies;
    int niveau_actuel;
    
    // Sons
    Mix_Chunk *son_selection;   // Son quand on clique sur OUI/NON
    Mix_Chunk *son_erreur;      // Son quand réponse fausse
    Mix_Chunk *son_succes;      // Son quand réponse correcte
    Mix_Chunk *son_temps_ecoule; // Son quand temps écoulé
    
    // Polices
    TTF_Font *police_question;
    TTF_Font *police_reponse;
    TTF_Font *police_score;
    
    // Textures
    SDL_Texture *fond;
    SDL_Texture *texte_question;
    SDL_Texture *btn_oui;
    SDL_Texture *btn_non;
    SDL_Texture *msg_reussi;
    SDL_Texture *msg_echoue;
    SDL_Texture *texte_score;
    SDL_Texture *texte_vies;
    SDL_Texture *texte_niveau;
    
    // Positions
    SDL_Rect pos_fond;
    SDL_Rect pos_question;
    SDL_Rect pos_btn_oui;
    SDL_Rect pos_btn_non;
    SDL_Rect pos_msg;
    SDL_Rect pos_score;
    SDL_Rect pos_vies;
    SDL_Rect pos_niveau;
    SDL_Rect pos_barre_temps;
    
    // États
    int en_cours;
    int resultat_affiche;
    int reponse_donnee;
    int enigme_terminee;
    int menu_actif;
    
    SDL_Renderer *renderer;
} Enigme;

// Initialisation et nettoyage
void enigme_init(Enigme *e, SDL_Renderer *renderer);
void enigme_liberer(Enigme *e);

// Chargement des questions depuis fichier
int enigme_charger_questions(Enigme *e, const char *nom_fichier);

// Génération et logique
void enigme_generer_question_aleatoire(Enigme *e);
int enigme_verifier_reponse(Enigme *e, int reponse_choisie);
void enigme_reinitialiser(Enigme *e);
void enigme_reinitialiser_dejavu(Enigme *e);

// Gestion du chronomètre
void enigme_demarrer_chrono(Enigme *e);
void enigme_mettre_a_jour_chrono(Enigme *e);
int enigme_chrono_termine(Enigme *e);

// Sons
void enigme_charger_sons(Enigme *e);
void enigme_jouer_son_selection(Enigme *e);
void enigme_jouer_son_erreur(Enigme *e);
void enigme_jouer_son_succes(Enigme *e);

// Affichage
void enigme_afficher(Enigme *e);
void enigme_afficher_resultat(Enigme *e);
void enigme_afficher_barre_temps(Enigme *e);
void enigme_mettre_a_jour_affichage_score(Enigme *e);

// Gestion des événements
void enigme_gerer_clic(Enigme *e, int x, int y, int *continuer, int *en_jeu);

// Sauvegarde
void enigme_sauvegarder(Enigme *e);
void enigme_charger(Enigme *e);

#endif
