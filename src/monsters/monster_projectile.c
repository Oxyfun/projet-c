#include "monster_projectile.h"
#include "../utils/constants.h"
#include <stdio.h>

void monster_projectile_init(MonsterProjectile* proj, float x, float y, float vx, float vy, float damage, SDL_Texture* texture) {
    proj->x = x;
    proj->y = y;
    proj->vx = vx;
    proj->vy = vy;
    proj->damage = damage;
    proj->active = true;
    proj->lifetime = 0.0f;
    proj->max_lifetime = 4.0f;
    proj->size = 20.0f;
    proj->angle = 0.0f;
    proj->angle_speed = 200.0f;
    proj->texture = texture;
}

void monster_projectile_update(MonsterProjectile* proj, float dt) {
    if (!proj->active) return;

    proj->x += proj->vx * dt;
    proj->y += proj->vy * dt;

    proj->lifetime += dt;

    if (proj->lifetime >= proj->max_lifetime) {
        proj->active = false;
    }

    if (proj->x < 0 || proj->x > WINDOW_WIDTH || proj->y < 0 || proj->y > WINDOW_HEIGHT) {
        proj->active = false;
    }

    proj->angle += proj->angle_speed * dt;
}

void monster_projectile_render(SDL_Renderer* r, MonsterProjectile* proj) {
    if (!proj->active) return;

    SDL_Rect rect = {
        (int)(proj->x - proj->size / 2),
        (int)(proj->y - proj->size / 2),
        (int)proj->size,
        (int)proj->size
    };

    if (proj->texture != NULL) {
        SDL_Point center = { rect.w / 2, rect.h / 2 };
        SDL_RenderCopyEx(r, proj->texture, NULL, &rect, proj->angle, &center, SDL_FLIP_NONE);
    } else {
        SDL_SetRenderDrawColor(r, 255, 255, 0, 255);
        SDL_RenderFillRect(r, &rect);
    }
}

void monster_projectile_cleanup(MonsterProjectile* proj) {
    proj->texture = NULL;
    proj->active = false;
}
