#include "monster.h"
#include "../utils/constants.h"
#include "../utils/assets.h"

void monster_init(Monster* m, SDL_Renderer* renderer) {
	m->w = 48.0f;
	m->h = 48.0f;
	m->x = (float)WINDOW_WIDTH / 5.0f - m->w / 2.0f;
	m->y = (float)WINDOW_HEIGHT / 5.0f - m->h / 2.0f;
	m->vx = 0.0f;
	m->vy = 0.0f;

	m->alive = true;

	m->health = 5;

	m->current_health = m->health;

	m->speed = 80.0f;
    m->direction = 0;

    // Chargement des textures
    m->texture_up = load_texture(renderer, "assets/images/monstre/monstre_haut.png");
    m->texture_down = load_texture(renderer, "assets/images/monstre/monstre_bas.png");
    m->texture_left = load_texture(renderer, "assets/images/monstre/monstre_gauche.png");
    m->texture_right = load_texture(renderer, "assets/images/monstre/monstre_droite.png");

    // Texture par défaut
    m->current_texture = m->texture_down;
}

void monster_render(SDL_Renderer* r, Monster* m)
{
	SDL_Rect rect = {
		(int)m->x,
		(int)m->y,
		(int)m->w,
		(int)m->h };

	if (m->alive)
	{
        if (m->current_texture != NULL) {
            SDL_RenderCopy(r, m->current_texture, NULL, &rect);
        } else {
            // Fallback carré rouge si pas de texture
            SDL_SetRenderDrawColor(r, 255, 0, 0, 255);
            SDL_RenderFillRect(r, &rect);
        }
	}

}


void monster_follow(Monster* m, float player_x, float player_y, float dt)
{
	if (m->alive == false) return;

	float dx = player_x - m->x;
	float dy = player_y - m->y;
	float length = sqrtf(dx * dx + dy * dy);

	if (length > 0.0f) {
		dx /= length;
		dy /= length;
	}

	m->x += dx * m->speed * dt;
	m->y += dy * m->speed * dt;

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
