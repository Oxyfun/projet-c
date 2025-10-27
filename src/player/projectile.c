#include "projectile.h"
#include "../utils/constants.h"
#include <stdio.h>

// Initialisation d'un projectile
void projectile_init(Projectile* proj, float x, float y, int direction, float speed, float damage, SDL_Texture* shared_texture) {
    proj->x = x;
    proj->y = y;
    proj->damage = damage;
    proj->active = true;
    proj->lifetime = 0.0f;
    proj->max_lifetime = 3.0f;
    proj->size = 20.0f;
    proj->angle = 0.0f;
    proj->angle_speed = 200.0f;

    // Utiliser la texture partagée (chargée une seule fois dans main.c)
    proj->texture = shared_texture;

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
    if (proj->x < 0 || proj->x > WINDOW_WIDTH || proj->y < 0 || proj->y > WINDOW_HEIGHT) {
        proj->active = false;
    }

    //rotation tir
    proj->angle += proj->angle_speed * dt;
}

// Rendu d'un projectile
void projectile_render(SDL_Renderer* r, Projectile* proj) {
    // Rectangle du projectile
    SDL_Rect rect = {
        (int)(proj->x - proj->size / 2), // Centrer le projectile
        (int)(proj->y - proj->size / 2),
        (int)proj->size,
        (int)proj->size
    };

    // Rendu de la texture si disponible
    if (proj->texture != NULL)
    {
        SDL_Point center = { rect.w / 2, rect.h / 2 };

        SDL_RenderCopyEx(r, proj->texture, NULL, &rect, proj->angle, &center, SDL_FLIP_NONE);
    }
    else {
        // Fallback : carré jaune si pas de texture
        SDL_SetRenderDrawColor(r, 255, 255, 0, 255);
        SDL_RenderFillRect(r, &rect);
    }
}

// Nettoyage d'un projectile
void projectile_cleanup(Projectile* proj) {
    // Ne pas détruire la texture car elle est partagée entre tous les projectiles
    // Elle sera détruite une seule fois dans main.c
    proj->texture = NULL;
    proj->active = false;
}

