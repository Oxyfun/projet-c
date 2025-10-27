#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "../utils/sdl_common.h"
#include <stdbool.h>
#include <math.h>

// Structure pour les projectiles
typedef struct {
    // Position et mouvement
    float x, y;
    float vx, vy;

    // Stats du projectile
    float damage;
    float lifetime;
    float max_lifetime;

    // État
    bool active;

    // Visuel
    float size;
    SDL_Texture* texture;

    // Rotation du tir
    float angle;
    float angle_speed;
} Projectile;

// Fonctions pour les projectiles
void projectile_init(Projectile* proj, float x, float y, int direction, float speed, float damage, SDL_Texture* shared_texture);
void projectile_update(Projectile* proj, float dt);
void projectile_render(SDL_Renderer* r, Projectile* proj);
void projectile_cleanup(Projectile* proj);

#endif

