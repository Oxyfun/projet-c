#include "monster.h"
#include "../utils/constants.h"

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
		SDL_SetRenderDrawColor(r, 0, 100, 255, 255);
		SDL_RenderFillRect(r, &rect);
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
}

