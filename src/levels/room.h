#ifndef ROOM_H
#define ROOM_H

#include <stdbool.h>
#include "../utils/sdl_common.h"

#define ROOM_ROWS 8
#define ROOM_COLS 12
#define ROOM_CELL_SIZE 64
#define ROOMS_DIRECTORY "rooms"

typedef enum {
    TILE_EMPTY = 0,
    TILE_FLOOR = 1 << 0,
    TILE_ROCK = 1 << 1,
    TILE_DOOR = 1 << 2,
    TILE_CHEST = 1 << 3,
    TILE_MONSTER_SPAWN = 1 << 4
} TileType;

typedef struct {
    TileType tiles[ROOM_ROWS][ROOM_COLS];
    int rows;
    int cols;
} Room;

void room_init(Room* room);
void room_fill(Room* room, TileType tile);
void room_set_tile(Room* room, int row, int col, TileType tile);
void room_add_tile(Room* room, int row, int col, TileType tile);
void room_remove_tile(Room* room, int row, int col, TileType tile);
bool room_tile_has(const Room* room, int row, int col, TileType tile);
bool room_tile_is_blocking(const Room* room, int row, int col);
bool room_check_collision(const Room* room, const SDL_Rect* room_rect, float x, float y, float w, float h);
bool room_save_csv(const Room* room, const char* path);
bool room_load_csv(Room* room, const char* path);
bool room_load_random(Room* room, const char* directory);
int room_collect_csv_files(const char* directory, char files[][400], int max_files);

#endif


