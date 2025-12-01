#include "../utils/sdl_common.h"
#include "../levels/room.h"
#include <math.h>
#include <stdbool.h>

typedef struct {

	float x, y;

	float vx, vy;

	float w, h;

	float speed;

	float health;

	float current_health;

	float damage;

	bool alive;

	SDL_Texture* texture_up;
	SDL_Texture* texture_down;
	SDL_Texture* texture_left;
	SDL_Texture* texture_right;
	SDL_Texture* current_texture;

	int direction; // 0=bas, 1=haut, 2=gauche, 3=droite
} Monster;

void monster_init(Monster* m, float x, float y, SDL_Texture* tex_up, SDL_Texture* tex_down, SDL_Texture* tex_left, SDL_Texture* tex_right);
void monster_render(SDL_Renderer* r, Monster* m);
void monster_follow(Monster* m, float player_x, float player_y, float dt, const Room* room, const SDL_Rect* room_rect);