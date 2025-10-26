#include <SDL2/SDL_image.h>
#include <math.h>  // Pour la fonction sqrt
#include "player.h"

// Fonctions de collision du décor
extern bool decor_check_collision_with_grid(void* manager, float x, float y, float w, float h);
extern bool decor_check_horizontal_collision(void* manager, float x, float y, float w, float h);
extern bool decor_check_vertical_collision_up(void* manager, float x, float y, float w, float h);
extern bool decor_check_vertical_collision_down(void* manager, float x, float y, float w, float h);
extern bool decor_check_projectile_collision(void* manager, float x, float y, float size);
// Fonction pour charger une texture
SDL_Texture* load_texture(SDL_Renderer* renderer, const char* path) {
    SDL_Surface* surface = IMG_Load(path);
    if (surface == NULL) {
        printf("Erreur chargement image %s: %s\n", path, IMG_GetError());
        return NULL;
    }
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface); // Conversion en texture
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
    p->speed = 200.0f;        // Vitesse
    p->max_x = 800.0f;        // Limite droite
    p->max_y = 600.0f;        // Limite basse
    
    // Stats de tir
    p->projectile_damage = 10.0f;     // Dégâts
    p->fire_rate = 2.0f;              // Cadence de tir
    p->last_shot_time = 0.0f;         // Dernier tir
    p->projectile_speed = 300.0f;      // Vitesse des tirs
    
    // Chargement des textures
    p->texture_up = load_texture(renderer, "assets/images/personnages/personnage_haut.png");
    p->texture_down = load_texture(renderer, "assets/images/personnages/personnage_bas.png");
    p->texture_left = load_texture(renderer, "assets/images/personnages/personnage_gauche.png");
    p->texture_right = load_texture(renderer, "assets/images/personnages/personnage_droite.png");
    
    // Texture par défaut
    p->current_texture = p->texture_down;
}

// Fonction de collision pour le joueur (vérifie les pieds ET les côtés)
bool player_check_collision(Player* p, float new_x, float new_y, void* decor_manager) {
    if (decor_manager == NULL) return false;
    
    // Vérifier les limites de l'écran d'abord
    if (new_x < 0 || new_x + p->w > p->max_x || 
        new_y < 0 || new_y + p->h > p->max_y) {
        return true; // Collision avec les bords
    }
    
    // Vérifier la collision avec les éléments de décor
    // On vérifie toute la taille du joueur pour les collisions latérales
    return decor_check_collision_with_grid(decor_manager, new_x, new_y, p->w, p->h);
}

// Mise à jour du joueur avec collision
void player_update(Player* p, const Uint8* keys, float dt, Projectile* projectiles, int max_projectiles, SDL_Renderer* renderer, void* decor_manager) {
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
    
    // Calculer la nouvelle position
    float new_x = p->x + p->vx * dt;
    float new_y = p->y + p->vy * dt;
    
    // Vérifier les collisions avant de déplacer
    // Séparer les collisions horizontales et verticales pour un mouvement plus naturel
    // Ça évite que le joueur soit bloqué si seulement une partie de son corps touche un obstacle
    
    // Mouvement horizontal (gauche/droite)
    if (p->vx != 0) {
        // Vérifier les limites de l'écran d'abord (sinon le joueur peut sortir de la fenêtre)
        if (new_x >= 0 && new_x + p->w <= p->max_x) {
            // Vérifier collision avec les éléments de décor (rochers, murs, etc.)
            if (!decor_check_horizontal_collision(decor_manager, new_x, p->y, p->w, p->h)) {
                p->x = new_x; // Déplacement horizontal OK
            }
        }
    }
    
    // Mouvement vertical (haut/bas) - utiliser les fonctions spécialisées
    if (p->vy != 0) {
        // Vérifier les limites de l'écran d'abord (sinon le joueur peut sortir de la fenêtre)
        if (new_y >= 0 && new_y + p->h <= p->max_y) {
            bool can_move = false;
            
            if (p->vy < 0) {
                // Mouvement vers le haut - vérifier seulement la tête
                // Ça permet de passer sous les obstacles si on a juste la tête qui touche
                can_move = !decor_check_vertical_collision_up(decor_manager, p->x, new_y, p->w, p->h);
            } else {
                // Mouvement vers le bas - vérifier seulement les pieds
                // Ça permet de descendre même si les "oreilles" du joueur touchent un obstacle
                can_move = !decor_check_vertical_collision_down(decor_manager, p->x, new_y, p->w, p->h);
            }
            
            if (can_move) {
                p->y = new_y; // Déplacement vertical OK
            }
        }
    }
    
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

// Mise à jour d'un projectile avec collision
void projectile_update(Projectile* proj, float dt, void* decor_manager) {
    if (!proj->active) return;
    
    // Mise à jour de la position
    proj->x += proj->vx * dt;
    proj->y += proj->vy * dt;
    
    // Vérifier les collisions avec les éléments de décor
    if (decor_manager != NULL && decor_check_projectile_collision(decor_manager, proj->x, proj->y, proj->size)) {
        // Le projectile est bloqué par un élément solide
        proj->active = false; // Désactiver le projectile
        return;
    }
    
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
