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
    
    // Chargement des textures
    p->texture_up = load_texture(renderer, "assets/personnage_haut.png");
    p->texture_down = load_texture(renderer, "assets/personnage_bas.png");
    p->texture_left = load_texture(renderer, "assets/personnage_gauche.png");
    p->texture_right = load_texture(renderer, "assets/personnage_droite.png");
    
    // Texture par défaut
    p->current_texture = p->texture_down;
}

// Mise à jour du joueur
void player_update(Player* p, const Uint8* keys, float dt) {
    if (!p->alive) return;
    
    // Variables pour les directions
    float move_x = 0.0f;
    float move_y = 0.0f;
    
    // Gestion des touches et direction
    if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP]) {
        move_y = -1.0f;  // Vers le haut
    }
    if (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN]) {
        move_y = 1.0f;   // Vers le bas
    }
    if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT]) {
        move_x = -1.0f;  // Vers la gauche
    }
    if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT]) {
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
