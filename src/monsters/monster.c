#include "monster.h"
#include "../utils/constants.h"
#include "../utils/assets.h"
#include "../levels/room.h"
#include <stdio.h>

static void monster_apply_type_stats(Monster* m, MonsterType type) {
	switch (type) {
		case MONSTER_TYPE_BASIC:
			m->speed = 80.0f;
			m->health = 5.0f;
			m->damage = 2.0f;
			break;
		case MONSTER_TYPE_TANK:
			m->speed = 50.0f;    
			m->health = 12.0f;   
			m->damage = 3.0f;   
			break;
        case MONSTER_TYPE_SHOOTER:
            m->speed = 70.0f;
            m->health = 6.0f;
            m->damage = 1.0f;
            m->fire_interval = 1.2f; // tir toutes les 1.2s
            m->projectile_speed = 180.0f;
            m->last_shot_time = 0.0f;
            break;
        case MONSTER_TYPE_BOSS:
            m->speed = 40.0f;
            m->health = 50.0f;
            m->damage = 3.0f;
            m->fire_interval = 0.8f; // tir plus rapide
            m->projectile_speed = 200.0f;
            m->last_shot_time = 0.0f;
            break;
		
	}
}

void monster_init(Monster* m, float x, float y, MonsterType type, SDL_Texture* tex_up, SDL_Texture* tex_down, SDL_Texture* tex_left, SDL_Texture* tex_right, SDL_Texture* projectile_tex) {
	m->w = 48.0f;
	m->h = 48.0f;

	if (type == MONSTER_TYPE_BOSS) {
		m->w = 96.0f;
		m->h = 96.0f;
	}

	m->x = x;
	m->y = y;
	m->vx = 0.0f;
	m->vy = 0.0f;

	m->alive = true;

	m->type = type;
	monster_apply_type_stats(m, type);

	m->current_health = m->health;

    m->direction = 0;

    // Assignation des textures
    m->texture_up = tex_up;
    m->texture_down = tex_down;
    m->texture_left = tex_left;
    m->texture_right = tex_right;

    // Texture par défaut
    m->current_texture = m->texture_down;

    // Texture du projectile
    m->projectile_texture = projectile_tex;
}

void monster_render(SDL_Renderer* r, Monster* m)
{
    if (!m->alive) return;

	SDL_Rect rect = {
		(int)m->x,
		(int)m->y,
		(int)m->w,
		(int)m->h };

    // afficher la texture
    if (m->current_texture != NULL) {
        SDL_RenderCopy(r, m->current_texture, NULL, &rect);
    } else {
        // Carre jaune si pas de texture
        SDL_SetRenderDrawColor(r, 255, 255, 0, 255);
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

    // Calcul du mouvement
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
        m->x += move_x;
        m->y += move_y;
    }

    // Mise à jour de la direction et de la texture
    if (fabs(dx) > fabs(dy)) {
        if (dx > 0) {
            m->direction = 3; 
            m->current_texture = m->texture_right;
        } else {
            m->direction = 2;
            m->current_texture = m->texture_left;
        }
    } else {
        if (m->texture_up == NULL && m->texture_down == NULL) {
            if (dx > 0) {
                m->direction = 3; 
                m->current_texture = m->texture_right;
            } else {
                m->direction = 2; 
                m->current_texture = m->texture_left;
            }
        } else {
            // Autres monstres: orientation basique
            if (dy > 0) {
                m->direction = 0; 
                m->current_texture = m->texture_down;
            } else {
                m->direction = 1; 
                m->current_texture = m->texture_up;
            }
        }
    }
}

void monster_update(Monster* m, float player_x, float player_y, float dt, const Room* room, const SDL_Rect* room_rect, MonsterProjectile* projectiles, int max_projectiles, SDL_Texture* projectile_texture, float current_time) {
    if (!m->alive) return;

    // comportement selon type de boss
    if (m->type == MONSTER_TYPE_SHOOTER || m->type == MONSTER_TYPE_BOSS) {
        // garder une distance de confort
        float dx = player_x - m->x;
        float dy = player_y - m->y;
        float dist = sqrtf(dx*dx + dy*dy);
        float desired_min = 150.0f;
        float desired_max = 300.0f;

        if (m->type == MONSTER_TYPE_BOSS) {
            desired_min = 200.0f;
            desired_max = 400.0f;
        }

        if (dist < desired_min) {
            // reculer
            float nx = -dx / dist;
            float ny = -dy / dist;
            float move_x = nx * m->speed * dt;
            float move_y = ny * m->speed * dt;
            // collision check
            if (!room_check_collision(room, room_rect, m->x + move_x, m->y, m->w, m->h)) m->x += move_x;
            if (!room_check_collision(room, room_rect, m->x, m->y + move_y, m->w, m->h)) m->y += move_y;
        } else if (dist > desired_max) {
            // avancer
            float nx = dx / dist;
            float ny = dy / dist;
            float move_x = nx * m->speed * dt;
            float move_y = ny * m->speed * dt;
            if (!room_check_collision(room, room_rect, m->x + move_x, m->y, m->w, m->h)) m->x += move_x;
            if (!room_check_collision(room, room_rect, m->x, m->y + move_y, m->w, m->h)) m->y += move_y;
        }

        // Met à jour l'orientation vers le joueur
        if (m->type == MONSTER_TYPE_SHOOTER) {
            if (dx > 0) {
                m->current_texture = m->texture_right;
            } else {
                m->current_texture = m->texture_left;
            }
        } else {
            if (fabs(dx) > fabs(dy)) {
                if (dx > 0) {
                    m->current_texture = m->texture_right;
                } else {
                    m->current_texture = m->texture_left;
                }
            } else {
                if (dy > 0) {
                    m->current_texture = m->texture_down;
                } else {
                    m->current_texture = m->texture_up;
                }
            }
        }

        // tir si cooldown ok
        if (current_time - m->last_shot_time >= m->fire_interval) {
            float shoot_dx = player_x - m->x;
            float shoot_dy = player_y - m->y;
            float shoot_dist = sqrtf(shoot_dx * shoot_dx + shoot_dy * shoot_dy);
            
            if (shoot_dist > 0.0f) {
                shoot_dx /= shoot_dist;
                shoot_dy /= shoot_dist;
            }
            
            // trouver emplacement projectile libre
            for (int i = 0; i < max_projectiles; i++) {
                if (!projectiles[i].active) {
                    // tirer vers le joueur
                    float angle = atan2f(shoot_dy, shoot_dx);
                    float speed = m->projectile_speed;
                    float vx = cosf(angle) * speed;
                    float vy = sinf(angle) * speed;
                    
                    // Utiliser la texture du projectile spécifique du monstre
                    SDL_Texture* proj_texture = (m->projectile_texture != NULL) ? m->projectile_texture : projectile_texture;
                    monster_projectile_init(&projectiles[i], m->x + m->w/2, m->y + m->h/2, vx, vy, 1.0f, proj_texture);
                    m->last_shot_time = current_time;
                    break;
                }
            }
        }
    } else {
        monster_follow(m, player_x, player_y, dt, room, room_rect);
    }
}
