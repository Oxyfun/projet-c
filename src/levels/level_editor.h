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
<<<<<<< HEAD
    SDL_Texture* texture_spawn_basic;
    SDL_Texture* texture_spawn_tank;
    SDL_Texture* texture_spawn_shooter;
    SDL_Texture* texture_spawn_boss;
=======
    SDL_Texture* texture_spawn;
    SDL_Texture* texture_item_coeur;
    SDL_Texture* texture_item_piment;
>>>>>>> 51943c440a6468b9fbb32fc6db4747f11bfb43d9
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


