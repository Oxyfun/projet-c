#ifndef PLAYER_H
#define PLAYER_H

#include "../utils/sdl_common.h"
#include <stdbool.h>
#include <math.h>
#include "projectile.h"

// Structure joueur
typedef struct {
    // Position
    float x, y;
    
    // Vitesse
    float vx, vy;
    
    // Dimensions
    float w, h;
    
    // Stats du joueur
    float speed; // Vitesse de déplacement
    float max_x, max_y; // Limites de l'écran (pour éviter les valeurs en dur)
    
    // Stats de tir
    float projectile_damage; // Dégâts des projectiles
    float fire_rate; // Cadence de tir (projectiles par seconde)
    float last_shot_time; // Temps du dernier tir (pour la cadence)
    float projectile_speed; // Vitesse des projectiles
    
    // État
    bool alive;
    
    // Textures pour les différentes directions
    SDL_Texture* texture_up;
    SDL_Texture* texture_down;
    SDL_Texture* texture_left;
    SDL_Texture* texture_right;
    SDL_Texture* current_texture;
    
    // Direction actuelle
    int direction; // 0=bas, 1=haut, 2=gauche, 3=droite
} Player;

// Fonctions essentielles
void player_init(Player* p, SDL_Renderer* renderer);
void player_update(Player* p, const Uint8* keys, float dt, Projectile* projectiles, int max_projectiles, SDL_Renderer* renderer);
void player_render(SDL_Renderer* r, Player* p);
void player_cleanup(Player* p);

// Fonctions de tir
void player_shoot(Player* p, Projectile* projectile, int direction, SDL_Renderer* renderer);
bool player_can_shoot(Player* p, float current_time);

#endif

