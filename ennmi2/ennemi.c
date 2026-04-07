#include "ennemi.h" // Inclusion du fichier header contenant la structure Ennemi et les prototypes

// Fonction d'initialisation de l'ennemi
void initEnnemi(Ennemi *e)
{
    static int init = 0; // Variable statique pour vérifier si srand a déjà été appelé
    if(!init) {
        srand(time(NULL)); // Initialisation du générateur aléatoire (une seule fois)
        init = 1;
    }
    
    // Position de départ de l'ennemi (rectangle SDL)
    e->posEnnemi.x = 100;  // Position X
    e->posEnnemi.y = 400;  // Position Y
    e->posEnnemi.w = 80;   // Largeur
    e->posEnnemi.h = 100;  // Hauteur
    
    // Définition du point de départ du mouvement
    e->posDepart.x = 100;
    e->posDepart.y = 400;
    
    // Définition du point d'arrivée (limite du déplacement)
    e->posArrivee.x = 700;
    e->posArrivee.y = 400;
    
    e->direction = 0;  // 0 = déplacement vers la droite, 1 = vers la gauche
    e->vitesse = 3;    // Vitesse de déplacement (pixels par frame)
    e->vie = 100;      // Points de vie de l'ennemi
    e->actif = 1;      // 1 = ennemi actif (vivant), 0 = mort/inactif
    e->degats = 10;    // Dégâts infligés au joueur
    e->frameActuelle = 0;        // Frame actuelle pour l'animation
    e->compteurAnimation = 0;    // Compteur pour ralentir l'animation
    
    // Initialisation du tableau de textures à NULL
    for(int i = 0; i < 5; i++) {
        e->textures[i] = NULL;
    }
    
    printf("Ennemi initialisé - Trajectoire horizontale de x=100 à x=700\n"); // Debug
}

// Fonction pour charger les images de l'ennemi
void chargerTexturesEnnemi(Ennemi *e, SDL_Renderer *renderer)
{
    char chemin[100]; // Variable pour stocker le chemin de chaque image
    
    // Boucle pour charger 5 images
    for(int i = 0; i < 5; i++) {
        sprintf(chemin, "ennemi_%d.png", i+1); // Génère le nom: ennemi_1.png ... ennemi_5.png
        
        SDL_Surface* surface = IMG_Load(chemin); // Chargement de l'image
        
        if(surface) { // Si chargement réussi
            e->textures[i] = SDL_CreateTextureFromSurface(renderer, surface); // Conversion en texture
            SDL_FreeSurface(surface); // Libération de la surface
            printf("Chargé: %s\n", chemin); // Message de succès
        } else {
            printf("Erreur chargement: %s\n", chemin); // Message d'erreur
            e->textures[i] = NULL;
        }
    }
}

// Fonction d'affichage de l'ennemi
void afficherEnnemi(Ennemi e, SDL_Renderer *renderer)
{  
    // Vérifie que le renderer existe, que l'ennemi est actif et que la texture existe
    if(renderer && e.actif && e.textures[e.frameActuelle]) {
        // Affiche la texture actuelle à la position de l'ennemi
        SDL_RenderCopy(renderer, e.textures[e.frameActuelle], NULL, &e.posEnnemi);
    }
}

// Fonction pour gérer l'animation de l'ennemi
void animerEnnemi(Ennemi *e)
{
    if(!e->actif) return; // Si ennemi mort, ne rien faire
    
    e->compteurAnimation++; // Incrémenter le compteur
    
    // Tous les 8 ticks, on change de frame
    if(e->compteurAnimation >= 8) {
        e->compteurAnimation = 0;
        e->frameActuelle++; // Passer à la frame suivante
        
        // Si on dépasse le nombre de frames, on revient à 0 (boucle)
        if(e->frameActuelle >= 5) {
            e->frameActuelle = 0;
        }
    }
}

// Fonction de déplacement horizontal (va-et-vient)
void deplacerLigne(Ennemi *e)
{
    if(!e->actif) return; // Si ennemi mort, ne rien faire
    
    // Cas 1: déplacement vers la droite
    if(e->direction == 0) {
        e->posEnnemi.x += e->vitesse; // Avancer vers la droite
        
        // Si on atteint la limite droite
        if(e->posEnnemi.x + e->posEnnemi.w >= e->posArrivee.x) {
            e->posEnnemi.x = e->posArrivee.x - e->posEnnemi.w; // Corriger la position
            e->direction = 1;  // Changer direction vers gauche
            printf("Ennemi change direction: va vers GAUCHE\n");
        }
    } 
    // Cas 2: déplacement vers la gauche
    else {
        e->posEnnemi.x -= e->vitesse; // Avancer vers la gauche
        
        // Si on atteint la limite gauche
        if(e->posEnnemi.x <= e->posDepart.x) {
            e->posEnnemi.x = e->posDepart.x; // Corriger position
            e->direction = 0;  // Changer direction vers droite
            printf("Ennemi change direction: va vers DROITE\n");
        }
    }
    
    // Affichage de debug de la position toutes les 30 frames
    static int compteur = 0;
    if(compteur++ % 30 == 0) {
        printf("Ennemi position: x=%d, direction=%s\n", 
               e->posEnnemi.x, 
               e->direction == 0 ? "DROITE" : "GAUCHE");
    }
}

// Fonction pour infliger des dégâts à l'ennemi
void ennemiPrendreDegats(Ennemi *e, int degats)
{
    if(!e->actif) return; // Si déjà mort, ne rien faire
    
    e->vie -= degats; // Réduction des points de vie
    printf("Ennemi touché! Vie: %d\n", e->vie);
    
    // Si la vie tombe à 0 ou moins
    if(e->vie <= 0) {
        e->actif = 0; // Désactiver l'ennemi
        printf("Ennemi vaincu!\n");
    }
}

// Fonction de collision Bounding Box (rectangle vs rectangle)
int collisionBB(perso p, Ennemi e)
{
    if(!e.actif) return 0; // Pas de collision si ennemi mort
    
    // Vérifie les cas où il n'y a PAS collision
    if((p.poshero.x + p.poshero.w < e.posEnnemi.x) ||   // joueur à gauche
       (p.poshero.x > e.posEnnemi.x + e.posEnnemi.w) || // joueur à droite
       (p.poshero.y + p.poshero.h < e.posEnnemi.y) ||   // joueur au-dessus
       (p.poshero.y > e.posEnnemi.y + e.posEnnemi.h)) { // joueur en-dessous
        return 0; // Pas de collision
    }
    
    return 1; // Collision détectée
}

// Fonction de gestion de collision joueur-ennemi
void gererCollision(perso *p, Ennemi *e)
{
    if(!e->actif) return; // Si ennemi mort, rien à faire
    
    // Vérifie collision
    if(collisionBB(*p, *e)) {
        Uint32 currentTime = SDL_GetTicks(); // Temps actuel
        
        // Délai de 1 seconde entre chaque dégât
        if(currentTime - p->lastHit > 1000) {
            p->vie -= e->degats; // Réduire la vie du joueur
            p->lastHit = currentTime; // Mettre à jour le temps du dernier coup
            
            printf("Joueur touché! Vie: %d\n", p->vie);
            
            // Vérifier si le joueur est mort
            if(p->vie <= 0) {
                printf("GAME OVER!\n");
            }
        }
    }
}

