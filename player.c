#include <SDL2/SDL_image.h>
#include <math.h>  // Pour la fonction sqrt
#include "player.h"
// Fonction pour charger une texture
SDL_Texture* load_texture(SDL_Renderer* renderer, const char* path) {
    SDL_Surface* surface = IMG_Load(path);
    if (surface == NULL) {
        printf("Erreur chargement image %s: %s\n", path, IMG_GetError());
        return NULL;
    }
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface); // Conversion en texture pour l'opti
    SDL_FreeSurface(surface);
    
    if (texture == NULL) {
        printf("Erreur création texture %s: %s\n", path, SDL_GetError());
        return NULL;
    }
    
    return texture;
}

// Initialisation du joueur
void player_init(Player* p, SDL_Renderer* renderer) {
    p->x = 400.0f;  // Centre de l'écran
    p->y = 300.0f;
    p->vx = 0.0f;
    p->vy = 0.0f;
    p->w = 48.0f;
    p->h = 48.0f;
    p->alive = true;
    p->direction = 0; // Commence vers le bas
    
    // Stats du joueur
    p->speed = 200.0f;        // Vitesse de déplacement
    p->max_x = 800.0f;        // Limite droite de l'écran
    p->max_y = 600.0f;        // Limite basse de l'écran
    
    // Stats de tir
    p->projectile_damage = 10.0f;     // Dégâts des projectiles
    p->fire_rate = 2.0f;              // 2 projectiles par seconde
    p->last_shot_time = 0.0f;         // Pas encore tiré
    p->projectile_speed = 300.0f;      // Vitesse des projectiles
    
    // Chargement des textures
    p->texture_up = load_texture(renderer, "assets/images/personnages/personnage_haut.png");
    p->texture_down = load_texture(renderer, "assets/images/personnages/personnage_bas.png");
    p->texture_left = load_texture(renderer, "assets/images/personnages/personnage_gauche.png");
    p->texture_right = load_texture(renderer, "assets/images/personnages/personnage_droite.png");
    
    // Texture par défaut
    p->current_texture = p->texture_down;
}

// Mise à jour du joueur
void player_update(Player* p, const Uint8* keys, float dt, Projectile* projectiles, int max_projectiles, SDL_Renderer* renderer) {
    if (!p->alive) return;
    
    // Variables pour les directions
    float move_x = 0.0f;
    float move_y = 0.0f;
    
    // Gestion des touches et direction (SEULEMENT ZQSD pour le mouvement)
    if (keys[SDL_SCANCODE_W]) {
        move_y = -1.0f;  // Vers le haut
    }
    if (keys[SDL_SCANCODE_S]) {
        move_y = 1.0f;   // Vers le bas
    }
    if (keys[SDL_SCANCODE_A]) {
        move_x = -1.0f;  // Vers la gauche
    }
    if (keys[SDL_SCANCODE_D]) {
        move_x = 1.0f;   // Vers la droite
    }
    
    // éviter la vitesse diagonale trop rapide
    if (move_x != 0.0f || move_y != 0.0f) {
        // Calcul de la longueur du vecteur
        float magnitude = sqrt(move_x * move_x + move_y * move_y);
        
        // diviser par la magnitude pour avoir un vecteur unitaire
        move_x = move_x / magnitude;
        move_y = move_y / magnitude;
        
        p->vx = move_x * p->speed;
        p->vy = move_y * p->speed;
        
        // Mise à jour de la direction et texture
        if (move_y < 0) {
            p->direction = 1; // Haut
            p->current_texture = p->texture_up;
        } else if (move_y > 0) {
            p->direction = 0; // Bas
            p->current_texture = p->texture_down;
        } else if (move_x < 0) {
            p->direction = 2; // Gauche
            p->current_texture = p->texture_left;
        } else if (move_x > 0) {
            p->direction = 3; // Droite
            p->current_texture = p->texture_right;
        }
    } else {
        // Aucune touche pressée
        p->vx = 0.0f;
        p->vy = 0.0f;
    }
    
    // Mise à jour position
    p->x += p->vx * dt; // dt est le delta time
    p->y += p->vy * dt;
    
    // Limites écran (utilisant les propriétés de la structure)
    if (p->x < 0) p->x = 0;
    if (p->x + p->w > p->max_x) p->x = p->max_x - p->w;
    if (p->y < 0) p->y = 0;
    if (p->y + p->h > p->max_y) p->y = p->max_y - p->h;
    
    // Gestion du tir avec les flèches directionnelles
    float current_time = SDL_GetTicks() / 1000.0f;
    
    // Chercher un projectile inactif pour tirer
    Projectile* free_projectile = NULL;
    for (int i = 0; i < max_projectiles; i++) {
        if (!projectiles[i].active) {
            free_projectile = &projectiles[i];
            break;
        }
    }
    
    // Tir avec les flèches directionnelles
    if (free_projectile != NULL && player_can_shoot(p, current_time)) {
        if (keys[SDL_SCANCODE_DOWN]) {
            player_shoot(p, free_projectile, 0, renderer);  // Bas
        } else if (keys[SDL_SCANCODE_UP]) {
            player_shoot(p, free_projectile, 1, renderer);  // Haut
        } else if (keys[SDL_SCANCODE_LEFT]) {
            player_shoot(p, free_projectile, 2, renderer);  // Gauche
        } else if (keys[SDL_SCANCODE_RIGHT]) {
            player_shoot(p, free_projectile, 3, renderer);  // Droite
        }
    }
}

// Rendu du joueur
void player_render(SDL_Renderer* r, Player* p) {
    if (!p->alive) return;
    
    SDL_Rect rect = { // Taille du perso dans le fichier player.h
        (int)p->x,
        (int)p->y,
        (int)p->w,
        (int)p->h
    };
    
    // Rendu de la texture si disponible
    if (p->current_texture != NULL) {
        SDL_RenderCopy(r, p->current_texture, NULL, &rect);
    } else {
        // rectangle bleu si pas de texture
        SDL_SetRenderDrawColor(r, 0, 100, 255, 255);
        SDL_RenderFillRect(r, &rect);
    }
}

// Nettoyage des ressources
void player_cleanup(Player* p) {
    if (p->texture_up != NULL) {
        SDL_DestroyTexture(p->texture_up);
        p->texture_up = NULL;
    }
    if (p->texture_down != NULL) {
        SDL_DestroyTexture(p->texture_down);
        p->texture_down = NULL;
    }
    if (p->texture_left != NULL) {
        SDL_DestroyTexture(p->texture_left);
        p->texture_left = NULL;
    }
    if (p->texture_right != NULL) {
        SDL_DestroyTexture(p->texture_right);
        p->texture_right = NULL;
    }
    p->current_texture = NULL;
}

// Fonction pour vérifier si le joueur peut tirer
bool player_can_shoot(Player* p, float current_time) {
    float time_since_last_shot = current_time - p->last_shot_time;
    float fire_interval = 1.0f / p->fire_rate;  // Intervalle entre les tirs
    
    return time_since_last_shot >= fire_interval;
}

// Fonction pour faire tirer le joueur
void player_shoot(Player* p, Projectile* projectile, int direction, SDL_Renderer* renderer) {
    // Position de départ du projectile (centre du joueur)
    float start_x = p->x + p->w / 2.0f;
    float start_y = p->y + p->h / 2.0f;
    
    // Initialiser le projectile
    projectile_init(projectile, start_x, start_y, direction, p->projectile_speed, p->projectile_damage, renderer);
    
    // Mettre à jour le temps du dernier tir
    p->last_shot_time = SDL_GetTicks() / 1000.0f;
}

// Initialisation d'un projectile
void projectile_init(Projectile* proj, float x, float y, int direction, float speed, float damage, SDL_Renderer* renderer) {
    proj->x = x;
    proj->y = y;
    proj->speed = speed;
    proj->damage = damage;
    proj->direction = direction;
    proj->active = true;
    proj->lifetime = 0.0f;
    proj->max_lifetime = 3.0f;  // 3 secondes de vie
    proj->size = 20.0f;         // Taille d'affichage du projectile (3x plus grand)
    
    // Charger la texture du projectile
    proj->texture = load_texture(renderer, "assets/images/projectiles/proj.png");
    if (proj->texture == NULL) {
        printf("ATTENTION: Impossible de charger proj.png, utilisation du rendu par défaut\n");
    }
    
    // Calculer la direction de la vitesse
    switch (direction) {
        case 0: // Bas
            proj->vx = 0.0f;
            proj->vy = speed;
            break;
        case 1: // Haut
            proj->vx = 0.0f;
            proj->vy = -speed;
            break;
        case 2: // Gauche
            proj->vx = -speed;
            proj->vy = 0.0f;
            break;
        case 3: // Droite
            proj->vx = speed;
            proj->vy = 0.0f;
            break;
    }
}

// Mise à jour d'un projectile
void projectile_update(Projectile* proj, float dt) {
    if (!proj->active) return;
    
    // Mise à jour de la position
    proj->x += proj->vx * dt;
    proj->y += proj->vy * dt;
    
    // Mise à jour de la durée de vie
    proj->lifetime += dt;
    
    // Vérifier si le projectile est mort
    if (proj->lifetime >= proj->max_lifetime) {
        proj->active = false;
    }
    
    // Vérifier les limites de l'écran (désactiver si sort de l'écran)
    if (proj->x < 0 || proj->x > 800 || proj->y < 0 || proj->y > 600) {
        proj->active = false;
    }
}

// Rendu d'un projectile
void projectile_render(SDL_Renderer* r, Projectile* proj) {
    if (!proj->active) return;
    
    // Rectangle du projectile
    SDL_Rect rect = {
        (int)(proj->x - proj->size / 2),  // Centrer le projectile
        (int)(proj->y - proj->size / 2),
        (int)proj->size,
        (int)proj->size
    };
    
    // Rendu de la texture si disponible
    if (proj->texture != NULL) {
        SDL_RenderCopy(r, proj->texture, NULL, &rect);
    } else {
        // Fallback : carré jaune si pas de texture
        SDL_SetRenderDrawColor(r, 255, 255, 0, 255);  // Jaune
        SDL_RenderFillRect(r, &rect);
    }
}

// Nettoyage d'un projectile
void projectile_cleanup(Projectile* proj) {
    if (proj->texture != NULL) {
        SDL_DestroyTexture(proj->texture);
        proj->texture = NULL;
    }
}
