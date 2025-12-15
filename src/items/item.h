#ifndef ITEM_H
#define ITEM_H

#include "../utils/sdl_common.h"
#include "../player/player.h"
#include <stdbool.h>

typedef enum {
    STAT_MAX_HEALTH,
    STAT_SPEED,
    STAT_DAMAGE,
    STAT_FIRE_RATE,
    STAT_PROJECTILE_SPEED,
    STAT_COUNT
} ItemStatType;

typedef struct {
    char name[64];
    char description[256];
    char sprite_path[256];
    SDL_Texture* texture;
    
    float stat_modifiers[STAT_COUNT];
    
    float x, y;
    float w, h;
    bool is_collected;
} Item;

typedef struct {
    Item* items;
    int count;
    int capacity;
} ItemManager;

ItemManager* item_manager_create(int initial_capacity);
void item_manager_destroy(ItemManager* manager);

Item* item_load_from_csv(const char* item_folder, SDL_Renderer* renderer);
bool item_manager_add(ItemManager* manager, Item* item);
int item_manager_load_all_from_directory(ItemManager* manager, const char* directory, SDL_Renderer* renderer);

void item_apply_to_player(const Item* item, Player* player);
void item_render(SDL_Renderer* renderer, const Item* item);
bool item_check_collision(const Item* item, const Player* player);
void item_cleanup(Item* item);

const char* item_stat_type_to_string(ItemStatType type);
ItemStatType item_string_to_stat_type(const char* str);

#endif
