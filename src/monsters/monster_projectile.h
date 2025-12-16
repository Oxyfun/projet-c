#ifndef MONSTER_PROJECTILE_H
#define MONSTER_PROJECTILE_H

#include "../utils/sdl_common.h"
#include <stdbool.h>
#include <math.h>

// Structure pour les projectiles des monstres
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
} MonsterProjectile;

// Fonctions pour les projectiles des monstres
void monster_projectile_init(MonsterProjectile* proj, float x, float y, float vx, float vy, float damage, SDL_Texture* texture);
void monster_projectile_update(MonsterProjectile* proj, float dt);
void monster_projectile_render(SDL_Renderer* r, MonsterProjectile* proj);
void monster_projectile_cleanup(MonsterProjectile* proj);

#endif
