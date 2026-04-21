#ifndef ENIGME_H_INCLUDED
#define ENIGME_H_INCLUDED

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#define MAX_QUESTIONS 100
#define TEMPS_PAR_QUESTION 10000

#define QUESTION_ZONE_X 32
#define QUESTION_ZONE_Y 155
#define QUESTION_ZONE_W 736
#define QUESTION_ZONE_H 158

#define HOURGLASS_SPRITE_FILE "hourglass_sprites.png"
#define HOURGLASS_SHEET_COLS 6
#define HOURGLASS_SHEET_ROWS 1
#define HOURGLASS_FRAME_COUNT 0
#define HOURGLASS_SCREEN_X 6
#define HOURGLASS_SCREEN_Y 4
#define HOURGLASS_DRAW_MAX_W 560
#define HOURGLASS_DRAW_MAX_H 520
#define HUD_TEXT_X 582
#define HOURGLASS_TIMER_GAP 12
#define HOURGLASS_SHEET_RIGHT_TO_LEFT 0
#define HOURGLASS_ANIM_USE_LOOP 0
#define HOURGLASS_ANIM_FRAME_MS 120
#define HOURGLASS_SMOOTH_BLEND 1

typedef struct {
    char question[256];
    char option_a[256];
    char option_b[256];
    int bonne_reponse; /* 1 = option A, 2 = option B */
    int id_question;
} Question;

typedef struct
{
    Question questions[MAX_QUESTIONS];
    int nb_questions;
    int question_actuelle;
    
    Uint32 temps_debut;
    int temps_restant;
    int chrono_actif;
    SDL_Texture *tex_hourglass;
    int hourglass_frame_w;
    int hourglass_frame_h;
    int hourglass_nb_frames;
    int hourglass_sheet_cols;
    SDL_Rect pos_hourglass;
    
    int score;
    int vies;
    int niveau_actuel;
    
    Mix_Chunk *son_selection;
    Mix_Chunk *son_erreur;
    Mix_Chunk *son_succes;
    Mix_Chunk *son_temps_ecoule;
    
    TTF_Font *police_question;
    TTF_Font *police_reponse;
    TTF_Font *police_score;
    TTF_Font *police_chrono;
    
    SDL_Texture *fond;
    SDL_Texture *texte_question;
    SDL_Texture *btn_oui;
    SDL_Texture *btn_non;
    SDL_Texture *texte_option_a;
    SDL_Texture *texte_option_b;
    SDL_Texture *msg_reussi;
    SDL_Texture *msg_echoue;
    SDL_Texture *texte_score;
    SDL_Texture *texte_vies;
    SDL_Texture *texte_niveau;
    SDL_Texture *tex_timer_sablier;
    SDL_Texture *tex_heart;
    
    SDL_Rect pos_fond;
    SDL_Rect pos_question;
    SDL_Rect pos_btn_oui;
    SDL_Rect pos_btn_non;
    SDL_Rect pos_option_a;
    SDL_Rect pos_option_b;
    SDL_Rect pos_msg;
    SDL_Rect pos_score;
    SDL_Rect pos_vies;
    SDL_Rect pos_niveau;
    SDL_Rect pos_timer_sablier;
    
    int timer_sablier_dsec_cache;
    
    int en_cours;
    int resultat_affiche;
    int reponse_donnee;
    int enigme_terminee;
    int menu_actif;
    
    SDL_Renderer *renderer;

    int quiz_index;
    int quiz_bonnes;
    Uint32 feedback_debut_ms;
    int feedback_correct;
} Enigme;

void enigme_init(Enigme *e, SDL_Renderer *renderer);
void enigme_liberer(Enigme *e);

int enigme_charger_questions(Enigme *e, const char *nom_fichier);

void enigme_generer_question_aleatoire(Enigme *e);
void enigme_melanger_questions(Enigme *e);
void enigme_preparer_question_courante(Enigme *e);
int enigme_verifier_reponse(Enigme *e, int reponse_choisie);
void enigme_reinitialiser(Enigme *e);

void enigme_mettre_a_jour_chrono(Enigme *e);
int enigme_chrono_termine(Enigme *e);

void enigme_charger_sons(Enigme *e);

void enigme_afficher(Enigme *e);
void enigme_afficher_resultat(Enigme *e);
void enigme_afficher_chrono_hourglass(Enigme *e);
void enigme_mettre_a_jour_affichage_score(Enigme *e);

void enigme_gerer_clic(Enigme *e, int x, int y, int *continuer, int *en_jeu);

void enigme_sauvegarder(Enigme *e);
void enigme_charger(Enigme *e);

#endif
