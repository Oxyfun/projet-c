#include "player.h"
#include "../utils/assets.h"
#include "../utils/constants.h"
#include <stdio.h>
#include "../levels/room.h"

static bool player_world_to_cell(const SDL_Rect* room_rect, float x, float y, int* out_row, int* out_col) {
    if (room_rect == NULL) {
        return false;
    }

    float local_x = x - (float)room_rect->x;
    float local_y = y - (float)room_rect->y;

    if (local_x < 0.0f || local_y < 0.0f) {
        return false;
    }

    int col = (int)(local_x / ROOM_CELL_SIZE);
    int row = (int)(local_y / ROOM_CELL_SIZE);

    if (col < 0 || col >= ROOM_COLS || row < 0 || row >= ROOM_ROWS) {
        return false;
    }

    if (out_col != NULL) {
        *out_col = col;
    }
    if (out_row != NULL) {
        *out_row = row;
    }

    return true;
}

static bool player_collides_with_room(Player* p, const Room* room, const SDL_Rect* room_rect, float test_x, float test_y) {
    if (room == NULL || room_rect == NULL) {
        return false;
    }

    float margin = 4.0f;
    float sample_points[4][2] = {
        { test_x + margin, test_y + margin },
        { test_x + p->w - margin, test_y + margin },
        { test_x + margin, test_y + p->h - margin },
        { test_x + p->w - margin, test_y + p->h - margin }
    };

    for (int i = 0; i < 4; i++) {
        int row = 0;
        int col = 0;

        if (!player_world_to_cell(room_rect, sample_points[i][0], sample_points[i][1], &row, &col)) {
            return true;
        }

        if (room_tile_is_blocking(room, row, col)) {
            return true;
        }
    }

    return false;
}

// Initialisation du joueur
void player_init(Player* p, SDL_Renderer* renderer) {
    p->w = 48.0f;
    p->h = 48.0f;
    p->x = (float)WINDOW_WIDTH / 2.0f - p->w / 2.0f;
    p->y = (float)WINDOW_HEIGHT / 2.0f - p->h / 2.0f;
    p->vx = 0.0f;
    p->vy = 0.0f;
    p->direction = 0;
    
    // Stats du joueur
    p->speed = 200.0f; // Vitesse de déplacement
    
    // Stats de tir
    p->projectile_damage = 10.0f; // Dégâts des projectiles
    p->fire_rate = 2.0f; // 2 projectiles par seconde
    p->fire_interval = 1.0f / p->fire_rate;
    p->last_shot_time = 0.0f;
    p->projectile_speed = 300.0f; // Vitesse des projectiles
    
    // Chargement des textures
    p->texture_up = load_texture(renderer, "assets/images/personnages/personnage_haut.png");
    p->texture_down = load_texture(renderer, "assets/images/personnages/personnage_bas.png");
    p->texture_left = load_texture(renderer, "assets/images/personnages/personnage_gauche.png");
    p->texture_right = load_texture(renderer, "assets/images/personnages/personnage_droite.png");
    
    // Texture par défaut
    p->current_texture = p->texture_down;
}

// Mise à jour du joueur
void player_update(Player* p, const Uint8* keys, float dt, float current_time, Projectile* projectiles, int max_projectiles, SDL_Texture* projectile_texture, const Room* room, const SDL_Rect* room_rect) {
    // Variables pour les directions
    float move_x = 0.0f;
    float move_y = 0.0f;
    
    // Gestion des touches
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
    
    // Mise à jour position + collisions sur la grille
    if (room != NULL && room_rect != NULL) {
        float new_x = p->x + p->vx * dt;
        float new_y = p->y + p->vy * dt;

        if (!player_collides_with_room(p, room, room_rect, new_x, p->y)) {
            p->x = new_x;
        } else {
            p->vx = 0.0f;
        }

        if (!player_collides_with_room(p, room, room_rect, p->x, new_y)) {
            p->y = new_y;
        } else {
            p->vy = 0.0f;
        }
    } else {
        p->x += p->vx * dt;
        p->y += p->vy * dt;
    }
    
    // Limites écran si pas de grille
    if (room_rect == NULL) {
        if (p->x < 0) p->x = 0;
        if (p->x + p->w > WINDOW_WIDTH) p->x = WINDOW_WIDTH - p->w;
        if (p->y < 0) p->y = 0;
        if (p->y + p->h > WINDOW_HEIGHT) p->y = WINDOW_HEIGHT - p->h;
    } else {
        float min_x = (float)room_rect->x;
        float max_x = (float)(room_rect->x + room_rect->w) - p->w;
        float min_y = (float)room_rect->y;
        float max_y = (float)(room_rect->y + room_rect->h) - p->h;

        if (p->x < min_x) p->x = min_x;
        if (p->x > max_x) p->x = max_x;
        if (p->y < min_y) p->y = min_y;
        if (p->y > max_y) p->y = max_y;
    }
    
    // Gestion du tir avec les flèches directionnelles
    int shoot_direction = -1;
    if (keys[SDL_SCANCODE_DOWN]) {
        shoot_direction = 0; // Bas
        p->current_texture = p->texture_down;
    } else if (keys[SDL_SCANCODE_UP]) {
        shoot_direction = 1; // Haut
        p->current_texture = p->texture_up;
    } else if (keys[SDL_SCANCODE_LEFT]) {
        shoot_direction = 2; // Gauche
        p->current_texture = p->texture_left;
    } else if (keys[SDL_SCANCODE_RIGHT]) {
        shoot_direction = 3; // Droite
        p->current_texture = p->texture_right;
    }
    
    // Si une touche de tir est pressée et qu'on peut tirer
    if (shoot_direction != -1 && player_can_shoot(p, current_time)) {
        // Chercher un projectile inactif
        for (int i = 0; i < max_projectiles; i++) {
            if (!projectiles[i].active) {
                player_shoot(p, &projectiles[i], shoot_direction, projectile_texture, current_time);
                break; // On tire un seul projectile
            }
        }
    }
}

// Rendu du joueur
void player_render(SDL_Renderer* r, Player* p) {
    SDL_Rect rect = {
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
    return time_since_last_shot >= p->fire_interval;
}

// Fonction pour faire tirer le joueur
void player_shoot(Player* p, Projectile* projectile, int direction, SDL_Texture* projectile_texture, float current_time) {
    // Position de départ du projectile (centre du joueur)
    float start_x = p->x + p->w / 2.0f;
    float start_y = p->y + p->h / 2.0f;
    
    // Initialiser le projectile avec la texture partagée
    projectile_init(projectile, start_x, start_y, direction, p->projectile_speed, p->projectile_damage, projectile_texture);
    
    // Mettre à jour le temps du dernier tir
    p->last_shot_time = current_time;
}

