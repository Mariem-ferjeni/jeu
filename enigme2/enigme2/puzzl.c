#include "puzzl.h"
// Ajoutez cette fonction au début du fichier
static inline int min(int a, int b) { return (a < b) ? a : b; }
static inline int max(int a, int b) { return (a > b) ? a : b; }
void initialiser_puzzle(EnsemblePuzzle *p)
{
    char *fichiers_images[] = {"oppenheimer_portrait.png", "oppenheimer_trinity.png", 
                        "oppenheimer_losalamos.png", "oppenheimer_bombe.png", "oppenheimer_affiche.png"};
    SDL_Surface *image_temp;
    int i, j, x_depart;
    
    memset(p, 0, sizeof(EnsemblePuzzle));
    srand(time(NULL));
    
    p->jeu_actif = 0;
    p->puzzle_courant = -1;
    p->bonne_proposition_index = -1;
    p->selection_courante = -1;
    p->mode_glisse = 0;
    p->reussite = 0;
    p->termine = 0;
    p->duree_limite = 30;
    p->temps_restant = 30;
    p->chrono_actif = 0;
    p->survol_suivant = 0;
    p->survol_retour = 0;
    p->afficher_message = 0;
    p->angle_message = 0.0f;
    p->echelle_message = 1.0f;
    p->image_rot = NULL;
    p->chrono_joue = 0;
    
    p->couleur_blanc.r = 255; p->couleur_blanc.g = 255; p->couleur_blanc.b = 255;
    p->couleur_vert.r = 0; p->couleur_vert.g = 255; p->couleur_vert.b = 0;
    p->couleur_rouge.r = 255; p->couleur_rouge.g = 0; p->couleur_rouge.b = 0;
    p->couleur_jaune.r = 255; p->couleur_jaune.g = 255; p->couleur_jaune.b = 0;
    
    p->police_normale = TTF_OpenFont("texxte.ttf", 24);
    if(p->police_normale == NULL) p->police_normale = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 24);
    
    p->son_reussite = Mix_LoadWAV("succes.mp3");
    p->son_echec = Mix_LoadWAV("echec.mp3");
    p->son_compte_a_rebours = Mix_LoadWAV("chrono.mp3");
    p->son_selection = Mix_LoadWAV("selection.mp3");
    p->son_depot = Mix_LoadWAV("depose.mp3");
    
    p->image_succes = IMG_Load("succes.png");
    p->image_echec = IMG_Load("echec.png");
    
    for(i = 0; i < NB_PUZZLES; i++)
    {
        image_temp = IMG_Load(fichiers_images[i]);
        p->images_completes[i] = SDL_CreateRGBSurface(0, 350, 250, 32, 0,0,0,0);
        if(image_temp != NULL)
        {
            SDL_BlitScaled(image_temp, NULL, p->images_completes[i], NULL);
            SDL_FreeSurface(image_temp);
        }
        else
            SDL_FillRect(p->images_completes[i], NULL, SDL_MapRGB(p->images_completes[i]->format, 80, 60, 100));
    }
    
    p->emplacement_trou = SDL_CreateRGBSurface(0, 80, 80, 32, 0,0,0,0);
    if(p->emplacement_trou)
    {
        SDL_FillRect(p->emplacement_trou, NULL, SDL_MapRGB(p->emplacement_trou->format, 50, 50, 50));
        for(i = 0; i < 8; i++)
            for(j = 0; j < 8; j++)
                if((i+j) % 2 == 0)
                {
                    SDL_Rect rect = {i*10, j*10, 10, 10};
                    SDL_FillRect(p->emplacement_trou, &rect, SDL_MapRGB(p->emplacement_trou->format, 80, 80, 80));
                }
    }
    
    p->fond = IMG_Load("fond_puzzle.png");
    if(p->fond == NULL)
    {
        p->fond = SDL_CreateRGBSurface(0, LARGEUR_ECRAN, HAUTEUR_ECRAN, 32, 0,0,0,0);
        if(p->fond) SDL_FillRect(p->fond, NULL, SDL_MapRGB(p->fond->format, 30, 30, 50));
    }
    
    p->bouton_suivant_normal = IMG_Load("bouton_suivant_normal.png");
    if(p->bouton_suivant_normal == NULL)
    {
        p->bouton_suivant_normal = SDL_CreateRGBSurface(0, 150, 60, 32, 0,0,0,0);
        if(p->bouton_suivant_normal) SDL_FillRect(p->bouton_suivant_normal, NULL, SDL_MapRGB(p->bouton_suivant_normal->format, 0, 150, 200));
    }
    
    p->bouton_suivant_survol = IMG_Load("bouton_suivant_survol.png");
    if(p->bouton_suivant_survol == NULL)
    {
        p->bouton_suivant_survol = SDL_CreateRGBSurface(0, 150, 60, 32, 0,0,0,0);
        if(p->bouton_suivant_survol) SDL_FillRect(p->bouton_suivant_survol, NULL, SDL_MapRGB(p->bouton_suivant_survol->format, 0, 200, 255));
    }
    
    p->bouton_retour_normal = IMG_Load("bouton_retour_normal.png");
    if(p->bouton_retour_normal == NULL)
    {
        p->bouton_retour_normal = SDL_CreateRGBSurface(0, 150, 60, 32, 0,0,0,0);
        if(p->bouton_retour_normal) SDL_FillRect(p->bouton_retour_normal, NULL, SDL_MapRGB(p->bouton_retour_normal->format, 200, 80, 80));
    }
    
    p->bouton_retour_survol = IMG_Load("bouton_retour_survol.png");
    if(p->bouton_retour_survol == NULL)
    {
        p->bouton_retour_survol = SDL_CreateRGBSurface(0, 150, 60, 32, 0,0,0,0);
        if(p->bouton_retour_survol) SDL_FillRect(p->bouton_retour_survol, NULL, SDL_MapRGB(p->bouton_retour_survol->format, 255, 100, 100));
    }
    
    p->zone_image_principale.x = (LARGEUR_ECRAN - 350) / 2;
    p->zone_image_principale.y = 60;
    p->zone_image_principale.w = 350;
    p->zone_image_principale.h = 250;
    
    p->zone_trou.x = p->zone_image_principale.x + 135;
    p->zone_trou.y = p->zone_image_principale.y + 85;
    p->zone_trou.w = 80;
    p->zone_trou.h = 80;
    
    j = p->zone_image_principale.y + p->zone_image_principale.h + 40;
    x_depart = (LARGEUR_ECRAN - (3 * 80 + 2 * 30)) / 2;
    for(i = 0; i < 3; i++)
    {
        p->zones_propositions[i].x = x_depart + i * (80 + 30);
        p->zones_propositions[i].y = j;
        p->zones_propositions[i].w = 80;
        p->zones_propositions[i].h = 80;
    }
    
    p->position_bouton_retour.x = 30;
    p->position_bouton_retour.y = HAUTEUR_ECRAN - 160;
    p->position_bouton_retour.w = 150;
    p->position_bouton_retour.h = 60;
    
  p->position_bouton_suivant.x = LARGEUR_ECRAN - 300;  // Plus à droit
p->position_bouton_suivant.y = HAUTEUR_ECRAN - 150;
p->position_bouton_suivant.w = 150;
p->position_bouton_suivant.h = 60;
}
void generer_puzzle(EnsemblePuzzle *p)
{
    SDL_Rect source = {135, 85, 80, 80};
    int i, autre_puzzle;
    
    p->puzzle_courant = rand() % NB_PUZZLES;
    p->bonne_proposition_index = rand() % NB_PROPOSITIONS;
    
    for(i = 0; i < NB_PROPOSITIONS; i++)
    {
        if(p->propositions[i].image) SDL_FreeSurface(p->propositions[i].image);
        
        p->propositions[i].image = SDL_CreateRGBSurface(0, 80, 80, 32, 0,0,0,0);
        
        if(i == p->bonne_proposition_index)
        {
            if(p->images_completes[p->puzzle_courant])
                SDL_BlitSurface(p->images_completes[p->puzzle_courant], &source, p->propositions[i].image, NULL);
            p->propositions[i].est_correct = 1;
        }
        else
        {
            autre_puzzle = (p->puzzle_courant + i + 1) % NB_PUZZLES;
            if(p->images_completes[autre_puzzle])
                SDL_BlitSurface(p->images_completes[autre_puzzle], &source, p->propositions[i].image, NULL);
            p->propositions[i].est_correct = 0;
        }
        
        p->propositions[i].identifiant = i;
        p->propositions[i].est_placee = 0;
        p->propositions[i].position = p->zones_propositions[i];
    }
    
    p->selection_courante = -1;
    p->mode_glisse = 0;
    p->reussite = 0;
    p->termine = 0;
    p->chrono_actif = 1;
    p->temps_depart = SDL_GetTicks();
    p->temps_restant = p->duree_limite;
    p->afficher_message = 0;
    p->angle_message = 0.0f;
    p->echelle_message = 0.3f;
    p->chrono_joue = 0;
}
SDL_Surface* rotation_zoom_surface(SDL_Surface *source, float angle, float echelle)
{
    SDL_Surface *temp_scaled, *result;
    int scaled_w, scaled_h;
    int dest_w, dest_h;
    float angle_rad;
    int x, y;
    int src_x, src_y;
    Uint32 pixel;
    Uint8 r, g, b, a;
    float cos_angle, sin_angle;
    int centre_x_src, centre_y_src;
    int min_x, max_x, min_y, max_y;
    
    if(source == NULL) return NULL;
    
    // Dimensions après mise à l'échelle
    scaled_w = (int)(source->w * echelle);
    scaled_h = (int)(source->h * echelle);
    
    if(scaled_w < 1) scaled_w = 1;
    if(scaled_h < 1) scaled_h = 1;
    
    // Créer la surface mise à l'échelle
    temp_scaled = SDL_CreateRGBSurface(0, scaled_w, scaled_h, 32, 
                                       0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if(temp_scaled == NULL) return NULL;
    
    SDL_SetSurfaceBlendMode(temp_scaled, SDL_BLENDMODE_NONE);
    SDL_FillRect(temp_scaled, NULL, SDL_MapRGBA(temp_scaled->format, 0, 0, 0, 0));
    SDL_BlitScaled(source, NULL, temp_scaled, NULL);
    
    // Calculer la taille nécessaire après rotation
    angle_rad = angle * M_PI / 180.0f;
    cos_angle = cos(angle_rad);
    sin_angle = sin(angle_rad);
    
    // Calculer les 4 coins après rotation
    centre_x_src = scaled_w / 2;
    centre_y_src = scaled_h / 2;
    
    // Coin supérieur gauche
    int x1 = -centre_x_src;
    int y1 = -centre_y_src;
    int rx1 = x1 * cos_angle - y1 * sin_angle;
    int ry1 = x1 * sin_angle + y1 * cos_angle;
    
    // Coin supérieur droit
    int x2 = scaled_w - centre_x_src;
    int y2 = -centre_y_src;
    int rx2 = x2 * cos_angle - y2 * sin_angle;
    int ry2 = x2 * sin_angle + y2 * cos_angle;
    
    // Coin inférieur gauche
    int x3 = -centre_x_src;
    int y3 = scaled_h - centre_y_src;
    int rx3 = x3 * cos_angle - y3 * sin_angle;
    int ry3 = x3 * sin_angle + y3 * cos_angle;
    
    // Coin inférieur droit
    int x4 = scaled_w - centre_x_src;
    int y4 = scaled_h - centre_y_src;
    int rx4 = x4 * cos_angle - y4 * sin_angle;
    int ry4 = x4 * sin_angle + y4 * cos_angle;
    
    // Trouver les extrémités
    min_x = min(rx1, min(rx2, min(rx3, rx4)));
    max_x = max(rx1, max(rx2, max(rx3, rx4)));
    min_y = min(ry1, min(ry2, min(ry3, ry4)));
    max_y = max(ry1, max(ry2, max(ry3, ry4)));
    
    dest_w = max_x - min_x;
    dest_h = max_y - min_y;
    
    // Créer la surface de résultat avec la taille appropriée
    result = SDL_CreateRGBSurface(0, dest_w, dest_h, 32,
                                  0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if(result == NULL)
    {
        SDL_FreeSurface(temp_scaled);
        return NULL;
    }
    
    SDL_SetSurfaceBlendMode(result, SDL_BLENDMODE_BLEND);
    SDL_FillRect(result, NULL, SDL_MapRGBA(result->format, 0, 0, 0, 0));
    
    // Verrouiller les surfaces
    SDL_LockSurface(temp_scaled);
    SDL_LockSurface(result);
    
    // Centre de destination
    int centre_x_dest = dest_w / 2;
    int centre_y_dest = dest_h / 2;
    
    // Pour chaque pixel de destination
    for(y = 0; y < dest_h; y++)
    {
        for(x = 0; x < dest_w; x++)
        {
            // Coordonnées relatives au centre de destination
            int rel_x = x - centre_x_dest;
            int rel_y = y - centre_y_dest;
            
            // Rotation inverse pour trouver la source
            src_x = (int)(rel_x * cos_angle + rel_y * sin_angle) + centre_x_src;
            src_y = (int)(-rel_x * sin_angle + rel_y * cos_angle) + centre_y_src;
            
            // Vérifier si dans les limites de la source
            if(src_x >= 0 && src_x < scaled_w && src_y >= 0 && src_y < scaled_h)
            {
                // Lire le pixel source
                pixel = ((Uint32*)temp_scaled->pixels)[src_y * (temp_scaled->pitch / 4) + src_x];
                
                // Extraire les composantes RGBA
                SDL_GetRGBA(pixel, temp_scaled->format, &r, &g, &b, &a);
                
                // Écrire le pixel avec sa transparence
                if(a > 10) // Seuil pour éviter les bords flous
                {
                    ((Uint32*)result->pixels)[y * (result->pitch / 4) + x] = 
                        SDL_MapRGBA(result->format, r, g, b, a);
                }
            }
        }
    }
    
    // Déverrouiller les surfaces
    SDL_UnlockSurface(temp_scaled);
    SDL_UnlockSurface(result);
    
    SDL_FreeSurface(temp_scaled);
    return result;
}

void mettre_a_jour_rotzoom(EnsemblePuzzle *p)
{
    SDL_Surface *image_originale;
    Uint32 temps_ecoule;
    
    if(p == NULL || !p->afficher_message) return;
    
    temps_ecoule = SDL_GetTicks() - p->temps_message;
    if(temps_ecoule > 1500)
    {
        p->afficher_message = 0;
        if(p->image_rot) SDL_FreeSurface(p->image_rot);
        p->image_rot = NULL;
        return;
    }
    
    if(p->reussite)
        image_originale = p->image_succes;
    else
        image_originale = p->image_echec;
    
    if(p->reussite)
    {
        // Animation de succès : rotation et agrandissement
        p->angle_message = (temps_ecoule * 12.0f) / 1000.0f;
        if(p->angle_message >= 360.0f) p->angle_message -= 360.0f;
        
        // Agrandissement progressif
        p->echelle_message = 0.3f + (temps_ecoule * 0.9f) / 1500.0f;
        if(p->echelle_message > 1.2f) p->echelle_message = 1.2f;
    }
    else
    {
        // Animation d'échec : rotation et rétrécissement
        p->angle_message = -(temps_ecoule * 10.0f) / 1000.0f;
        if(p->angle_message <= -360.0f) p->angle_message += 360.0f;
        
        // Rétrécissement progressif
        p->echelle_message = 1.2f - (temps_ecoule * 1.0f) / 1500.0f;
        if(p->echelle_message < 0.2f) p->echelle_message = 0.2f;
    }
    
    if(p->image_rot) SDL_FreeSurface(p->image_rot);
    
    if(image_originale)
    {
        p->image_rot = rotation_zoom_surface(image_originale, p->angle_message, p->echelle_message);
    }
}

void afficher_puzzle(EnsemblePuzzle p, SDL_Surface *ecran)
{
    char texte_temps[20];
    SDL_Surface *surface_temps;
    SDL_Color couleur_temps;
    int i;
    SDL_Rect destination_message;
    
    if(ecran == NULL || !p.jeu_actif) return;
    
    if(p.fond)
    {
        SDL_Rect destination = {0, 0, LARGEUR_ECRAN, HAUTEUR_ECRAN};
        SDL_BlitScaled(p.fond, NULL, ecran, &destination);
    }
    
    if(p.images_completes[p.puzzle_courant])
        SDL_BlitSurface(p.images_completes[p.puzzle_courant], NULL, ecran, &p.zone_image_principale);
    
    if(p.emplacement_trou) SDL_BlitSurface(p.emplacement_trou, NULL, ecran, &p.zone_trou);
    
    for(i = 0; i < NB_PROPOSITIONS; i++)
    {
        SDL_Rect cadre = {p.zones_propositions[i].x - 5, p.zones_propositions[i].y - 5, 90, 90};
        SDL_FillRect(ecran, &cadre, SDL_MapRGB(ecran->format, 80, 60, 40));
        
        if(p.propositions[i].image && !p.propositions[i].est_placee)
            SDL_BlitSurface(p.propositions[i].image, NULL, ecran, &p.zones_propositions[i]);
        else if(p.propositions[i].est_placee && i == p.bonne_proposition_index)
            SDL_BlitSurface(p.propositions[i].image, NULL, ecran, &p.zone_trou);
    }
    
    if(p.mode_glisse && p.image_glissee)
        SDL_BlitSurface(p.image_glissee, NULL, ecran, &p.position_glisse);
    
    if(p.survol_suivant && p.bouton_suivant_survol)
        SDL_BlitSurface(p.bouton_suivant_survol, NULL, ecran, &p.position_bouton_suivant);
    else if(p.bouton_suivant_normal)
        SDL_BlitSurface(p.bouton_suivant_normal, NULL, ecran, &p.position_bouton_suivant);
    
    if(p.survol_retour && p.bouton_retour_survol)
        SDL_BlitSurface(p.bouton_retour_survol, NULL, ecran, &p.position_bouton_retour);
    else if(p.bouton_retour_normal)
        SDL_BlitSurface(p.bouton_retour_normal, NULL, ecran, &p.position_bouton_retour);
    
    if(p.temps_restant <= 10) couleur_temps = p.couleur_rouge;
    else if(p.temps_restant <= 20) couleur_temps = p.couleur_jaune;
    else couleur_temps = p.couleur_blanc;
    
    sprintf(texte_temps, "Temps: %ds", p.temps_restant);
    surface_temps = TTF_RenderText_Blended(p.police_normale, texte_temps, couleur_temps);
    if(surface_temps)
    {
        SDL_Rect position = {LARGEUR_ECRAN - 130, 20, surface_temps->w, surface_temps->h};
        SDL_BlitSurface(surface_temps, NULL, ecran, &position);
        SDL_FreeSurface(surface_temps);
    }
    
   if(p.termine && p.afficher_message && p.image_rot)
{
    destination_message.x = (LARGEUR_ECRAN - p.image_rot->w) / 2;
    destination_message.y = (HAUTEUR_ECRAN - p.image_rot->h) / 2 - 50;
    destination_message.w = p.image_rot->w;
    destination_message.h = p.image_rot->h;
    
    // S'assurer que le blend mode est activé
    SDL_SetSurfaceBlendMode(p.image_rot, SDL_BLENDMODE_BLEND);
    SDL_BlitSurface(p.image_rot, NULL, ecran, &destination_message);
}
}

int gerer_evenement(EnsemblePuzzle *p, SDL_Event evenement, int *points, int *vies_restantes)
{
    int x, y, i;
    
    if(p == NULL || !p->jeu_actif) return 1;
    
    if(evenement.type == SDL_QUIT) return 0;
    
    if(evenement.type == SDL_MOUSEMOTION)
    {
        x = evenement.motion.x;
        y = evenement.motion.y;
        
        if(x >= p->position_bouton_suivant.x && x <= p->position_bouton_suivant.x + p->position_bouton_suivant.w && 
           y >= p->position_bouton_suivant.y && y <= p->position_bouton_suivant.y + p->position_bouton_suivant.h)
        {
            p->survol_suivant = 1;
        }
        else
            p->survol_suivant = 0;
        
        if(x >= p->position_bouton_retour.x && x <= p->position_bouton_retour.x + p->position_bouton_retour.w && 
           y >= p->position_bouton_retour.y && y <= p->position_bouton_retour.y + p->position_bouton_retour.h)
        {
            p->survol_retour = 1;
        }
        else
            p->survol_retour = 0;
        
        if(p->mode_glisse)
        {
            p->position_glisse.x = evenement.motion.x - 40;
            p->position_glisse.y = evenement.motion.y - 40;
        }
    }
    
    if(evenement.type == SDL_MOUSEBUTTONDOWN && evenement.button.button == SDL_BUTTON_LEFT && !p->termine)
    {
        x = evenement.button.x;
        y = evenement.button.y;
        
        for(i = 0; i < NB_PROPOSITIONS; i++)
        {
            if(x >= p->zones_propositions[i].x && x <= p->zones_propositions[i].x + 80 &&
               y >= p->zones_propositions[i].y && y <= p->zones_propositions[i].y + 80 &&
               !p->propositions[i].est_placee)
            {
                p->selection_courante = i;
                p->mode_glisse = 1;
                p->image_glissee = p->propositions[i].image;
                p->position_glisse.x = x - 40;
                p->position_glisse.y = y - 40;
                p->position_glisse.w = 80;
                p->position_glisse.h = 80;
                if(p->son_selection) Mix_PlayChannel(-1, p->son_selection, 0);
                break;
            }
        }
    }
    
    if(evenement.type == SDL_MOUSEBUTTONUP && evenement.button.button == SDL_BUTTON_LEFT && p->mode_glisse)
    {
        x = evenement.button.x;
        y = evenement.button.y;
        
        if(x >= p->zone_trou.x && x <= p->zone_trou.x + 80 && y >= p->zone_trou.y && y <= p->zone_trou.y + 80)
        {
            if(p->selection_courante == p->bonne_proposition_index)
            {
                p->propositions[p->selection_courante].est_placee = 1;
                p->reussite = 1;
                p->termine = 1;
                p->chrono_actif = 0;
                p->afficher_message = 1;
                p->temps_message = SDL_GetTicks();
                p->angle_message = 0.0f;
                p->echelle_message = 0.3f;
                if(points) *points += 50;
                if(p->son_reussite) Mix_PlayChannel(-1, p->son_reussite, 0);
            }
            else
            {
                p->reussite = 0;
                p->termine = 1;
                p->chrono_actif = 0;
                p->afficher_message = 1;
                p->temps_message = SDL_GetTicks();
                p->angle_message = 0.0f;
                p->echelle_message = 1.2f;
                if(points) *points -= 20;
                if(vies_restantes) *vies_restantes -= 1;
                if(p->son_echec) Mix_PlayChannel(-1, p->son_echec, 0);
            }
            if(p->son_depot) Mix_PlayChannel(-1, p->son_depot, 0);
        }
        p->mode_glisse = 0;
        p->selection_courante = -1;
        p->image_glissee = NULL;
    }
    
    if(evenement.type == SDL_MOUSEBUTTONDOWN && evenement.button.button == SDL_BUTTON_LEFT && p->termine)
    {
        x = evenement.button.x;
        y = evenement.button.y;
        
        if(x >= p->position_bouton_suivant.x && x <= p->position_bouton_suivant.x + p->position_bouton_suivant.w && 
           y >= p->position_bouton_suivant.y && y <= p->position_bouton_suivant.y + p->position_bouton_suivant.h)
        {
            if(p->son_selection) Mix_PlayChannel(-1, p->son_selection, 0);
            return 2;
        }
        
        if(x >= p->position_bouton_retour.x && x <= p->position_bouton_retour.x + p->position_bouton_retour.w && 
           y >= p->position_bouton_retour.y && y <= p->position_bouton_retour.y + p->position_bouton_retour.h)
        {
            if(p->son_selection) Mix_PlayChannel(-1, p->son_selection, 0);
            return 0;
        }
    }
    
    if(evenement.type == SDL_KEYDOWN && evenement.key.keysym.sym == SDLK_ESCAPE) return 0;
    
    return 1;
}

void mettre_a_jour_temps(EnsemblePuzzle *p)
{
    int duree_ecoulee;
    
    if(p == NULL || !p->chrono_actif || p->termine) return;
    
    duree_ecoulee = (SDL_GetTicks() - p->temps_depart) / 1000;
    p->temps_restant = p->duree_limite - duree_ecoulee;
    
    if(p->temps_restant <= 5 && p->temps_restant > 0 && !p->chrono_joue && p->son_compte_a_rebours)
    {
        Mix_PlayChannel(-1, p->son_compte_a_rebours, 0);
        p->chrono_joue = 1;
    }
    
    if(p->temps_restant <= 0 && p->chrono_actif)
    {
        p->temps_restant = 0;
        p->chrono_actif = 0;
        p->termine = 1;
        p->reussite = 0;
        p->afficher_message = 1;
        p->temps_message = SDL_GetTicks();
        p->angle_message = 0.0f;
        p->echelle_message = 1.2f;
        if(p->son_echec) Mix_PlayChannel(-1, p->son_echec, 0);
    }
}

void liberer_puzzle(EnsemblePuzzle *p)
{
    int i;
    
    if(p == NULL) return;
    
    for(i = 0; i < NB_PUZZLES; i++)
        if(p->images_completes[i]) SDL_FreeSurface(p->images_completes[i]);
    
    for(i = 0; i < NB_PROPOSITIONS; i++)
        if(p->propositions[i].image) SDL_FreeSurface(p->propositions[i].image);
    
    if(p->emplacement_trou) SDL_FreeSurface(p->emplacement_trou);
    if(p->fond) SDL_FreeSurface(p->fond);
    if(p->bouton_suivant_normal) SDL_FreeSurface(p->bouton_suivant_normal);
    if(p->bouton_suivant_survol) SDL_FreeSurface(p->bouton_suivant_survol);
    if(p->bouton_retour_normal) SDL_FreeSurface(p->bouton_retour_normal);
    if(p->bouton_retour_survol) SDL_FreeSurface(p->bouton_retour_survol);
    if(p->police_normale) TTF_CloseFont(p->police_normale);
    if(p->son_reussite) Mix_FreeChunk(p->son_reussite);
    if(p->son_echec) Mix_FreeChunk(p->son_echec);
    if(p->son_compte_a_rebours) Mix_FreeChunk(p->son_compte_a_rebours);
    if(p->son_selection) Mix_FreeChunk(p->son_selection);
    if(p->son_depot) Mix_FreeChunk(p->son_depot);
    if(p->image_rot) SDL_FreeSurface(p->image_rot);
    if(p->image_succes) SDL_FreeSurface(p->image_succes);
    if(p->image_echec) SDL_FreeSurface(p->image_echec);
}
