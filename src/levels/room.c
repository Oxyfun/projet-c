#define _CRT_SECURE_NO_WARNINGS
#include "room.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

static bool room_is_valid_index(int row, int col) {
    return (row >= 0 && row < ROOM_ROWS && col >= 0 && col < ROOM_COLS);
}

static bool room_world_to_cell(const SDL_Rect* room_rect, float x, float y, int* out_row, int* out_col) {
    if (room_rect == NULL) {
        return false;
    }

    float local_x = x - (float)room_rect->x;
    float local_y = y - (float)room_rect->y;

    if (local_x < 0.0f || local_y < 0.0f) {
        return false;
    }

    int col = (int)(local_x / ROOM_CELL_SIZE);
    int row = (int)(local_y / ROOM_CELL_SIZE);

    if (col < 0 || col >= ROOM_COLS || row < 0 || row >= ROOM_ROWS) {
        return false;
    }

    if (out_col != NULL) {
        *out_col = col;
    }
    if (out_row != NULL) {
        *out_row = row;
    }

    return true;
}

void room_init(Room* room) {
    if (room == NULL) {
        return;
    }

    room->rows = ROOM_ROWS;
    room->cols = ROOM_COLS;
    room_fill(room, TILE_FLOOR);
}

void room_fill(Room* room, TileType tile) {
    if (room == NULL) {
        return;
    }

    for (int r = 0; r < ROOM_ROWS; r++) {
        for (int c = 0; c < ROOM_COLS; c++) {
            room_set_tile(room, r, c, tile);
        }
    }
}

void room_set_tile(Room* room, int row, int col, TileType tile) {
    if (room == NULL || !room_is_valid_index(row, col)) {
        return;
    }

    TileType value = tile;

    if ((value & TILE_ROCK) != 0) {
        value |= TILE_FLOOR;
    }
    if ((value & TILE_DOOR) != 0) {
        value |= TILE_FLOOR;
    }
    if ((value & TILE_CHEST) != 0) {
        value |= TILE_FLOOR;
    }
    if ((value & (TILE_MONSTER_SPAWN_BASIC | TILE_MONSTER_SPAWN_TANK | TILE_MONSTER_SPAWN_SHOOTER | TILE_MONSTER_SPAWN_BOSS)) != 0) {
        value |= TILE_FLOOR;
    }
    if ((value & TILE_CHEST_OPENED) != 0) {
        value |= TILE_FLOOR;
    }
    if ((value & TILE_ITEM_COEUR) != 0) {
        value |= TILE_FLOOR;
    }
    if ((value & TILE_ITEM_PIMENT) != 0) {
        value |= TILE_FLOOR;
    }

    room->tiles[row][col] = value;
}

void room_add_tile(Room* room, int row, int col, TileType tile) {
    if (room == NULL || !room_is_valid_index(row, col)) {
        return;
    }

    if (tile == TILE_EMPTY) {
        room->tiles[row][col] = TILE_EMPTY;
        return;
    }

    TileType current = room->tiles[row][col];

    if ((tile & TILE_FLOOR) != 0) {
        current |= TILE_FLOOR;
    }

    if ((tile & TILE_ROCK) != 0) {
        current |= (TILE_ROCK | TILE_FLOOR);
    }

    if ((tile & TILE_DOOR) != 0) {
        current |= (TILE_DOOR | TILE_FLOOR);
    }

    if ((tile & TILE_CHEST) != 0) {
        current |= (TILE_CHEST | TILE_FLOOR);
    }

    if ((tile & (TILE_MONSTER_SPAWN_BASIC | TILE_MONSTER_SPAWN_TANK | TILE_MONSTER_SPAWN_SHOOTER | TILE_MONSTER_SPAWN_BOSS)) != 0) {
        current |= ((tile & (TILE_MONSTER_SPAWN_BASIC | TILE_MONSTER_SPAWN_TANK | TILE_MONSTER_SPAWN_SHOOTER | TILE_MONSTER_SPAWN_BOSS)) | TILE_FLOOR);
    }

    if ((tile & TILE_CHEST_OPENED) != 0) {
        current |= (TILE_CHEST_OPENED | TILE_FLOOR);
    }

    if ((tile & TILE_ITEM_COEUR) != 0) {
        current |= (TILE_ITEM_COEUR | TILE_FLOOR);
    }

    if ((tile & TILE_ITEM_PIMENT) != 0) {
        current |= (TILE_ITEM_PIMENT | TILE_FLOOR);
    }

    room->tiles[row][col] = current;
}

void room_remove_tile(Room* room, int row, int col, TileType tile) {
    if (room == NULL || !room_is_valid_index(row, col)) {
        return;
    }

    TileType current = room->tiles[row][col];

    if (tile == TILE_EMPTY) {
        room->tiles[row][col] = TILE_EMPTY;
        return;
    }

    current &= ~tile;

    if ((current & TILE_ROCK) != 0) {
        current |= TILE_FLOOR;
    }
    if ((current & TILE_DOOR) != 0) {
        current |= TILE_FLOOR;
    }
    if ((current & TILE_CHEST) != 0) {
        current |= TILE_FLOOR;
    }
    if ((current & (TILE_MONSTER_SPAWN_BASIC | TILE_MONSTER_SPAWN_TANK | TILE_MONSTER_SPAWN_SHOOTER | TILE_MONSTER_SPAWN_BOSS)) != 0) {
        current |= TILE_FLOOR;
    }
    if ((current & TILE_CHEST_OPENED) != 0) {
        current |= TILE_FLOOR;
    }
    if ((current & TILE_MONSTER_SPAWN) != 0) {
        current |= TILE_FLOOR;
    }
    if ((current & TILE_ITEM_COEUR) != 0) {
        current |= TILE_FLOOR;
    }
    if ((current & TILE_ITEM_PIMENT) != 0) {
        current |= TILE_FLOOR;
    }

    room->tiles[row][col] = current;
}

bool room_tile_has(const Room* room, int row, int col, TileType tile) {
    if (room == NULL || !room_is_valid_index(row, col)) {
        return false;
    }

    return (room->tiles[row][col] & tile) != 0;
}

bool room_tile_is_blocking(const Room* room, int row, int col) {
    return room_tile_has(room, row, col, TILE_ROCK) || 
           room_tile_has(room, row, col, TILE_CHEST);
}

bool room_check_collision(const Room* room, const SDL_Rect* room_rect, float x, float y, float w, float h) {
    if (room == NULL || room_rect == NULL) {
        return false;
    }

    float margin = 4.0f;
    float sample_points[4][2] = {
        { x + margin, y + margin },
        { x + w - margin, y + margin },
        { x + margin, y + h - margin },
        { x + w - margin, y + h - margin }
    };

    for (int i = 0; i < 4; i++) {
        int row = 0;
        int col = 0;

        if (!room_world_to_cell(room_rect, sample_points[i][0], sample_points[i][1], &row, &col)) {
            return true;
        }

        if (room_tile_is_blocking(room, row, col)) {
            return true;
        }
    }

    return false;
}

static void room_parse_line(Room* room, int row, const char* line) {
    char buffer[512];
    strncpy(buffer, line, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    char* token = strtok(buffer, ";\r\n");
    int col = 0;

    while (token != NULL && col < ROOM_COLS) {
        int value = atoi(token);
        if (value < 0) {
            value = 0;
        }

        TileType tile = (TileType)(value & (TILE_FLOOR | TILE_ROCK | TILE_DOOR | TILE_CHEST | TILE_MONSTER_SPAWN_BASIC | TILE_MONSTER_SPAWN_TANK | TILE_MONSTER_SPAWN_SHOOTER | TILE_MONSTER_SPAWN_BOSS));

        if ((tile & TILE_ROCK) != 0) {
            tile |= TILE_FLOOR;
        }
        if ((tile & TILE_DOOR) != 0) {
            tile |= TILE_FLOOR;
        }
        if ((tile & TILE_CHEST) != 0) {
            tile |= TILE_FLOOR;
        }
        if ((tile & (TILE_MONSTER_SPAWN_BASIC | TILE_MONSTER_SPAWN_TANK | TILE_MONSTER_SPAWN_SHOOTER | TILE_MONSTER_SPAWN_BOSS)) != 0) {
            tile |= TILE_FLOOR;
        }

        room_set_tile(room, row, col, tile);

        token = strtok(NULL, ";\r\n");
        col++;
    }

    while (col < ROOM_COLS) {
        room_set_tile(room, row, col, TILE_FLOOR);
        col++;
    }
}

bool room_save_csv(const Room* room, const char* path) {
    if (room == NULL || path == NULL) {
        return false;
    }

    FILE* file = fopen(path, "w");
    if (file == NULL) {
        printf("Impossible d'ouvrir %s pour l'ecriture.\n", path);
        return false;
    }

    for (int r = 0; r < ROOM_ROWS; r++) {
        for (int c = 0; c < ROOM_COLS; c++) {
            fprintf(file, "%d", room->tiles[r][c]);
            if (c < ROOM_COLS - 1) {
                fputc(';', file);
            }
        }
        fputc('\n', file);
    }

    fclose(file);
    return true;
}

bool room_load_csv(Room* room, const char* path) {
    if (room == NULL || path == NULL) {
        return false;
    }

    FILE* file = fopen(path, "r");
    if (file == NULL) {
        printf("Impossible de charger la salle %s\n", path);
        return false;
    }

    char line[512];
    int row = 0;

    while (fgets(line, sizeof(line), file) != NULL && row < ROOM_ROWS) {
        room_parse_line(room, row, line);
        row++;
    }

    while (row < ROOM_ROWS) {
        for (int c = 0; c < ROOM_COLS; c++) {
            room_set_tile(room, row, c, TILE_FLOOR);
        }
        row++;
    }

    fclose(file);
    return true;
}

int room_collect_csv_files(const char* directory, char files[][400], int max_files) {
    if (directory == NULL || files == NULL || max_files <= 0) {
        return 0;
    }

    int count = 0;

#ifdef _WIN32
    char search_path[260];
    snprintf(search_path, sizeof(search_path), "%s\\*.csv", directory);

    WIN32_FIND_DATAA find_data;
    HANDLE handle = FindFirstFileA(search_path, &find_data);
    if (handle == INVALID_HANDLE_VALUE) {
        return 0;
    }

    do {
        if ((find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
            snprintf(files[count], 260, "%s/%s", directory, find_data.cFileName);
            count++;
            if (count >= max_files) {
                break;
            }
        }
    } while (FindNextFileA(handle, &find_data));

    FindClose(handle);
#else
    DIR* dir = opendir(directory);
    if (dir == NULL) {
        return 0;
    }

    struct dirent* entry = NULL;
    while ((entry = readdir(dir)) != NULL && count < max_files) {
        if (entry->d_type == DT_REG || entry->d_type == DT_UNKNOWN) {
            size_t len = strlen(entry->d_name);
            if (len > 4 && strcmp(entry->d_name + len - 4, ".csv") == 0) {
                snprintf(files[count], 260, "%s/%s", directory, entry->d_name);
                count++;
            }
        }
    }

    closedir(dir);
#endif

    return count;
}

bool room_load_random(Room* room, const char* directory) {
    if (room == NULL || directory == NULL) {
        return false;
    }

    char files[64][400];
    int count = room_collect_csv_files(directory, files, 64);

    if (count <= 0) {
        printf("Aucune salle trouvee dans %s\n", directory);
        return false;
    }

    int index = rand() % count;
    return room_load_csv(room, files[index]);
}
