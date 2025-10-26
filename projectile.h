#include <SDL.h>
#include <SDL_image.h>
#include <math.h> 
#include "stdbool.h"


// Structure pour les projectiles
typedef struct {
    // Position et mouvement
    float x, y;             // Position actuelle
    float vx, vy;           // Vitesse (direction)
    float speed;            // Vitesse de déplacement

    // Stats du projectile
    float damage;           // Dégâts infligés
    float lifetime;         // Durée de vie (en secondes)
    float max_lifetime;     // Durée de vie maximale

    // État
    bool active;            // Si le projectile est actif
    int direction;          // Direction du projectile (0=bas, 1=haut, 2=gauche, 3=droite)

    // Visuel
    float size;             // Taille du projectile
    SDL_Texture* texture;   // Texture du projectile


    // Rotation du tir
    float angle;
    float angle_speed;
    
} Projectile;

// Fonctions pour les projectiles
void projectile_init(Projectile* proj, float x, float y, int direction, float speed, float damage, SDL_Renderer* renderer);
void projectile_update(Projectile* proj, float dt);
void projectile_render(SDL_Renderer* r, Projectile* proj);
void projectile_cleanup(Projectile* proj);