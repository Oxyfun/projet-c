#ifndef DUNGEON_H
#define DUNGEON_H

#include "room.h"
#include "../utils/sdl_common.h"

#define MAX_TEMPLATES 50
#define DUNGEON_MAP_SIZE 10
#define MIN_ROOMS 8
#define MAX_ROOMS 12

typedef enum {
    DOOR_NONE = 0,
    DOOR_UP = 1 << 0,
    DOOR_DOWN = 1 << 1,
    DOOR_LEFT = 1 << 2,
    DOOR_RIGHT = 1 << 3
} DoorMask;

typedef struct {
    Room room;
    int door_mask;
    char filename[400];
} RoomTemplate;

typedef struct {
    RoomTemplate templates[MAX_TEMPLATES];
    int template_count;
    
    Room grid[DUNGEON_MAP_SIZE][DUNGEON_MAP_SIZE];
    bool has_room[DUNGEON_MAP_SIZE][DUNGEON_MAP_SIZE];
    
    int current_map_x;
    int current_map_y;
} Dungeon;

void dungeon_init(Dungeon* dungeon);
void dungeon_load_templates(Dungeon* dungeon, const char* directory);
void dungeon_generate(Dungeon* dungeon);
Room* dungeon_get_current_room(Dungeon* dungeon);
bool dungeon_try_move(Dungeon* dungeon, int direction, float* player_x, float* player_y);

#endif

