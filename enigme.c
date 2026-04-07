/*#include "enigme.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Chargement des questions depuis un fichier texte
int enigme_charger_questions(Enigme *e, const char *nom_fichier)
{
    FILE *fichier = fopen(nom_fichier, "r");
    if (fichier == NULL) {
        printf("ERREUR: Impossible d'ouvrir %s\n", nom_fichier);
        return 0;
    }
    
    e->nb_questions = 0;
    char ligne[512];
    
    while (fgets(ligne, sizeof(ligne), fichier) && e->nb_questions < MAX_QUESTIONS) {
        if (ligne[0] == '\n' || ligne[0] == '#') continue;
        
        ligne[strcspn(ligne, "\n")] = 0;
        
        char *q = strtok(ligne, ";");
        char *r = strtok(NULL, ";");
        char *n = strtok(NULL, ";");
        
        if (q && r && n) {
            strcpy(e->questions[e->nb_questions].question, q);
            e->questions[e->nb_questions].reponse = atoi(r);
            e->questions[e->nb_questions].niveau = atoi(n);
            e->questions[e->nb_questions].deja_vu = 0;
            e->nb_questions++;
        }
    }
    
    fclose(fichier);
    printf("%d questions chargees depuis %s\n", e->nb_questions, nom_fichier);
    return e->nb_questions;
}

// Chargement des sons
void enigme_charger_sons(Enigme *e)
{
    // Initialisation de SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Erreur Mix_OpenAudio: %s\n", Mix_GetError());
    }
    
    // Chargement des sons (créez ces fichiers ou utilisez des sons par défaut)
    e->son_selection = Mix_LoadWAV("selection.wav");
    if (!e->son_selection) printf("Son selection.wav non trouve\n");
    
    e->son_erreur = Mix_LoadWAV("erreur.wav");
    if (!e->son_erreur) printf("Son erreur.wav non trouve\n");
    
    e->son_succes = Mix_LoadWAV("succes.wav");
    if (!e->son_succes) printf("Son succes.wav non trouve\n");
    
    e->son_temps_ecoule = Mix_LoadWAV("temps.wav");
    if (!e->son_temps_ecoule) printf("Son temps.wav non trouve\n");
}

void enigme_jouer_son_selection(Enigme *e)
{
    if (e->son_selection) Mix_PlayChannel(-1, e->son_selection, 0);
}

void enigme_jouer_son_erreur(Enigme *e)
{
    if (e->son_erreur) Mix_PlayChannel(-1, e->son_erreur, 0);
}

void enigme_jouer_son_succes(Enigme *e)
{
    if (e->son_succes) Mix_PlayChannel(-1, e->son_succes, 0);
}

// Générer une question aléatoire
void enigme_generer_question_aleatoire(Enigme *e)
{
    int toutes_vues = 1;
    for (int i = 0; i < e->nb_questions; i++) {
        if (e->questions[i].deja_vu == 0) {
            toutes_vues = 0;
            break;
        }
    }
    
    if (toutes_vues) {
        enigme_reinitialiser_dejavu(e);
    }
    
    int index;
    do {
        index = rand() % e->nb_questions;
    } while (e->questions[index].deja_vu == 1);
    
    e->question_actuelle = index;
    e->questions[index].deja_vu = 1;
    e->reponse_donnee = 0;
    e->resultat_affiche = 0;
    e->chrono_actif = 1;
    
    SDL_Color blanc = {255, 255, 255, 255};
    SDL_Surface *temp = TTF_RenderText_Blended(e->police_question, 
                        e->questions[index].question, blanc);
    if (temp) {
        e->texte_question = SDL_CreateTextureFromSurface(e->renderer, temp);
        SDL_FreeSurface(temp);
    }
    
    enigme_demarrer_chrono(e);
}

void enigme_reinitialiser_dejavu(Enigme *e)
{
    for (int i = 0; i < e->nb_questions; i++) {
        e->questions[i].deja_vu = 0;
    }
    printf("Cycle de questions termine! Nouveau cycle.\n");
}

void enigme_demarrer_chrono(Enigme *e)
{
    e->temps_debut = SDL_GetTicks();
    e->temps_restant = TEMPS_PAR_QUESTION;
    e->chrono_actif = 1;
    e->largeur_barre_initiale = 400;
    e->barre_temps = (SDL_Rect){200, 550, e->largeur_barre_initiale, 20};
}

void enigme_mettre_a_jour_chrono(Enigme *e)
{
    if (!e->chrono_actif || e->reponse_donnee) return;
    
    Uint32 temps_ecoule = SDL_GetTicks() - e->temps_debut;
    e->temps_restant = TEMPS_PAR_QUESTION - temps_ecoule;
    
    float ratio = (float)e->temps_restant / TEMPS_PAR_QUESTION;
    if (ratio < 0) ratio = 0;
    
    e->barre_temps.w = e->largeur_barre_initiale * ratio;
    
    if (e->temps_restant <= 0 && !e->reponse_donnee) {
        e->chrono_actif = 0;
        e->reponse_donnee = 1;
        e->resultat_affiche = 1;
        e->vies--;
        enigme_jouer_son_erreur(e);  // Son d'erreur pour temps écoulé
        enigme_mettre_a_jour_affichage_score(e);
        printf("Temps ecoule! Vies restantes: %d\n", e->vies);
        
        if (e->vies <= 0) {
            e->enigme_terminee = 1;
            printf("Game Over! Score final: %d\n", e->score);
        }
    }
}

void enigme_afficher_barre_temps(Enigme *e)
{
    SDL_Rect fond_barre = {e->barre_temps.x, e->barre_temps.y, 
                           e->largeur_barre_initiale, e->barre_temps.h};
    SDL_SetRenderDrawColor(e->renderer, 80, 80, 80, 255);
    SDL_RenderFillRect(e->renderer, &fond_barre);
    
    if (e->temps_restant > TEMPS_PAR_QUESTION / 2) {
        SDL_SetRenderDrawColor(e->renderer, 0, 200, 0, 255);
    } else if (e->temps_restant > TEMPS_PAR_QUESTION / 4) {
        SDL_SetRenderDrawColor(e->renderer, 200, 200, 0, 255);
    } else {
        SDL_SetRenderDrawColor(e->renderer, 200, 0, 0, 255);
    }
    SDL_RenderFillRect(e->renderer, &e->barre_temps);
}

void enigme_mettre_a_jour_affichage_score(Enigme *e)
{
    SDL_Color blanc = {255, 255, 255, 255};
    SDL_Surface *temp;
    char buffer[50];
    
    sprintf(buffer, "Score: %d", e->score);
    temp = TTF_RenderText_Blended(e->police_score, buffer, blanc);
    if (temp) {
        e->texte_score = SDL_CreateTextureFromSurface(e->renderer, temp);
        SDL_FreeSurface(temp);
    }
    
    sprintf(buffer, "Vies: %d", e->vies);
    temp = TTF_RenderText_Blended(e->police_score, buffer, blanc);
    if (temp) {
        e->texte_vies = SDL_CreateTextureFromSurface(e->renderer, temp);
        SDL_FreeSurface(temp);
    }
    
    sprintf(buffer, "Niveau: %d", e->niveau_actuel);
    temp = TTF_RenderText_Blended(e->police_score, buffer, blanc);
    if (temp) {
        e->texte_niveau = SDL_CreateTextureFromSurface(e->renderer, temp);
        SDL_FreeSurface(temp);
    }
}

// Initialisation
void enigme_init(Enigme *e, SDL_Renderer *renderer)
{
    SDL_Color blanc = {255, 255, 255, 255};
    SDL_Surface *temp;
    
    srand(time(NULL));
    
    e->renderer = renderer;
    e->en_cours = 1;
    e->resultat_affiche = 0;
    e->menu_actif = 0;
    e->reponse_donnee = 0;
    e->enigme_terminee = 0;
    e->score = 0;
    e->vies = 3;
    e->niveau_actuel = 1;
    e->nb_questions = 0;
    e->question_actuelle = -1;
    
    // Chargement des polices
    e->police_question = TTF_OpenFont("arial.ttf", 35);
    if (!e->police_question) printf("ERREUR: arial.ttf non trouve\n");
    
    e->police_reponse = TTF_OpenFont("arial.ttf", 50);
    e->police_score = TTF_OpenFont("arial.ttf", 25);
    
    // Chargement des sons
    enigme_charger_sons(e);
    
    // Chargement du background avec verification
    e->fond = IMG_LoadTexture(renderer, "im_fond.png");
    if (!e->fond) {
        printf("ERREUR: Impossible de charger enigmealf.png\n");
        printf("IMG_GetError: %s\n", IMG_GetError());
    } else {
        printf("Background charge avec succes!\n");
    }
    
    // Chargement des autres images
    e->menu_barre = IMG_LoadTexture(renderer, "barre.png");
    e->btn_reprendre = IMG_LoadTexture(renderer, "Resume.png");
    e->btn_sauvegarder = IMG_LoadTexture(renderer, "SAVe.png");
    e->btn_quitter = IMG_LoadTexture(renderer, "EXIT.png");
    
    // Création des textes
    temp = TTF_RenderText_Blended(e->police_reponse, "OUI", blanc);
    e->btn_oui = SDL_CreateTextureFromSurface(renderer, temp);
    SDL_FreeSurface(temp);
    
    temp = TTF_RenderText_Blended(e->police_reponse, "NON", blanc);
    e->btn_non = SDL_CreateTextureFromSurface(renderer, temp);
    SDL_FreeSurface(temp);
    
    temp = TTF_RenderText_Blended(e->police_reponse, "BRAVO!", blanc);
    e->msg_reussi = SDL_CreateTextureFromSurface(renderer, temp);
    SDL_FreeSurface(temp);
    
    temp = TTF_RenderText_Blended(e->police_reponse, "RATEP!", blanc);
    e->msg_echoue = SDL_CreateTextureFromSurface(renderer, temp);
    SDL_FreeSurface(temp);
    
    // Positions
    e->pos_fond = (SDL_Rect){0, 0, 800, 600};
    e->pos_question = (SDL_Rect){50, 180, 700, 120};
    e->pos_btn_oui = (SDL_Rect){200, 450, 150, 70};
    e->pos_btn_non = (SDL_Rect){450, 450, 150, 70};
    e->pos_msg = (SDL_Rect){300, 280, 200, 80};
    e->pos_score = (SDL_Rect){20, 20, 150, 30};
    e->pos_vies = (SDL_Rect){20, 55, 150, 30};
    e->pos_niveau = (SDL_Rect){20, 90, 150, 30};
    
    e->pos_menu = (SDL_Rect){250, 150, 300, 250};
    e->pos_btn_reprendre = (SDL_Rect){280, 190, 100, 60};
    e->pos_btn_sauvegarder = (SDL_Rect){280, 260, 100, 60};
    e->pos_btn_quitter = (SDL_Rect){280, 330, 100, 60};
    
    if (enigme_charger_questions(e, "question.txt") > 0) {
        enigme_generer_question_aleatoire(e);
    }
    
    enigme_mettre_a_jour_affichage_score(e);
}

// Vérifier la réponse
int enigme_verifier_reponse(Enigme *e, int reponse_choisie)
{
    if (e->reponse_donnee) return 0;
    
    // Jouer le son de selection
    enigme_jouer_son_selection(e);
    
    e->chrono_actif = 0;
    e->reponse_donnee = 1;
    e->resultat_affiche = 1;
    
    int juste = (reponse_choisie == e->questions[e->question_actuelle].reponse);
    
    if (juste) {
        e->score += 10 * e->niveau_actuel;
        enigme_jouer_son_succes(e);  // Son de succes
        printf("Bonne reponse! Score: %d\n", e->score);
        
        if (e->score > 0 && e->score % 50 == 0) {
            e->niveau_actuel++;
            printf("Niveau superieur! Niveau: %d\n", e->niveau_actuel);
        }
    } else {
        e->vies--;
        enigme_jouer_son_erreur(e);  // Son d'erreur
        printf("Mauvaise reponse! Vies: %d\n", e->vies);
    }
    
    enigme_mettre_a_jour_affichage_score(e);
    
    if (e->vies <= 0) {
        e->enigme_terminee = 1;
        printf("PARTIE TERMINEE! Score final: %d\n", e->score);
    }
    
    return juste;
}

// Afficher l'énigme
void enigme_afficher(Enigme *e)
{
    enigme_mettre_a_jour_chrono(e);
    
    // Effacer l'écran
    SDL_SetRenderDrawColor(e->renderer, 0, 0, 0, 255);
    SDL_RenderClear(e->renderer);
    
    // Afficher le background en plein ecran
    if (e->fond) {
        SDL_RenderCopy(e->renderer, e->fond, NULL, &e->pos_fond);
    } else {
        // Fond de secours si l'image n'est pas chargee
        SDL_SetRenderDrawColor(e->renderer, 40, 40, 80, 255);
        SDL_RenderFillRect(e->renderer, &e->pos_fond);
    }
    
    // Afficher le score, vies, niveau
    if (e->texte_score) {
        SDL_RenderCopy(e->renderer, e->texte_score, NULL, &e->pos_score);
    }
    if (e->texte_vies) {
        SDL_RenderCopy(e->renderer, e->texte_vies, NULL, &e->pos_vies);
    }
    if (e->texte_niveau) {
        SDL_RenderCopy(e->renderer, e->texte_niveau, NULL, &e->pos_niveau);
    }
    
    // Afficher la barre de temps
    enigme_afficher_barre_temps(e);
    
    // Afficher la question
    if (e->texte_question && !e->enigme_terminee) {
        SDL_RenderCopy(e->renderer, e->texte_question, NULL, &e->pos_question);
    }
    
    // Afficher les boutons
    if (!e->reponse_donnee && !e->enigme_terminee && !e->menu_actif) {
        SDL_RenderCopy(e->renderer, e->btn_oui, NULL, &e->pos_btn_oui);
        SDL_RenderCopy(e->renderer, e->btn_non, NULL, &e->pos_btn_non);
    }
    
    // Afficher le resultat
    if (e->resultat_affiche && !e->menu_actif) {
        if (e->vies > 0 && !e->enigme_terminee) {
            static Uint32 temps_affichage = 0;
            if (temps_affichage == 0) temps_affichage = SDL_GetTicks();
            
            if (SDL_GetTicks() - temps_affichage > 2000) {
                temps_affichage = 0;
                enigme_reinitialiser(e);
            }
        }
    }
    
    // Afficher le menu
    if (e->menu_actif) {
        enigme_afficher_menu(e);
    }
    
    // Afficher Game Over
    if (e->enigme_terminee) {
        SDL_Color blanc = {255, 255, 255, 255};
        SDL_Surface *temp = TTF_RenderText_Blended(e->police_reponse, "GAME OVER", blanc);
        if (temp) {
            SDL_Texture *gameover = SDL_CreateTextureFromSurface(e->renderer, temp);
            SDL_Rect pos_gameover = {250, 250, 300, 80};
            SDL_RenderCopy(e->renderer, gameover, NULL, &pos_gameover);
            SDL_DestroyTexture(gameover);
            SDL_FreeSurface(temp);
        }
    }
}

void enigme_reinitialiser(Enigme *e)
{
    if (e->vies > 0 && !e->enigme_terminee) {
        e->resultat_affiche = 0;
        e->reponse_donnee = 0;
        enigme_generer_question_aleatoire(e);
    }
}

void enigme_afficher_menu(Enigme *e)
{
    if (e->menu_barre) SDL_RenderCopy(e->renderer, e->menu_barre, NULL, &e->pos_menu);
    if (e->btn_reprendre) SDL_RenderCopy(e->renderer, e->btn_reprendre, NULL, &e->pos_btn_reprendre);
    if (e->btn_sauvegarder) SDL_RenderCopy(e->renderer, e->btn_sauvegarder, NULL, &e->pos_btn_sauvegarder);
    if (e->btn_quitter) SDL_RenderCopy(e->renderer, e->btn_quitter, NULL, &e->pos_btn_quitter);
}

void enigme_gerer_clic(Enigme *e, int x, int y, int *continuer, int *en_jeu)
{
    if (e->menu_actif) {
        if (x >= e->pos_btn_reprendre.x && x <= e->pos_btn_reprendre.x + 100 &&
            y >= e->pos_btn_reprendre.y && y <= e->pos_btn_reprendre.y + 60) {
            e->menu_actif = 0;
            enigme_jouer_son_selection(e);
        }
        else if (x >= e->pos_btn_sauvegarder.x && x <= e->pos_btn_sauvegarder.x + 100 &&
                 y >= e->pos_btn_sauvegarder.y && y <= e->pos_btn_sauvegarder.y + 60) {
            enigme_sauvegarder(e);
            e->menu_actif = 0;
            enigme_jouer_son_selection(e);
        }
        else if (x >= e->pos_btn_quitter.x && x <= e->pos_btn_quitter.x + 100 &&
                 y >= e->pos_btn_quitter.y && y <= e->pos_btn_quitter.y + 60) {
            *continuer = 0;
        }
        return;
    }
    
    if (!e->reponse_donnee && !e->enigme_terminee) {
        if (x >= e->pos_btn_oui.x && x <= e->pos_btn_oui.x + 150 &&
            y >= e->pos_btn_oui.y && y <= e->pos_btn_oui.y + 70) {
            enigme_verifier_reponse(e, 1);
        }
        else if (x >= e->pos_btn_non.x && x <= e->pos_btn_non.x + 150 &&
                 y >= e->pos_btn_non.y && y <= e->pos_btn_non.y + 70) {
            enigme_verifier_reponse(e, 0);
        }
    }
}

void enigme_sauvegarder(Enigme *e)
{
    FILE *f = fopen("enigme_save.txt", "w");
    if (f) {
        fprintf(f, "%d\n", e->score);
        fprintf(f, "%d\n", e->vies);
        fprintf(f, "%d\n", e->niveau_actuel);
        fprintf(f, "%d\n", e->question_actuelle);
        fprintf(f, "%d\n", e->nb_questions);
        
        for (int i = 0; i < e->nb_questions; i++) {
            fprintf(f, "%d\n", e->questions[i].deja_vu);
        }
        
        fclose(f);
        printf("Partie sauvegardee!\n");
    }
}

void enigme_charger(Enigme *e)
{
    FILE *f = fopen("enigme_save.txt", "r");
    if (f) {
        fscanf(f, "%d", &e->score);
        fscanf(f, "%d", &e->vies);
        fscanf(f, "%d", &e->niveau_actuel);
        fscanf(f, "%d", &e->question_actuelle);
        fscanf(f, "%d", &e->nb_questions);
        
        for (int i = 0; i < e->nb_questions; i++) {
            fscanf(f, "%d", &e->questions[i].deja_vu);
        }
        
        fclose(f);
        enigme_mettre_a_jour_affichage_score(e);
        printf("Partie chargee! Score: %d, Vies: %d\n", e->score, e->vies);
    }
}

void enigme_liberer(Enigme *e)
{
    // Liberer les textures
    if (e->fond) SDL_DestroyTexture(e->fond);
    if (e->texte_question) SDL_DestroyTexture(e->texte_question);
    if (e->btn_oui) SDL_DestroyTexture(e->btn_oui);
    if (e->btn_non) SDL_DestroyTexture(e->btn_non);
    if (e->msg_reussi) SDL_DestroyTexture(e->msg_reussi);
    if (e->msg_echoue) SDL_DestroyTexture(e->msg_echoue);
    if (e->menu_barre) SDL_DestroyTexture(e->menu_barre);
    if (e->btn_reprendre) SDL_DestroyTexture(e->btn_reprendre);
    if (e->btn_sauvegarder) SDL_DestroyTexture(e->btn_sauvegarder);
    if (e->btn_quitter) SDL_DestroyTexture(e->btn_quitter);
    if (e->texte_score) SDL_DestroyTexture(e->texte_score);
    if (e->texte_vies) SDL_DestroyTexture(e->texte_vies);
    if (e->texte_niveau) SDL_DestroyTexture(e->texte_niveau);
    
    // Liberer les polices
    if (e->police_question) TTF_CloseFont(e->police_question);
    if (e->police_reponse) TTF_CloseFont(e->police_reponse);
    if (e->police_score) TTF_CloseFont(e->police_score);
    
    // Liberer les sons
    if (e->son_selection) Mix_FreeChunk(e->son_selection);
    if (e->son_erreur) Mix_FreeChunk(e->son_erreur);
    if (e->son_succes) Mix_FreeChunk(e->son_succes);
    if (e->son_temps_ecoule) Mix_FreeChunk(e->son_temps_ecoule);
    
    Mix_CloseAudio();
}*/
#include "enigme.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Chargement des questions depuis un fichier texte
int enigme_charger_questions(Enigme *e, const char *nom_fichier)
{
    FILE *fichier = fopen(nom_fichier, "r");
    if (fichier == NULL) {
        printf("ERREUR: Impossible d'ouvrir %s\n", nom_fichier);
        return 0;
    }
    
    e->nb_questions = 0;
    char ligne[512];
    
    while (fgets(ligne, sizeof(ligne), fichier) && e->nb_questions < MAX_QUESTIONS) {
        if (ligne[0] == '\n' || ligne[0] == '#') continue;
        
        ligne[strcspn(ligne, "\n")] = 0;
        
        char *q = strtok(ligne, ";");
        char *r = strtok(NULL, ";");
        char *n = strtok(NULL, ";");
        
        if (q && r && n) {
            strcpy(e->questions[e->nb_questions].question, q);
            e->questions[e->nb_questions].reponse = atoi(r);
            e->questions[e->nb_questions].niveau = atoi(n);
            e->questions[e->nb_questions].deja_vu = 0;
            e->nb_questions++;
        }
    }
    
    fclose(fichier);
    printf("%d questions chargees depuis %s\n", e->nb_questions, nom_fichier);
    return e->nb_questions;
}

// Chargement des sons
void enigme_charger_sons(Enigme *e)
{
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    
    e->son_selection = Mix_LoadWAV("selection.wav");
    if (!e->son_selection) printf("Son selection.wav non trouve\n");
    
    e->son_erreur = Mix_LoadWAV("erreur.wav");
    if (!e->son_erreur) printf("Son erreur.wav non trouve\n");
    
    e->son_succes = Mix_LoadWAV("succes.wav");
    if (!e->son_succes) printf("Son succes.wav non trouve\n");
    
    e->son_temps_ecoule = Mix_LoadWAV("temps.wav");
    if (!e->son_temps_ecoule) printf("Son temps.wav non trouve\n");
}

void enigme_jouer_son_selection(Enigme *e)
{
    if (e->son_selection) Mix_PlayChannel(-1, e->son_selection, 0);
}

void enigme_jouer_son_erreur(Enigme *e)
{
    if (e->son_erreur) Mix_PlayChannel(-1, e->son_erreur, 0);
}

void enigme_jouer_son_succes(Enigme *e)
{
    if (e->son_succes) Mix_PlayChannel(-1, e->son_succes, 0);
}

// Générer une question aléatoire
void enigme_generer_question_aleatoire(Enigme *e)
{
    int toutes_vues = 1;
    for (int i = 0; i < e->nb_questions; i++) {
        if (e->questions[i].deja_vu == 0) {
            toutes_vues = 0;
            break;
        }
    }
    
    if (toutes_vues) {
        enigme_reinitialiser_dejavu(e);
    }
    
    int index;
    do {
        index = rand() % e->nb_questions;
    } while (e->questions[index].deja_vu == 1);
    
    e->question_actuelle = index;
    e->questions[index].deja_vu = 1;
    e->reponse_donnee = 0;
    e->resultat_affiche = 0;
    e->chrono_actif = 1;
    
    SDL_Color blanc = {255, 255, 255, 255};
    SDL_Surface *temp = TTF_RenderText_Blended(e->police_question, 
                        e->questions[index].question, blanc);
    if (temp) {
        e->texte_question = SDL_CreateTextureFromSurface(e->renderer, temp);
        SDL_FreeSurface(temp);
    }
    
    enigme_demarrer_chrono(e);
}

void enigme_reinitialiser_dejavu(Enigme *e)
{
    for (int i = 0; i < e->nb_questions; i++) {
        e->questions[i].deja_vu = 0;
    }
    printf("Cycle de questions termine! Nouveau cycle.\n");
}

void enigme_demarrer_chrono(Enigme *e)
{
    e->temps_debut = SDL_GetTicks();
    e->temps_restant = TEMPS_PAR_QUESTION;
    e->chrono_actif = 1;
    e->largeur_barre_initiale = 400;
    e->barre_temps = (SDL_Rect){200, 550, e->largeur_barre_initiale, 20};
}

void enigme_mettre_a_jour_chrono(Enigme *e)
{
    if (!e->chrono_actif || e->reponse_donnee) return;
    
    Uint32 temps_ecoule = SDL_GetTicks() - e->temps_debut;
    e->temps_restant = TEMPS_PAR_QUESTION - temps_ecoule;
    
    float ratio = (float)e->temps_restant / TEMPS_PAR_QUESTION;
    if (ratio < 0) ratio = 0;
    
    e->barre_temps.w = e->largeur_barre_initiale * ratio;
    
    if (e->temps_restant <= 0 && !e->reponse_donnee) {
        e->chrono_actif = 0;
        e->reponse_donnee = 1;
        e->resultat_affiche = 1;
        e->vies--;
        enigme_jouer_son_erreur(e);
        enigme_mettre_a_jour_affichage_score(e);
        printf("Temps ecoule! Vies restantes: %d\n", e->vies);
        
        if (e->vies <= 0) {
            e->enigme_terminee = 1;
            printf("Game Over! Score final: %d\n", e->score);
        }
    }
}

void enigme_afficher_barre_temps(Enigme *e)
{
    SDL_Rect fond_barre = {e->barre_temps.x, e->barre_temps.y, 
                           e->largeur_barre_initiale, e->barre_temps.h};
    SDL_SetRenderDrawColor(e->renderer, 80, 80, 80, 255);
    SDL_RenderFillRect(e->renderer, &fond_barre);
    
    if (e->temps_restant > TEMPS_PAR_QUESTION / 2) {
        SDL_SetRenderDrawColor(e->renderer, 0, 200, 0, 255);
    } else if (e->temps_restant > TEMPS_PAR_QUESTION / 4) {
        SDL_SetRenderDrawColor(e->renderer, 200, 200, 0, 255);
    } else {
        SDL_SetRenderDrawColor(e->renderer, 200, 0, 0, 255);
    }
    SDL_RenderFillRect(e->renderer, &e->barre_temps);
}

void enigme_mettre_a_jour_affichage_score(Enigme *e)
{
    SDL_Color blanc = {255, 255, 255, 255};
    SDL_Surface *temp;
    char buffer[50];
    
    sprintf(buffer, "Score: %d", e->score);
    temp = TTF_RenderText_Blended(e->police_score, buffer, blanc);
    if (temp) {
        e->texte_score = SDL_CreateTextureFromSurface(e->renderer, temp);
        SDL_FreeSurface(temp);
    }
    
    sprintf(buffer, "Vies: %d", e->vies);
    temp = TTF_RenderText_Blended(e->police_score, buffer, blanc);
    if (temp) {
        e->texte_vies = SDL_CreateTextureFromSurface(e->renderer, temp);
        SDL_FreeSurface(temp);
    }
    
    sprintf(buffer, "Niveau: %d", e->niveau_actuel);
    temp = TTF_RenderText_Blended(e->police_score, buffer, blanc);
    if (temp) {
        e->texte_niveau = SDL_CreateTextureFromSurface(e->renderer, temp);
        SDL_FreeSurface(temp);
    }
}

// Initialisation
void enigme_init(Enigme *e, SDL_Renderer *renderer)
{
    SDL_Color blanc = {255, 255, 255, 255};
    SDL_Surface *temp;
    
    srand(time(NULL));
    
    e->renderer = renderer;
    e->en_cours = 1;
    e->resultat_affiche = 0;
    e->reponse_donnee = 0;
    e->enigme_terminee = 0;
    e->menu_actif = 0;
    e->score = 0;
    e->vies = 3;
    e->niveau_actuel = 1;
    e->nb_questions = 0;
    e->question_actuelle = -1;
    
    // Chargement des polices
    e->police_question = TTF_OpenFont("arial.ttf", 35);
    if (!e->police_question) printf("ERREUR: arial.ttf non trouve\n");
    
    e->police_reponse = TTF_OpenFont("arial.ttf", 50);
    e->police_score = TTF_OpenFont("arial.ttf", 25);
    
    // Chargement des sons
    enigme_charger_sons(e);
    
    // Chargement du background
    e->fond = IMG_LoadTexture(renderer, "im_fond.png");
    if (!e->fond) {
        printf("ERREUR: Impossible de charger im_fond.png\n");
    } else {
        printf("Background charge avec succes!\n");
    }
    
    // Création des textes OUI/NON
    temp = TTF_RenderText_Blended(e->police_reponse, "OUI", blanc);
    e->btn_oui = SDL_CreateTextureFromSurface(renderer, temp);
    SDL_FreeSurface(temp);
    
    temp = TTF_RenderText_Blended(e->police_reponse, "NON", blanc);
    e->btn_non = SDL_CreateTextureFromSurface(renderer, temp);
    SDL_FreeSurface(temp);
    
    temp = TTF_RenderText_Blended(e->police_reponse, "BRAVO!", blanc);
    e->msg_reussi = SDL_CreateTextureFromSurface(renderer, temp);
    SDL_FreeSurface(temp);
    
    temp = TTF_RenderText_Blended(e->police_reponse, "RATEP!", blanc);
    e->msg_echoue = SDL_CreateTextureFromSurface(renderer, temp);
    SDL_FreeSurface(temp);
    
    // Positions
    e->pos_fond = (SDL_Rect){0, 0, 800, 600};
    e->pos_question = (SDL_Rect){50, 180, 700, 120};
    e->pos_btn_oui = (SDL_Rect){200, 450, 150, 70};
    e->pos_btn_non = (SDL_Rect){450, 450, 150, 70};
    e->pos_msg = (SDL_Rect){300, 280, 200, 80};
    e->pos_score = (SDL_Rect){20, 20, 150, 30};
    e->pos_vies = (SDL_Rect){20, 55, 150, 30};
    e->pos_niveau = (SDL_Rect){20, 90, 150, 30};
    
    if (enigme_charger_questions(e, "question.txt") > 0) {
        enigme_generer_question_aleatoire(e);
    }
    
    enigme_mettre_a_jour_affichage_score(e);
}

// Vérifier la réponse
int enigme_verifier_reponse(Enigme *e, int reponse_choisie)
{
    if (e->reponse_donnee) return 0;
    
    enigme_jouer_son_selection(e);
    
    e->chrono_actif = 0;
    e->reponse_donnee = 1;
    e->resultat_affiche = 1;
    
    int juste = (reponse_choisie == e->questions[e->question_actuelle].reponse);
    
    if (juste) {
        e->score += 10 * e->niveau_actuel;
        enigme_jouer_son_succes(e);
        printf("Bonne reponse! Score: %d\n", e->score);
        
        if (e->score > 0 && e->score % 50 == 0) {
            e->niveau_actuel++;
            printf("Niveau superieur! Niveau: %d\n", e->niveau_actuel);
        }
    } else {
        e->vies--;
        enigme_jouer_son_erreur(e);
        printf("Mauvaise reponse! Vies: %d\n", e->vies);
    }
    
    enigme_mettre_a_jour_affichage_score(e);
    
    if (e->vies <= 0) {
        e->enigme_terminee = 1;
        printf("PARTIE TERMINEE! Score final: %d\n", e->score);
    }
    
    return juste;
}

// Afficher l'énigme
void enigme_afficher(Enigme *e)
{
    enigme_mettre_a_jour_chrono(e);
    
    // Effacer l'écran
    SDL_SetRenderDrawColor(e->renderer, 0, 0, 0, 255);
    SDL_RenderClear(e->renderer);
    
    // Afficher le background
    if (e->fond) {
        SDL_RenderCopy(e->renderer, e->fond, NULL, &e->pos_fond);
    } else {
        SDL_SetRenderDrawColor(e->renderer, 40, 40, 80, 255);
        SDL_RenderFillRect(e->renderer, &e->pos_fond);
    }
    
    // Afficher le score, vies, niveau
    if (e->texte_score) {
        SDL_RenderCopy(e->renderer, e->texte_score, NULL, &e->pos_score);
    }
    if (e->texte_vies) {
        SDL_RenderCopy(e->renderer, e->texte_vies, NULL, &e->pos_vies);
    }
    if (e->texte_niveau) {
        SDL_RenderCopy(e->renderer, e->texte_niveau, NULL, &e->pos_niveau);
    }
    
    // Afficher la barre de temps
    enigme_afficher_barre_temps(e);
    
    // Afficher la question
    if (e->texte_question && !e->enigme_terminee) {
        SDL_RenderCopy(e->renderer, e->texte_question, NULL, &e->pos_question);
    }
    
    // Afficher les boutons OUI/NON
    if (!e->reponse_donnee && !e->enigme_terminee) {
        SDL_RenderCopy(e->renderer, e->btn_oui, NULL, &e->pos_btn_oui);
        SDL_RenderCopy(e->renderer, e->btn_non, NULL, &e->pos_btn_non);
    }
    
    // Afficher le resultat et passer à la question suivante
    if (e->resultat_affiche && !e->enigme_terminee) {
        static Uint32 temps_affichage = 0;
        if (temps_affichage == 0) temps_affichage = SDL_GetTicks();
        
        // Afficher le message de résultat
        if (e->vies > 0) {
            if (e->score > 0 && (e->score % 10 == 0 || e->reponse_donnee)) {
                // Le message s'affiche
            }
        }
        
        if (SDL_GetTicks() - temps_affichage > 2000) {
            temps_affichage = 0;
            enigme_reinitialiser(e);
        }
    }
    
    // Afficher Game Over
    if (e->enigme_terminee) {
        SDL_Color blanc = {255, 255, 255, 255};
        SDL_Surface *temp = TTF_RenderText_Blended(e->police_reponse, "GAME OVER", blanc);
        if (temp) {
            SDL_Texture *gameover = SDL_CreateTextureFromSurface(e->renderer, temp);
            SDL_Rect pos_gameover = {250, 250, 300, 80};
            SDL_RenderCopy(e->renderer, gameover, NULL, &pos_gameover);
            SDL_DestroyTexture(gameover);
            SDL_FreeSurface(temp);
        }
    }
}

void enigme_reinitialiser(Enigme *e)
{
    if (e->vies > 0 && !e->enigme_terminee) {
        e->resultat_affiche = 0;
        e->reponse_donnee = 0;
        enigme_generer_question_aleatoire(e);
    }
}

void enigme_gerer_clic(Enigme *e, int x, int y, int *continuer, int *en_jeu)
{
    if (!e->reponse_donnee && !e->enigme_terminee) {
        if (x >= e->pos_btn_oui.x && x <= e->pos_btn_oui.x + 150 &&
            y >= e->pos_btn_oui.y && y <= e->pos_btn_oui.y + 70) {
            enigme_verifier_reponse(e, 1);
        }
        else if (x >= e->pos_btn_non.x && x <= e->pos_btn_non.x + 150 &&
                 y >= e->pos_btn_non.y && y <= e->pos_btn_non.y + 70) {
            enigme_verifier_reponse(e, 0);
        }
    }
}

void enigme_sauvegarder(Enigme *e)
{
    FILE *f = fopen("enigme_save.txt", "w");
    if (f) {
        fprintf(f, "%d\n", e->score);
        fprintf(f, "%d\n", e->vies);
        fprintf(f, "%d\n", e->niveau_actuel);
        fprintf(f, "%d\n", e->question_actuelle);
        fprintf(f, "%d\n", e->nb_questions);
        
        for (int i = 0; i < e->nb_questions; i++) {
            fprintf(f, "%d\n", e->questions[i].deja_vu);
        }
        
        fclose(f);
        printf("Partie sauvegardee!\n");
    }
}

void enigme_charger(Enigme *e)
{
    FILE *f = fopen("enigme_save.txt", "r");
    if (f) {
        fscanf(f, "%d", &e->score);
        fscanf(f, "%d", &e->vies);
        fscanf(f, "%d", &e->niveau_actuel);
        fscanf(f, "%d", &e->question_actuelle);
        fscanf(f, "%d", &e->nb_questions);
        
        for (int i = 0; i < e->nb_questions; i++) {
            fscanf(f, "%d", &e->questions[i].deja_vu);
        }
        
        fclose(f);
        enigme_mettre_a_jour_affichage_score(e);
        printf("Partie chargee! Score: %d, Vies: %d\n", e->score, e->vies);
    }
}

void enigme_liberer(Enigme *e)
{
    if (e->fond) SDL_DestroyTexture(e->fond);
    if (e->texte_question) SDL_DestroyTexture(e->texte_question);
    if (e->btn_oui) SDL_DestroyTexture(e->btn_oui);
    if (e->btn_non) SDL_DestroyTexture(e->btn_non);
    if (e->msg_reussi) SDL_DestroyTexture(e->msg_reussi);
    if (e->msg_echoue) SDL_DestroyTexture(e->msg_echoue);
    if (e->texte_score) SDL_DestroyTexture(e->texte_score);
    if (e->texte_vies) SDL_DestroyTexture(e->texte_vies);
    if (e->texte_niveau) SDL_DestroyTexture(e->texte_niveau);
    
    if (e->police_question) TTF_CloseFont(e->police_question);
    if (e->police_reponse) TTF_CloseFont(e->police_reponse);
    if (e->police_score) TTF_CloseFont(e->police_score);
    
    if (e->son_selection) Mix_FreeChunk(e->son_selection);
    if (e->son_erreur) Mix_FreeChunk(e->son_erreur);
    if (e->son_succes) Mix_FreeChunk(e->son_succes);
    if (e->son_temps_ecoule) Mix_FreeChunk(e->son_temps_ecoule);
    
    Mix_CloseAudio();
}
