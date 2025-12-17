#define _CRT_SECURE_NO_WARNINGS
#include "dungeon.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../utils/constants.h"

static int dungeon_analyze_doors(const Room* room) {
    int mask = DOOR_NONE;

    for (int c = 0; c < ROOM_COLS; c++) {
        if (room_tile_has(room, 0, c, TILE_DOOR)) {
            mask |= DOOR_UP;
            break;
        }
    }

    for (int c = 0; c < ROOM_COLS; c++) {
        if (room_tile_has(room, ROOM_ROWS - 1, c, TILE_DOOR)) {
            mask |= DOOR_DOWN;
            break;
        }
    }

    for (int r = 0; r < ROOM_ROWS; r++) {
        if (room_tile_has(room, r, 0, TILE_DOOR)) {
            mask |= DOOR_LEFT;
            break;
        }
    }

    for (int r = 0; r < ROOM_ROWS; r++) {
        if (room_tile_has(room, r, ROOM_COLS - 1, TILE_DOOR)) {
            mask |= DOOR_RIGHT;
            break;
        }
    }

    return mask;
}

void dungeon_init(Dungeon* dungeon) {
    if (dungeon == NULL) return;
    memset(dungeon, 0, sizeof(Dungeon));
    dungeon->current_map_x = DUNGEON_MAP_SIZE / 2;
    dungeon->current_map_y = DUNGEON_MAP_SIZE / 2;
}

void dungeon_load_templates(Dungeon* dungeon, const char* directory) {
    char files[MAX_TEMPLATES][400];
    int count = room_collect_csv_files(directory, files, MAX_TEMPLATES);

    dungeon->template_count = 0;
    for (int i = 0; i < count; i++) {
        RoomTemplate* tmpl = &dungeon->templates[dungeon->template_count];
        room_init(&tmpl->room);
        
        if (room_load_csv(&tmpl->room, files[i])) {
            tmpl->door_mask = dungeon_analyze_doors(&tmpl->room);
            strcpy(tmpl->filename, files[i]);
            dungeon->template_count++;
        }
    }
}

static RoomTemplate* find_matching_template(Dungeon* dungeon, int required_mask) {
    int indices[MAX_TEMPLATES];
    for (int i = 0; i < dungeon->template_count; i++) indices[i] = i;
    for (int i = 0; i < dungeon->template_count; i++) {
        int j = rand() % dungeon->template_count;
        int temp = indices[i];
        indices[i] = indices[j];
        indices[j] = temp;
    }

    for (int i = 0; i < dungeon->template_count; i++) {
        int idx = indices[i];
        if (dungeon->templates[idx].door_mask == required_mask) {
            return &dungeon->templates[idx];
        }
    }

    for (int i = 0; i < dungeon->template_count; i++) {
        int idx = indices[i];
        if ((dungeon->templates[idx].door_mask & required_mask) == required_mask) {
            return &dungeon->templates[idx];
        }
    }

    return NULL;
}

void dungeon_generate(Dungeon* dungeon) {
    memset(dungeon->grid, 0, sizeof(dungeon->grid));
    memset(dungeon->has_room, 0, sizeof(dungeon->has_room));

    typedef struct { int x; int y; } Point;
    Point occupied_points[MAX_ROOMS];
    int room_count = 0;

    // depart au centre
    int cx = DUNGEON_MAP_SIZE / 2;
    int cy = DUNGEON_MAP_SIZE / 2;
    
    occupied_points[0] = (Point){cx, cy};
    room_count = 1;
    dungeon->has_room[cx][cy] = true;

    int target_rooms = MIN_ROOMS + rand() % (MAX_ROOMS - MIN_ROOMS + 1);

    int head_x = cx;
    int head_y = cy;

    while (room_count < target_rooms) {
        int dir = rand() % 4;
        int nx = head_x;
        int ny = head_y;

        switch (dir) {
            case 0: ny--; break;
            case 1: ny++; break;
            case 2: nx--; break;
            case 3: nx++; break;
        }

        if (nx >= 0 && nx < DUNGEON_MAP_SIZE && ny >= 0 && ny < DUNGEON_MAP_SIZE) {
            if (!dungeon->has_room[nx][ny]) {
                dungeon->has_room[nx][ny] = true;
                occupied_points[room_count] = (Point){nx, ny};
                room_count++;
            }
            head_x = nx;
            head_y = ny;
        }
    }

    for (int i = 0; i < room_count; i++) {
        int x = occupied_points[i].x;
        int y = occupied_points[i].y;
        
        int required_mask = DOOR_NONE;

        // verifier les voisins pour savoir ou mettre les portes
        if (y > 0 && dungeon->has_room[x][y-1]) required_mask |= DOOR_UP;
        if (y < DUNGEON_MAP_SIZE - 1 && dungeon->has_room[x][y+1]) required_mask |= DOOR_DOWN;
        if (x > 0 && dungeon->has_room[x-1][y]) required_mask |= DOOR_LEFT;
        if (x < DUNGEON_MAP_SIZE - 1 && dungeon->has_room[x+1][y]) required_mask |= DOOR_RIGHT;

        // trouver un template
        RoomTemplate* tmpl = find_matching_template(dungeon, required_mask);
        
        if (tmpl != NULL) {
            dungeon->grid[x][y] = tmpl->room;
        } else {
            room_init(&dungeon->grid[x][y]);
            if (required_mask & DOOR_UP) room_set_tile(&dungeon->grid[x][y], 0, ROOM_COLS/2, TILE_DOOR);
            if (required_mask & DOOR_DOWN) room_set_tile(&dungeon->grid[x][y], ROOM_ROWS-1, ROOM_COLS/2, TILE_DOOR);
            if (required_mask & DOOR_LEFT) room_set_tile(&dungeon->grid[x][y], ROOM_ROWS/2, 0, TILE_DOOR);
            if (required_mask & DOOR_RIGHT) room_set_tile(&dungeon->grid[x][y], ROOM_ROWS/2, ROOM_COLS-1, TILE_DOOR);
        }
    }

    // Verifier si il y a au moins une room avec un boss
    bool has_boss = false;
    for (int i = 0; i < room_count; i++) {
        int x = occupied_points[i].x;
        int y = occupied_points[i].y;
        for (int r = 0; r < ROOM_ROWS; r++) {
            for (int c = 0; c < ROOM_COLS; c++) {
                if (room_tile_has(&dungeon->grid[x][y], r, c, TILE_MONSTER_SPAWN_BOSS)) {
                    has_boss = true;
                    break;
                }
            }
            if (has_boss) break;
        }
        if (has_boss) break;
    }

    if (!has_boss) {
        printf("Aucune room avec un boss trouvee\n");
    }

    dungeon->current_map_x = cx;
    dungeon->current_map_y = cy;
}

Room* dungeon_get_current_room(Dungeon* dungeon) {
    if (!dungeon->has_room[dungeon->current_map_x][dungeon->current_map_y]) {
        return NULL;
    }
    return &dungeon->grid[dungeon->current_map_x][dungeon->current_map_y];
}

bool dungeon_try_move(Dungeon* dungeon, int direction, float* player_x, float* player_y) {
    int dx = 0; 
    int dy = 0;

    // 0=Haut, 1=Bas, 2=Gauche, 3=Droite
    switch (direction) {
        case 0: dy = -1; break; // haut
        case 1: dy = 1; break;  // bas
        case 2: dx = -1; break; // gauche
        case 3: dx = 1; break;  // droite
    }

    int next_map_x = dungeon->current_map_x + dx;
    int next_map_y = dungeon->current_map_y + dy;

    if (next_map_x >= 0 && next_map_x < DUNGEON_MAP_SIZE &&
        next_map_y >= 0 && next_map_y < DUNGEON_MAP_SIZE &&
        dungeon->has_room[next_map_x][next_map_y]) {
        
        dungeon->current_map_x = next_map_x;
        dungeon->current_map_y = next_map_y;

        float pw = 48.0f;
        float ph = 48.0f;

        if (direction == 0) {
            *player_y = (ROOM_ROWS - 2) * ROOM_CELL_SIZE + 16;
            *player_x = (ROOM_COLS / 2) * ROOM_CELL_SIZE + 8; 
        } else if (direction == 1) {
            *player_y = 1 * ROOM_CELL_SIZE + 16;
            *player_x = (ROOM_COLS / 2) * ROOM_CELL_SIZE + 8;
        } else if (direction == 2) {
            *player_x = (ROOM_COLS - 2) * ROOM_CELL_SIZE + 16;
            *player_y = (ROOM_ROWS / 2) * ROOM_CELL_SIZE + 8;
        } else if (direction == 3) {
            *player_x = 1 * ROOM_CELL_SIZE + 16;
            *player_y = (ROOM_ROWS / 2) * ROOM_CELL_SIZE + 8;
        }

        return true;
    }

    return false;
}

