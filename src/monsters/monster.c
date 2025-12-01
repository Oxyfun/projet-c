#include "monster.h"
#include "../utils/constants.h"
#include "../utils/assets.h"
#include "../levels/room.h"

void monster_init(Monster* m, float x, float y, SDL_Texture* tex_up, SDL_Texture* tex_down, SDL_Texture* tex_left, SDL_Texture* tex_right) {
	m->w = 48.0f;
	m->h = 48.0f;
	m->x = x;
	m->y = y;
	m->vx = 0.0f;
	m->vy = 0.0f;

	m->alive = true;

	m->health = 5;

	m->current_health = m->health;

	m->speed = 80.0f;
    m->direction = 0;

    // Assignation des textures
    m->texture_up = tex_up;
    m->texture_down = tex_down;
    m->texture_left = tex_left;
    m->texture_right = tex_right;

    // Texture par défaut
    m->current_texture = m->texture_down;
}

void monster_render(SDL_Renderer* r, Monster* m)
{
    if (!m->alive) return;

	SDL_Rect rect = {
		(int)m->x,
		(int)m->y,
		(int)m->w,
		(int)m->h };

    if (m->current_texture != NULL) {
        SDL_RenderCopy(r, m->current_texture, NULL, &rect);
    } else {
        // Fallback carré rouge si pas de texture
        SDL_SetRenderDrawColor(r, 255, 0, 0, 255);
        SDL_RenderFillRect(r, &rect);
    }
}


void monster_follow(Monster* m, float player_x, float player_y, float dt, const Room* room, const SDL_Rect* room_rect)
{
	if (m->alive == false) return;

	float dx = player_x - m->x;
	float dy = player_y - m->y;
	float length = sqrtf(dx * dx + dy * dy);

	if (length > 0.0f) {
		dx /= length;
		dy /= length;
	}

    // Calcul du mouvement prévu
	float move_x = dx * m->speed * dt;
	float move_y = dy * m->speed * dt;

    // Application avec collision (axe X)
    if (room != NULL && room_rect != NULL) {
        float next_x = m->x + move_x;
        if (!room_check_collision(room, room_rect, next_x, m->y, m->w, m->h)) {
            m->x = next_x;
        }

        // Application avec collision (axe Y)
        float next_y = m->y + move_y;
        if (!room_check_collision(room, room_rect, m->x, next_y, m->w, m->h)) {
            m->y = next_y;
        }
    } else {
        // Fallback sans collision si room non fournie
        m->x += move_x;
        m->y += move_y;
    }

    // Mise à jour de la direction et de la texture
    if (fabs(dx) > fabs(dy)) {
        if (dx > 0) {
            m->direction = 3; // Droite
            m->current_texture = m->texture_right;
        } else {
            m->direction = 2; // Gauche
            m->current_texture = m->texture_left;
        }
    } else {
        if (dy > 0) {
            m->direction = 0; // Bas
            m->current_texture = m->texture_down;
        } else {
            m->direction = 1; // Haut
            m->current_texture = m->texture_up;
        }
    }
}
