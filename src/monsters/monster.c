#include "monster.h"
#include "../utils/constants.h"

void monster_init(Monster* m, SDL_Renderer* renderer) {
    m->w = 48.0f;
    m->h = 48.0f;
    m->x = (float)WINDOW_WIDTH / 2.0f - m->w / 2.0f;
    m->y = (float)WINDOW_HEIGHT / 2.0f - m->h / 2.0f;
    m->vx = 0.0f;
    m->vy = 0.0f;
}

void monster_render(SDL_Renderer* r, Monster* m)
{


}


void monster_follow()
{


}
;

