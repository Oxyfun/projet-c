#include "../utils/sdl_common.h"
#include <math.h>

typedef struct {

	float x, y;

	float vx, vy;

	float w, h;

	float speed;

	float health;

	float current_health;

	float damage;
} Monster;

void monster_init(Monster* m, SDL_Renderer* renderer);
void monster_render(SDL_Renderer* r, Monster* m);
void monster_follow();