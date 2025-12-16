#include "../utils/sdl_common.h"
#include "../levels/room.h"
#include "monster_projectile.h"
#include <math.h>
#include <stdbool.h>

typedef enum {
	MONSTER_TYPE_BASIC = 0,
	MONSTER_TYPE_TANK = 1,
	MONSTER_TYPE_SHOOTER = 2,
	MONSTER_TYPE_BOSS = 3
} MonsterType;

typedef struct {

	float x, y;

	float vx, vy;

	float w, h;

	float speed;

	float health;

	float current_health;

	float damage;

	bool alive;

	MonsterType type;

	SDL_Texture* texture_up;
	SDL_Texture* texture_down;
	SDL_Texture* texture_left;
	SDL_Texture* texture_right;
	SDL_Texture* current_texture;
    
    // Texture spécifique pour le projectile de ce monstre
    SDL_Texture* projectile_texture;

	int direction; // 0=bas, 1=haut, 2=gauche, 3=droite

	// Shooter-specific
	float last_shot_time;
	float fire_interval;
	float projectile_speed;
} Monster;

void monster_init(Monster* m, float x, float y, MonsterType type, SDL_Texture* tex_up, SDL_Texture* tex_down, SDL_Texture* tex_left, SDL_Texture* tex_right, SDL_Texture* projectile_tex);
void monster_render(SDL_Renderer* r, Monster* m);
void monster_follow(Monster* m, float player_x, float player_y, float dt, const Room* room, const SDL_Rect* room_rect);
void monster_update(Monster* m, float player_x, float player_y, float dt, const Room* room, const SDL_Rect* room_rect, MonsterProjectile* projectiles, int max_projectiles, SDL_Texture* projectile_texture, float current_time);