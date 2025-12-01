#ifndef LEVEL_EDITOR_H
#define LEVEL_EDITOR_H

#include "../utils/sdl_common.h"
#include "../utils/constants.h"
#include "room.h"

typedef struct {
    Room room;
    TileType selected_tile;
    SDL_Texture* texture_floor;
    SDL_Texture* texture_rock;
    SDL_Texture* texture_door;
    SDL_Texture* texture_chest;
    SDL_Texture* texture_spawn;
    SDL_Rect grid_rect;
    int hovered_row;
    int hovered_col;
} LevelEditor;

void level_editor_init(LevelEditor* editor, SDL_Renderer* renderer);
void level_editor_handle_event(LevelEditor* editor, SDL_Event* event);
void level_editor_update(LevelEditor* editor, float dt);
void level_editor_render(LevelEditor* editor, SDL_Renderer* renderer);
void level_editor_cleanup(LevelEditor* editor);

#endif


