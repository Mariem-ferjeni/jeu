#include "enigme.h"
#include <SDL2/SDL_filesystem.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static SDL_Texture *enigme_load_texture_data(SDL_Renderer *r, const char *filename)
{
    SDL_Texture *t = NULL;
    char *base = SDL_GetBasePath();
    if (base) {
        char path[1024];
        snprintf(path, sizeof(path), "%s%s", base, filename);
        t = IMG_LoadTexture(r, path);
        if (t)
            printf("Chargement texture: %s\n", path);
        SDL_free(base);
    }
    if (!t) {
        t = IMG_LoadTexture(r, filename);
        if (t)
            printf("Chargement texture (cwd): %s\n", filename);
    }
    return t;
}

static void enigme_fit_rect_centered(int zone_x, int zone_y, int zone_w, int zone_h,
                                     int tex_w, int tex_h, SDL_Rect *out)
{
    if (!out) return;
    if (tex_w <= 0 || tex_h <= 0) {
        out->x = zone_x;
        out->y = zone_y;
        out->w = zone_w;
        out->h = zone_h;
        return;
    }
    float sx = (float)zone_w / (float)tex_w;
    float sy = (float)zone_h / (float)tex_h;
    float s = sx < sy ? sx : sy;
    int dw = (int)(tex_w * s);
    int dh = (int)(tex_h * s);
    if (dw < 1) dw = 1;
    if (dh < 1) dh = 1;
    out->x = zone_x + (zone_w - dw) / 2;
    out->y = zone_y + (zone_h - dh) / 2;
    out->w = dw;
    out->h = dh;
}

static void enigme_fit_rect_top_left(int zone_x, int zone_y, int zone_w, int zone_h,
                                     int tex_w, int tex_h, SDL_Rect *out)
{
    if (!out) return;
    if (tex_w <= 0 || tex_h <= 0) {
        out->x = zone_x;
        out->y = zone_y;
        out->w = zone_w;
        out->h = zone_h;
        return;
    }
    float sx = (float)zone_w / (float)tex_w;
    float sy = (float)zone_h / (float)tex_h;
    float s = sx < sy ? sx : sy;
    int dw = (int)(tex_w * s);
    int dh = (int)(tex_h * s);
    if (dw < 1) dw = 1;
    if (dh < 1) dh = 1;
    out->x = zone_x;
    out->y = zone_y;
    out->w = dw;
    out->h = dh;
}

int enigme_charger_questions(Enigme *e, const char *nom_fichier)
{
    FILE *fichier = fopen(nom_fichier, "r");
    if (fichier == NULL) {
        printf("ERREUR: Impossible d'ouvrir %s\n", nom_fichier);
        return 0;
    }
    
    e->nb_questions = 0;
    char ligne[768];
    
    while (fgets(ligne, sizeof(ligne), fichier) && e->nb_questions < MAX_QUESTIONS) {
        if (ligne[0] == '\n' || ligne[0] == '#') continue;
        
        ligne[strcspn(ligne, "\r\n")] = 0;
        
        Question *q = &e->questions[e->nb_questions];
        int id = 0;
        int br = 0;
        if (sscanf(ligne, "%255[^;];%255[^;];%255[^;];%d;%d",
                    q->question, q->option_a, q->option_b, &br, &id) >= 4) {
            q->bonne_reponse = br;
            q->id_question = id;
            e->nb_questions++;
        }
    }
    
    fclose(fichier);
    printf("%d questions chargees depuis %s\n", e->nb_questions, nom_fichier);
    return e->nb_questions;
}

void enigme_melanger_questions(Enigme *e)
{
    if (e->nb_questions < 2)
        return;
    for (int i = e->nb_questions - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Question t = e->questions[i];
        e->questions[i] = e->questions[j];
        e->questions[j] = t;
    }
}

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

void enigme_preparer_question_courante(Enigme *e)
{
    if (!e->police_question || !e->renderer || e->nb_questions < 1)
        return;
    if (e->quiz_index < 0 || e->quiz_index >= e->nb_questions)
        return;

    e->question_actuelle = e->quiz_index;
    e->reponse_donnee = 0;
    e->resultat_affiche = 0;
    e->chrono_actif = 1;

    if (e->texte_question) {
        SDL_DestroyTexture(e->texte_question);
        e->texte_question = NULL;
    }
    if (e->texte_option_a) {
        SDL_DestroyTexture(e->texte_option_a);
        e->texte_option_a = NULL;
    }
    if (e->texte_option_b) {
        SDL_DestroyTexture(e->texte_option_b);
        e->texte_option_b = NULL;
    }

    const Question *q = &e->questions[e->quiz_index];
    SDL_Color couleur_question = {255, 248, 220, 255};
    SDL_Color couleur_opt = {255, 235, 160, 255};
    SDL_Surface *temp;

    temp = TTF_RenderText_Blended_Wrapped(e->police_question, q->question,
                                          couleur_question, QUESTION_ZONE_W);
    if (temp) {
        e->texte_question = SDL_CreateTextureFromSurface(e->renderer, temp);
        enigme_fit_rect_centered(QUESTION_ZONE_X, QUESTION_ZONE_Y,
                                 QUESTION_ZONE_W, QUESTION_ZONE_H,
                                 temp->w, temp->h, &e->pos_question);
        SDL_FreeSurface(temp);
    }

    TTF_Font *font_opt = e->police_score ? e->police_score : e->police_question;
    if (font_opt) {
        temp = TTF_RenderText_Blended_Wrapped(font_opt, q->option_a,
                                              couleur_opt, 360);
        if (temp) {
            e->texte_option_a = SDL_CreateTextureFromSurface(e->renderer, temp);
            e->pos_option_a.x = 40;
            e->pos_option_a.y = 430;
            e->pos_option_a.w = temp->w < 400 ? temp->w : 400;
            e->pos_option_a.h = temp->h < 140 ? temp->h : 140;
            SDL_FreeSurface(temp);
        }
        temp = TTF_RenderText_Blended_Wrapped(font_opt, q->option_b,
                                              couleur_opt, 360);
        if (temp) {
            e->texte_option_b = SDL_CreateTextureFromSurface(e->renderer, temp);
            e->pos_option_b.x = 460;
            e->pos_option_b.y = 430;
            e->pos_option_b.w = temp->w < 400 ? temp->w : 400;
            e->pos_option_b.h = temp->h < 140 ? temp->h : 140;
            SDL_FreeSurface(temp);
        }
    }

    e->feedback_debut_ms = 0;
    e->temps_debut = SDL_GetTicks();
    e->temps_restant = TEMPS_PAR_QUESTION;
    e->timer_sablier_dsec_cache = -1;
    if (e->tex_timer_sablier) {
        SDL_DestroyTexture(e->tex_timer_sablier);
        e->tex_timer_sablier = NULL;
    }
    enigme_mettre_a_jour_affichage_score(e);
}

void enigme_generer_question_aleatoire(Enigme *e)
{
    enigme_preparer_question_courante(e);
}

static void hourglass_blit_frame(Enigme *e, int fi, Uint8 alpha)
{
    if (alpha == 0 || e->hourglass_sheet_cols < 1)
        return;
    if (fi < 0)
        fi = 0;
    if (fi >= e->hourglass_nb_frames)
        fi = e->hourglass_nb_frames - 1;
    int col = fi % e->hourglass_sheet_cols;
    int row = fi / e->hourglass_sheet_cols;
    SDL_Rect src = {
        col * e->hourglass_frame_w,
        row * e->hourglass_frame_h,
        e->hourglass_frame_w,
        e->hourglass_frame_h
    };
    SDL_SetTextureBlendMode(e->tex_hourglass, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(e->tex_hourglass, alpha);
    SDL_RenderCopy(e->renderer, e->tex_hourglass, &src, &e->pos_hourglass);
}

void enigme_mettre_a_jour_chrono(Enigme *e)
{
    if (!e->chrono_actif || e->reponse_donnee) return;
    
    Uint32 temps_ecoule = SDL_GetTicks() - e->temps_debut;
    e->temps_restant = TEMPS_PAR_QUESTION - temps_ecoule;
    
    if (e->temps_restant <= 0 && !e->reponse_donnee) {
        int mauvais = (e->questions[e->quiz_index].bonne_reponse == 1) ? 2 : 1;
        enigme_verifier_reponse(e, mauvais);
        printf("Temps ecoule! (compte comme une erreur)\n");
    }
}

static int hourglass_map_frame_index(int fi, int n)
{
#if HOURGLASS_SHEET_RIGHT_TO_LEFT
    return (n - 1) - fi;
#else
    (void)n;
    return fi;
#endif
}

void enigme_afficher_chrono_hourglass(Enigme *e)
{
    if (!e->tex_hourglass || e->hourglass_frame_w <= 0 || e->hourglass_nb_frames < 1)
        return;
    if (e->enigme_terminee || e->reponse_donnee || !e->chrono_actif)
        return;

    int n = e->hourglass_nb_frames;
    float pos = 0.f;

#if HOURGLASS_ANIM_USE_LOOP
    {
        Uint32 ms = HOURGLASS_ANIM_FRAME_MS > 0 ? (Uint32)HOURGLASS_ANIM_FRAME_MS : 1u;
        pos = fmodf((float)SDL_GetTicks() / (float)ms, (float)n);
    }
#else
    {
        float ratio = (float)e->temps_restant / (float)TEMPS_PAR_QUESTION;
        if (ratio < 0.0f) ratio = 0.0f;
        if (ratio > 1.0f) ratio = 1.0f;
        if (n > 1)
            pos = (1.0f - ratio) * (float)(n - 1);
    }
#endif

    if (n < 2) {
        hourglass_blit_frame(e, hourglass_map_frame_index(0, n), 255);
        SDL_SetTextureAlphaMod(e->tex_hourglass, 255);
        return;
    }

#if HOURGLASS_SMOOTH_BLEND
    if (pos < 0.f) pos = 0.f;
    if (pos > (float)(n - 1)) pos = (float)(n - 1);
    {
        int fi0 = (int)floorf(pos);
        int fi1 = fi0 + 1;
        float t = pos - (float)fi0;
        if (fi1 >= n) {
            fi1 = n - 1;
            t = 0.f;
        }
        fi0 = hourglass_map_frame_index(fi0, n);
        fi1 = hourglass_map_frame_index(fi1, n);
        Uint8 a0 = (Uint8)((1.f - t) * 255.f + 0.5f);
        Uint8 a1 = (Uint8)(t * 255.f + 0.5f);
        hourglass_blit_frame(e, fi0, a0);
        hourglass_blit_frame(e, fi1, a1);
    }
#else
    {
        int fi = (int)(pos + 0.5f);
        if (fi < 0) fi = 0;
        if (fi >= n) fi = n - 1;
        fi = hourglass_map_frame_index(fi, n);
        hourglass_blit_frame(e, fi, 255);
    }
#endif
    SDL_SetTextureAlphaMod(e->tex_hourglass, 255);
}

void enigme_mettre_a_jour_affichage_score(Enigme *e)
{
    if (!e->police_score || !e->renderer)
        return;
    SDL_Color couleur_hud = {245, 242, 255, 255};
    SDL_Surface *temp;
    char buffer[50];
    
    sprintf(buffer, "Bonnes: %d", e->quiz_bonnes);
    temp = TTF_RenderText_Blended(e->police_score, buffer, couleur_hud);
    if (temp) {
        if (e->texte_score) SDL_DestroyTexture(e->texte_score);
        e->texte_score = SDL_CreateTextureFromSurface(e->renderer, temp);
        e->pos_score.w = temp->w;
        e->pos_score.h = temp->h;
        SDL_FreeSurface(temp);
    }
    
    if (e->nb_questions > 0 && e->quiz_index < e->nb_questions) {
        sprintf(buffer, "Question %d/%d", e->quiz_index + 1, e->nb_questions);
    } else if (e->nb_questions > 0) {
        sprintf(buffer, "Question %d/%d", e->nb_questions, e->nb_questions);
    } else {
        strcpy(buffer, "Question -/-");
    }
    temp = TTF_RenderText_Blended(e->police_score, buffer, couleur_hud);
    if (temp) {
        if (e->texte_vies) SDL_DestroyTexture(e->texte_vies);
        e->texte_vies = SDL_CreateTextureFromSurface(e->renderer, temp);
        e->pos_vies.w = temp->w;
        e->pos_vies.h = temp->h;
        SDL_FreeSurface(temp);
    }
    
    sprintf(buffer, "Touches 1 / 2");
    temp = TTF_RenderText_Blended(e->police_score, buffer, couleur_hud);
    if (temp) {
        if (e->texte_niveau) SDL_DestroyTexture(e->texte_niveau);
        e->texte_niveau = SDL_CreateTextureFromSurface(e->renderer, temp);
        e->pos_niveau.w = temp->w;
        e->pos_niveau.h = temp->h;
        SDL_FreeSurface(temp);
    }
}

static void enigme_mettre_a_jour_timer_sablier(Enigme *e)
{
    TTF_Font *font = e->police_chrono ? e->police_chrono : e->police_score;
    if (!font || !e->renderer)
        return;

    if (e->enigme_terminee || !e->chrono_actif || e->reponse_donnee) {
        if (e->tex_timer_sablier) {
            SDL_DestroyTexture(e->tex_timer_sablier);
            e->tex_timer_sablier = NULL;
        }
        e->timer_sablier_dsec_cache = -1;
        return;
    }

    int ms = e->temps_restant;
    if (ms < 0)
        ms = 0;
    int dsec = ms / 100;
    if (dsec == e->timer_sablier_dsec_cache && e->tex_timer_sablier)
        return;
    e->timer_sablier_dsec_cache = dsec;

    if (e->tex_timer_sablier) {
        SDL_DestroyTexture(e->tex_timer_sablier);
        e->tex_timer_sablier = NULL;
    }

    char buf[40];
    snprintf(buf, sizeof buf, "%.1f s", ms / 1000.0f);
    SDL_Color couleur = {255, 240, 200, 255};
    SDL_Surface *s = TTF_RenderText_Blended(font, buf, couleur);
    if (!s)
        return;
    e->tex_timer_sablier = SDL_CreateTextureFromSurface(e->renderer, s);
    e->pos_timer_sablier.w = s->w;
    e->pos_timer_sablier.h = s->h;
    if (e->pos_hourglass.w > 0 && e->pos_hourglass.h > 0) {
        e->pos_timer_sablier.x = e->pos_hourglass.x + (e->pos_hourglass.w - s->w) / 2;
        e->pos_timer_sablier.y = e->pos_hourglass.y + e->pos_hourglass.h + HOURGLASS_TIMER_GAP;
    } else {
        e->pos_timer_sablier.x = HOURGLASS_SCREEN_X;
        e->pos_timer_sablier.y = HOURGLASS_SCREEN_Y + 72;
    }
    SDL_FreeSurface(s);
}

void enigme_init(Enigme *e, SDL_Renderer *renderer)
{
    SDL_Color couleur_boutons = {255, 235, 160, 255};
    SDL_Surface *temp;
    
    if (e->renderer != NULL)
        enigme_liberer(e);
    memset(e, 0, sizeof(*e));
    e->renderer = renderer;
    e->timer_sablier_dsec_cache = -1;
    
    srand(time(NULL));
    
    e->en_cours = 1;
    e->resultat_affiche = 0;
    e->reponse_donnee = 0;
    e->enigme_terminee = 0;
    e->menu_actif = 0;
    e->score = 0;
    e->vies = 3; // Modifie à 3 vies
    e->niveau_actuel = 1;
    e->nb_questions = 0;
    e->question_actuelle = -1;
    e->quiz_index = 0;
    e->quiz_bonnes = 0;
    e->feedback_debut_ms = 0;
    e->feedback_correct = 0;
    e->texte_option_a = NULL;
    e->texte_option_b = NULL;
    
    e->police_question = TTF_OpenFont("arial.ttf", 42);
    if (!e->police_question) printf("ERREUR: arial.ttf non trouve\n");
    
    e->police_reponse = TTF_OpenFont("arial.ttf", 52);
    if (e->police_reponse)
        TTF_SetFontStyle(e->police_reponse, TTF_STYLE_BOLD);
    e->police_score = TTF_OpenFont("arial.ttf", 28);
    e->police_chrono = TTF_OpenFont("arial.ttf", 38);
    if (!e->police_chrono)
        printf("AVERTISSEMENT: police_chrono (arial 38) non chargee, timer en police score.\n");
    
    enigme_charger_sons(e);
    
    e->fond = enigme_load_texture_data(renderer, "im_fond.png");
    if (!e->fond) {
        printf("ERREUR: Impossible de charger im_fond.png (%s)\n", IMG_GetError());
    }
    
    e->tex_heart = enigme_load_texture_data(renderer, "heart.png");
    if (!e->tex_heart) {
        printf("AVERTISSEMENT: Impossible de charger heart.png\n");
    }

    
    e->tex_hourglass = enigme_load_texture_data(renderer, HOURGLASS_SPRITE_FILE);
    if (!e->tex_hourglass)
        e->tex_hourglass = enigme_load_texture_data(renderer, "hourglass.png");
    if (!e->tex_hourglass)
        e->tex_hourglass = enigme_load_texture_data(renderer, "hourglass_sprites.png.png");
    if (!e->tex_hourglass) {
        printf("AVERTISSEMENT: sablier introuvable (essayez hourglass_sprites.png ou renommez .png.png). "
               "Placez le PNG a cote de l'executable 'enigme'.\n");
    } else {
        SDL_SetTextureBlendMode(e->tex_hourglass, SDL_BLENDMODE_BLEND);
        SDL_SetTextureScaleMode(e->tex_hourglass, SDL_ScaleModeLinear);
        int tw = 0, th = 0;
        int cols = HOURGLASS_SHEET_COLS;
        int rows = HOURGLASS_SHEET_ROWS;
        if (cols < 1) cols = 1;
        if (rows < 1) rows = 1;
        SDL_QueryTexture(e->tex_hourglass, NULL, NULL, &tw, &th);
        e->hourglass_sheet_cols = cols;
        e->hourglass_frame_w = tw / cols;
        e->hourglass_frame_h = th / rows;
        if (e->hourglass_frame_w <= 0) e->hourglass_frame_w = tw;
        if (e->hourglass_frame_h <= 0) e->hourglass_frame_h = th;
        e->hourglass_nb_frames = cols * rows;
#if HOURGLASS_FRAME_COUNT > 0
        if (HOURGLASS_FRAME_COUNT < e->hourglass_nb_frames)
            e->hourglass_nb_frames = HOURGLASS_FRAME_COUNT;
#endif
        if (e->hourglass_nb_frames < 1) e->hourglass_nb_frames = 1;
        if (tw != cols * e->hourglass_frame_w || th != rows * e->hourglass_frame_h)
            printf("AVERTISSEMENT sablier: PNG %dx%d / grille %dx%d — cellules %dx%d (reste ignore)\n",
                   tw, th, cols, rows, e->hourglass_frame_w, e->hourglass_frame_h);
        enigme_fit_rect_top_left(HOURGLASS_SCREEN_X, HOURGLASS_SCREEN_Y,
                                 HOURGLASS_DRAW_MAX_W, HOURGLASS_DRAW_MAX_H,
                                 e->hourglass_frame_w, e->hourglass_frame_h,
                                 &e->pos_hourglass);
        printf("Sablier: %d frames, grille %dx%d, cellule %dx%d\n",
               e->hourglass_nb_frames, cols, rows, e->hourglass_frame_w, e->hourglass_frame_h);
    }
    
    e->btn_oui = NULL;
    e->btn_non = NULL;
    e->msg_reussi = NULL;
    e->msg_echoue = NULL;
    if (e->police_reponse) {
        temp = TTF_RenderText_Blended(e->police_reponse, "OUI", couleur_boutons);
        if (temp) {
            e->btn_oui = SDL_CreateTextureFromSurface(renderer, temp);
            SDL_FreeSurface(temp);
        }
        temp = TTF_RenderText_Blended(e->police_reponse, "NON", couleur_boutons);
        if (temp) {
            e->btn_non = SDL_CreateTextureFromSurface(renderer, temp);
            SDL_FreeSurface(temp);
        }
        temp = TTF_RenderText_Blended(e->police_reponse, "BRAVO!", couleur_boutons);
        if (temp) {
            e->msg_reussi = SDL_CreateTextureFromSurface(renderer, temp);
            SDL_FreeSurface(temp);
        }
        temp = TTF_RenderText_Blended(e->police_reponse, "RATE!", couleur_boutons);
        if (temp) {
            e->msg_echoue = SDL_CreateTextureFromSurface(renderer, temp);
            SDL_FreeSurface(temp);
        }
    }
    
    e->pos_fond = (SDL_Rect){0, 0, 800, 600};
    e->pos_question = (SDL_Rect){QUESTION_ZONE_X, QUESTION_ZONE_Y, QUESTION_ZONE_W, QUESTION_ZONE_H};
    {
        int ow = 0, oh = 0, nw = 0, nh = 0;
        if (e->btn_oui) SDL_QueryTexture(e->btn_oui, NULL, NULL, &ow, &oh);
        if (e->btn_non) SDL_QueryTexture(e->btn_non, NULL, NULL, &nw, &nh);
        enigme_fit_rect_centered(200, 450, 150, 70, ow, oh, &e->pos_btn_oui);
        enigme_fit_rect_centered(450, 450, 150, 70, nw, nh, &e->pos_btn_non);
    }
    e->pos_msg = (SDL_Rect){300, 280, 200, 80};
    e->pos_score = (SDL_Rect){HUD_TEXT_X, 18, 0, 0};
    e->pos_vies = (SDL_Rect){HUD_TEXT_X, 52, 0, 0};
    e->pos_niveau = (SDL_Rect){HUD_TEXT_X, 86, 0, 0};
    
    if (enigme_charger_questions(e, "question.txt") > 0) {
        enigme_melanger_questions(e);
        e->quiz_index = 0;
        e->quiz_bonnes = 0;
        enigme_preparer_question_courante(e);
    }
    
    enigme_mettre_a_jour_affichage_score(e);
}

int enigme_verifier_reponse(Enigme *e, int reponse_choisie)
{
    if (e->reponse_donnee) return 0;
    if (e->quiz_index < 0 || e->quiz_index >= e->nb_questions) return 0;
    
    if (e->son_selection) Mix_PlayChannel(-1, e->son_selection, 0);
    
    e->chrono_actif = 0;
    e->reponse_donnee = 1;
    e->resultat_affiche = 1;
    
    int juste = (reponse_choisie == e->questions[e->quiz_index].bonne_reponse);
    e->feedback_correct = juste;
    
    if (juste) {
        e->quiz_bonnes++;
        e->score = e->quiz_bonnes;
        if (e->son_succes) Mix_PlayChannel(-1, e->son_succes, 0);
        printf("Bonne reponse! Bonnes: %d/%d\n", e->quiz_bonnes, e->nb_questions);
    } else {
        e->vies--;
        if (e->son_erreur) Mix_PlayChannel(-1, e->son_erreur, 0);
        printf("Mauvaise reponse. Vies restantes: %d\n", e->vies);
    }
    
    enigme_mettre_a_jour_affichage_score(e);
    
    return juste;
}

void enigme_afficher(Enigme *e)
{
    enigme_mettre_a_jour_chrono(e);
    enigme_mettre_a_jour_timer_sablier(e);
    
    SDL_SetRenderDrawColor(e->renderer, 0, 0, 0, 255);
    SDL_RenderClear(e->renderer);
    
    if (e->fond) {
        SDL_RenderCopy(e->renderer, e->fond, NULL, &e->pos_fond);
    } else {
        SDL_SetRenderDrawColor(e->renderer, 40, 40, 80, 255);
        SDL_RenderFillRect(e->renderer, &e->pos_fond);
    }
    
    /* Bande de temps au centre en bas */
    if (!e->enigme_terminee && e->chrono_actif) {
        int bar_max_w = 400;
        int bar_h = 24;
        int bar_x = (800 - bar_max_w) / 2; // 200
        int bar_y = 540;
        
        float ratio = (float)e->temps_restant / (float)TEMPS_PAR_QUESTION;
        if (ratio < 0.0f) ratio = 0.0f;
        if (ratio > 1.0f) ratio = 1.0f;
        
        int current_w = (int)(bar_max_w * ratio);
        
        /* Outline background */
        SDL_Rect bg_rect = {bar_x - 3, bar_y - 3, bar_max_w + 6, bar_h + 6};
        SDL_SetRenderDrawColor(e->renderer, 220, 220, 220, 255);
        SDL_RenderDrawRect(e->renderer, &bg_rect);
        
        /* Fill */
        if (current_w > 0) {
            SDL_Rect fill_rect = {bar_x, bar_y, current_w, bar_h};
            int r, g, b = 20;
            if (ratio > 0.5f) {
                g = 255;
                r = (int)(255.0f * (1.0f - ratio) * 2.0f);
            } else {
                r = 255;
                g = (int)(255.0f * ratio * 2.0f);
            }
            if (r > 255) r = 255;
            if (g > 255) g = 255;
            
            SDL_SetRenderDrawColor(e->renderer, r, g, b, 255);
            SDL_RenderFillRect(e->renderer, &fill_rect);
        }
    }
    
    if (e->texte_score) {
        SDL_RenderCopy(e->renderer, e->texte_score, NULL, &e->pos_score);
    }
    if (e->texte_vies) {
        SDL_RenderCopy(e->renderer, e->texte_vies, NULL, &e->pos_vies);
    }
    if (e->texte_niveau) {
        SDL_RenderCopy(e->renderer, e->texte_niveau, NULL, &e->pos_niveau);
    }
    
    /* Dessin des cœurs pour les vies */
    if (e->tex_heart && !e->enigme_terminee) {
        int tw=0, th=0;
        SDL_QueryTexture(e->tex_heart, NULL, NULL, &tw, &th);
        if (tw == 0 || th == 0) { tw = 32; th = 32; }
        
        int cols = 1;
        /* Simple assumption: if it's a wide sprite sheet of hearts, we just take the first frame.
           But usually single icons are 1:1 or 1:x */
        if (tw > 2*th) cols = tw / th; // Basic heuristic if it's a true sprite sheet
        
        int frame_w = tw / cols;
        int draw_w = frame_w > 48 ? 48 : frame_w;
        int draw_h = th > 48 ? 48 : th;
        
        for (int i = 0; i < e->vies; i++) {
            SDL_Rect src_rect = { 0, 0, frame_w, th };
            SDL_Rect dst_rect = { 20 + i * (draw_w + 10), 20, draw_w, draw_h };
            SDL_RenderCopy(e->renderer, e->tex_heart, &src_rect, &dst_rect);
        }
    }
    
    if (e->texte_question && !e->enigme_terminee) {
        SDL_Rect pan = {
            QUESTION_ZONE_X - 12,
            QUESTION_ZONE_Y - 10,
            QUESTION_ZONE_W + 24,
            QUESTION_ZONE_H + 20
        };
        SDL_SetRenderDrawBlendMode(e->renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(e->renderer, 20, 14, 26, 215);
        SDL_RenderFillRect(e->renderer, &pan);
        SDL_SetRenderDrawBlendMode(e->renderer, SDL_BLENDMODE_NONE);
    }
    
    if (e->texte_question && !e->enigme_terminee) {
        SDL_RenderCopy(e->renderer, e->texte_question, NULL, &e->pos_question);
    }
    
    if (!e->reponse_donnee && !e->enigme_terminee) {
        SDL_SetRenderDrawColor(e->renderer, 60, 50, 90, 255);
        SDL_Rect cadre_a = {e->pos_option_a.x - 8, e->pos_option_a.y - 8,
                            e->pos_option_a.w + 16, e->pos_option_a.h + 16};
        SDL_Rect cadre_b = {e->pos_option_b.x - 8, e->pos_option_b.y - 8,
                            e->pos_option_b.w + 16, e->pos_option_b.h + 16};
        SDL_RenderFillRect(e->renderer, &cadre_a);
        SDL_RenderFillRect(e->renderer, &cadre_b);
        SDL_SetRenderDrawColor(e->renderer, 200, 180, 255, 255);
        SDL_RenderDrawRect(e->renderer, &cadre_a);
        SDL_RenderDrawRect(e->renderer, &cadre_b);
        if (e->texte_option_a)
            SDL_RenderCopy(e->renderer, e->texte_option_a, NULL, &e->pos_option_a);
        if (e->texte_option_b)
            SDL_RenderCopy(e->renderer, e->texte_option_b, NULL, &e->pos_option_b);
    }
    
    if (e->resultat_affiche && !e->enigme_terminee) {
        if (e->feedback_debut_ms == 0)
            e->feedback_debut_ms = SDL_GetTicks();
        if (e->feedback_correct && e->msg_reussi) {
            SDL_RenderCopy(e->renderer, e->msg_reussi, NULL, &e->pos_msg);
        } else if (!e->feedback_correct && e->msg_echoue) {
            SDL_RenderCopy(e->renderer, e->msg_echoue, NULL, &e->pos_msg);
        }
        if (SDL_GetTicks() - e->feedback_debut_ms > 2000) {
            e->feedback_debut_ms = 0;
            enigme_reinitialiser(e);
        }
    }
    
    if (e->enigme_terminee) {
        SDL_Color blanc = {255, 255, 255, 255};
        char ligne1[64];
        char ligne2[64];
        if (e->vies <= 0) {
            snprintf(ligne1, sizeof ligne1, "GAME OVER !");
            snprintf(ligne2, sizeof ligne2, "Vous n'avez plus de vies.");
        } else {
            snprintf(ligne1, sizeof ligne1, "Quiz termine !");
            snprintf(ligne2, sizeof ligne2, "Score : %d / %d", e->quiz_bonnes, e->nb_questions);
        }
        SDL_Surface *s1 = TTF_RenderText_Blended(e->police_reponse, ligne1, blanc);
        SDL_Surface *s2 = e->police_question
            ? TTF_RenderText_Blended(e->police_question, ligne2, blanc)
            : NULL;
        if (s1) {
            SDL_Texture *t1 = SDL_CreateTextureFromSurface(e->renderer, s1);
            SDL_Rect r1 = {200, 280, s1->w, s1->h};
            SDL_RenderCopy(e->renderer, t1, NULL, &r1);
            SDL_DestroyTexture(t1);
            SDL_FreeSurface(s1);
        }
        if (s2) {
            SDL_Texture *t2 = SDL_CreateTextureFromSurface(e->renderer, s2);
            SDL_Rect r2 = {200, 360, s2->w, s2->h};
            SDL_RenderCopy(e->renderer, t2, NULL, &r2);
            SDL_DestroyTexture(t2);
            SDL_FreeSurface(s2);
        }
    }
}

void enigme_reinitialiser(Enigme *e)
{
    if (e->enigme_terminee)
        return;
    if (e->vies <= 0) {
        e->enigme_terminee = 1;
        printf("Jeu termine: Plus de vies.\n");
        return;
    }
    e->resultat_affiche = 0;
    e->reponse_donnee = 0;
    e->quiz_index++;
    if (e->quiz_index >= e->nb_questions) {
        e->enigme_terminee = 1;
        printf("Quiz fini. Score: %d / %d\n", e->quiz_bonnes, e->nb_questions);
        return;
    }
    enigme_preparer_question_courante(e);
}

void enigme_gerer_clic(Enigme *e, int x, int y, int *continuer, int *en_jeu)
{
    (void)continuer;
    (void)en_jeu;
    if (!e->reponse_donnee && !e->enigme_terminee) {
        SDL_Rect cadre_a = {e->pos_option_a.x - 8, e->pos_option_a.y - 8,
                            e->pos_option_a.w + 16, e->pos_option_a.h + 16};
        SDL_Rect cadre_b = {e->pos_option_b.x - 8, e->pos_option_b.y - 8,
                            e->pos_option_b.w + 16, e->pos_option_b.h + 16};
        if (x >= cadre_a.x && x < cadre_a.x + cadre_a.w &&
            y >= cadre_a.y && y < cadre_a.y + cadre_a.h) {
            enigme_verifier_reponse(e, 1);
        } else if (x >= cadre_b.x && x < cadre_b.x + cadre_b.w &&
                   y >= cadre_b.y && y < cadre_b.y + cadre_b.h) {
            enigme_verifier_reponse(e, 2);
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
        fprintf(f, "%d\n", e->quiz_index);
        fprintf(f, "%d\n", e->quiz_bonnes);
        
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
        fscanf(f, "%d", &e->quiz_index);
        fscanf(f, "%d", &e->quiz_bonnes);
        
        fclose(f);
        enigme_mettre_a_jour_affichage_score(e);
        printf("Partie chargee! Score: %d, Vies: %d\n", e->score, e->vies);
    }
}

void enigme_liberer(Enigme *e)
{
    if (e->fond) SDL_DestroyTexture(e->fond);
    if (e->tex_heart) SDL_DestroyTexture(e->tex_heart);
    if (e->tex_hourglass) SDL_DestroyTexture(e->tex_hourglass);
    if (e->tex_timer_sablier) SDL_DestroyTexture(e->tex_timer_sablier);
    if (e->texte_question) SDL_DestroyTexture(e->texte_question);
    if (e->texte_option_a) SDL_DestroyTexture(e->texte_option_a);
    if (e->texte_option_b) SDL_DestroyTexture(e->texte_option_b);
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
    if (e->police_chrono) TTF_CloseFont(e->police_chrono);
    
    if (e->son_selection) Mix_FreeChunk(e->son_selection);
    if (e->son_erreur) Mix_FreeChunk(e->son_erreur);
    if (e->son_succes) Mix_FreeChunk(e->son_succes);
    if (e->son_temps_ecoule) Mix_FreeChunk(e->son_temps_ecoule);
    
    Mix_CloseAudio();
}
