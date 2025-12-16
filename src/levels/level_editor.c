#include "level_editor.h"
#include "../utils/assets.h"
#include "../core/menu.h"
#include <stdio.h>

static bool level_editor_mouse_to_cell(LevelEditor* editor, int mouse_x, int mouse_y, int* out_row, int* out_col) {
    int local_x = mouse_x - editor->grid_rect.x;
    int local_y = mouse_y - editor->grid_rect.y;

    if (local_x < 0 || local_y < 0) {
        return false;
    }

    if (local_x >= editor->grid_rect.w || local_y >= editor->grid_rect.h) {
        return false;
    }

    if (out_col != NULL) {
        *out_col = local_x / ROOM_CELL_SIZE;
    }

    if (out_row != NULL) {
        *out_row = local_y / ROOM_CELL_SIZE;
    }

    return true;
}

static void level_editor_update_hover(LevelEditor* editor, int mouse_x, int mouse_y) {
    editor->hovered_row = -1;
    editor->hovered_col = -1;

    int local_x = mouse_x - editor->grid_rect.x;
    int local_y = mouse_y - editor->grid_rect.y;

    if (local_x < 0 || local_y < 0) {
        return;
    }

    if (local_x >= editor->grid_rect.w || local_y >= editor->grid_rect.h) {
        return;
    }

    editor->hovered_col = local_x / ROOM_CELL_SIZE;
    editor->hovered_row = local_y / ROOM_CELL_SIZE;
}

static void level_editor_place_tile(LevelEditor* editor, int row, int col, TileType type) {
    if (row < 0 || row >= ROOM_ROWS || col < 0 || col >= ROOM_COLS) {
        return;
    }

    if (type == TILE_EMPTY) {
        room_set_tile(&editor->room, row, col, TILE_EMPTY);
    } else {
        room_add_tile(&editor->room, row, col, type);
    }
}

static void level_editor_clear(LevelEditor* editor) {
    room_fill(&editor->room, TILE_FLOOR);
}

static bool level_editor_compute_next_save_path(char* out_path, size_t size) {
    for (int i = 1; i < 1000; i++) {
        char candidate[260];
        snprintf(candidate, sizeof(candidate), ROOMS_DIRECTORY "/room_%03d.csv", i);

        FILE* test = fopen(candidate, "r");
        if (test == NULL) {
            snprintf(out_path, size, "%s", candidate);
            return true;
        }

        fclose(test);
    }

    return false;
}

static void level_editor_save(LevelEditor* editor) {
    // Vérifier si la salle contient au moins une porte
    bool has_door = false;
    bool door_placement_invalid = false;

    for (int r = 0; r < ROOM_ROWS; r++) {
        for (int c = 0; c < ROOM_COLS; c++) {
            if (room_tile_has(&editor->room, r, c, TILE_DOOR)) {
                has_door = true;
                
                // Vérifier si la porte est sur un bord
                bool is_edge = (r == 0 || r == ROOM_ROWS - 1 || c == 0 || c == ROOM_COLS - 1);
                if (!is_edge) {
                    door_placement_invalid = true;
                }
            }
        }
    }

    if (!has_door) {
        printf("Erreur: La salle doit contenir au moins une porte pour etre sauvegardee.\n");
        return;
    }

    if (door_placement_invalid) {
        printf("Erreur: Les portes doivent etre placees sur les bords de la salle.\n");
        return;
    }

    char path[260];
    if (!level_editor_compute_next_save_path(path, sizeof(path))) {
        printf("Impossible de trouver un nom de fichier disponible.\n");
        return;
    }

    if (room_save_csv(&editor->room, path)) {
        printf("Salle sauvegardee dans %s\n", path);
    } else {
        printf("Echec de la sauvegarde de la salle.\n");
    }
}

static void level_editor_load_random(LevelEditor* editor) {
    if (room_load_random(&editor->room, ROOMS_DIRECTORY)) {
        printf("Salle chargee depuis un fichier aleatoire.\n");
    }
}

void level_editor_init(LevelEditor* editor, SDL_Renderer* renderer) {
    if (editor == NULL) {
        return;
    }

    room_init(&editor->room);
    editor->selected_tile = TILE_ROCK;
    editor->hovered_row = -1;
    editor->hovered_col = -1;

    editor->grid_rect.w = ROOM_COLS * ROOM_CELL_SIZE;
    editor->grid_rect.h = ROOM_ROWS * ROOM_CELL_SIZE;
    editor->grid_rect.x = (WINDOW_WIDTH - editor->grid_rect.w) / 2;
    editor->grid_rect.y = (WINDOW_HEIGHT - editor->grid_rect.h) / 2;

    editor->texture_floor = load_texture(renderer, "assets/images/decor/Sprite-sol.png");
    editor->texture_rock = load_texture(renderer, "assets/images/decor/Sprite-rock.png");
    editor->texture_door = load_texture(renderer, "assets/images/decor/Sprite-porte.png");
    editor->texture_chest = load_texture(renderer, "assets/images/decor/Sprite-coffre.png");
    editor->texture_spawn_basic = load_texture(renderer, "assets/images/monstre/basique/basique_bas.png");
    editor->texture_spawn_tank = load_texture(renderer, "assets/images/monstre/tank/tank_bas.png");
    editor->texture_spawn_shooter = load_texture(renderer, "assets/images/monstre/shooter/shooter_droite.png");
    editor->texture_spawn_boss = load_texture(renderer, "assets/images/monstre/boss/boss_bas.png");

    printf("=== Editeur de niveaux actif ===\n");
    printf("Clic gauche: poser | Clic droit: effacer\n");
    printf("Touches: 1 sol | 2 rocher | 3 porte | 4 coffre\n");
    printf("         I basique | O tank | P shooter | B boss\n");
    printf("         0 vide | C vider | S sauver | L charger aleatoire | ESC retour menu\n");
}

void level_editor_handle_event(LevelEditor* editor, SDL_Event* event) {
    if (editor == NULL || event == NULL) {
        return;
    }

    switch (event->type) {
        case SDL_MOUSEMOTION:
            level_editor_update_hover(editor, event->motion.x, event->motion.y);
            break;

        case SDL_MOUSEBUTTONDOWN: {
            int row = -1;
            int col = -1;
            if (!level_editor_mouse_to_cell(editor, event->button.x, event->button.y, &row, &col)) {
                break;
            }

            if (event->button.button == SDL_BUTTON_LEFT) {
                level_editor_place_tile(editor, row, col, editor->selected_tile);
            } else if (event->button.button == SDL_BUTTON_RIGHT) {
                level_editor_place_tile(editor, row, col, TILE_EMPTY);
            }
            break;
        }

        case SDL_KEYDOWN:
            switch (event->key.keysym.sym) {
                case SDLK_0:
                    editor->selected_tile = TILE_EMPTY;
                    break;
                case SDLK_1:
                    editor->selected_tile = TILE_FLOOR;
                    break;
                case SDLK_2:
                    editor->selected_tile = TILE_ROCK;
                    break;
                case SDLK_3:
                    editor->selected_tile = TILE_DOOR;
                    break;
                case SDLK_4:
                    editor->selected_tile = TILE_CHEST;
                    break;
                case SDLK_i:
                    editor->selected_tile = TILE_MONSTER_SPAWN_BASIC;
                    break;
                case SDLK_o:
                    editor->selected_tile = TILE_MONSTER_SPAWN_TANK;
                    break;
                case SDLK_p:
                    editor->selected_tile = TILE_MONSTER_SPAWN_SHOOTER;
                    break;
                case SDLK_b:
                    editor->selected_tile = TILE_MONSTER_SPAWN_BOSS;
                    break;
                case SDLK_c:
                    level_editor_clear(editor);
                    break;
                case SDLK_s:
                    level_editor_save(editor);
                    break;
                case SDLK_l:
                    level_editor_load_random(editor);
                    break;
                case SDLK_ESCAPE:
                    g_menu_state = MENU_STATE_MAIN_MENU;
                    break;
                default:
                    break;
            }
            break;

        default:
            break;
    }
}

void level_editor_update(LevelEditor* editor, float dt) {
    (void)editor;
    (void)dt;
}

static void level_editor_render_selected_tile(LevelEditor* editor, SDL_Renderer* renderer) {
    SDL_Rect preview = {20, WINDOW_HEIGHT - 84, 64, 64};

    SDL_SetRenderDrawColor(renderer, 10, 10, 10, 255);
    SDL_RenderFillRect(renderer, &preview);

    SDL_Rect inner = {preview.x + 4, preview.y + 4, preview.w - 8, preview.h - 8};

    if (editor->selected_tile == TILE_EMPTY) {
        SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
        SDL_RenderFillRect(renderer, &inner);
    } else {
        if ((editor->selected_tile & TILE_FLOOR) != 0) {
            if (editor->texture_floor != NULL) {
                SDL_RenderCopy(renderer, editor->texture_floor, NULL, &inner);
            }
        }

        if ((editor->selected_tile & TILE_ROCK) != 0) {
            if (editor->texture_rock != NULL) {
                SDL_RenderCopy(renderer, editor->texture_rock, NULL, &inner);
            }
        }

        if ((editor->selected_tile & TILE_DOOR) != 0) {
            if (editor->texture_door != NULL) {
                SDL_RenderCopy(renderer, editor->texture_door, NULL, &inner);
            }
        }

        if ((editor->selected_tile & TILE_CHEST) != 0) {
            if (editor->texture_chest != NULL) {
                SDL_RenderCopy(renderer, editor->texture_chest, NULL, &inner);
            }
        }

        if ((editor->selected_tile & TILE_MONSTER_SPAWN_BASIC) != 0) {
            if (editor->texture_spawn_basic != NULL) {
                SDL_RenderCopy(renderer, editor->texture_spawn_basic, NULL, &inner);
            }
        }

        if ((editor->selected_tile & TILE_MONSTER_SPAWN_TANK) != 0) {
            if (editor->texture_spawn_tank != NULL) {
                // If a specific texture is set for tank, use it; otherwise draw red square
                SDL_RenderCopy(renderer, editor->texture_spawn_tank, NULL, &inner);
            } else {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                SDL_RenderFillRect(renderer, &inner);
            }
        }

        if ((editor->selected_tile & TILE_MONSTER_SPAWN_SHOOTER) != 0) {
            if (editor->texture_spawn_shooter != NULL) {
                SDL_RenderCopy(renderer, editor->texture_spawn_shooter, NULL, &inner);
            } else {
                // Shooter preview: green square
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                SDL_RenderFillRect(renderer, &inner);
            }
        }

        if ((editor->selected_tile & TILE_MONSTER_SPAWN_BOSS) != 0) {
            if (editor->texture_spawn_boss != NULL) {
                SDL_RenderCopy(renderer, editor->texture_spawn_boss, NULL, &inner);
            } else {
                // Boss preview: purple square
                SDL_SetRenderDrawColor(renderer, 128, 0, 255, 255);
                SDL_RenderFillRect(renderer, &inner);
            }
        }
    }

    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderDrawRect(renderer, &preview);
}


void level_editor_render(LevelEditor* editor, SDL_Renderer* renderer) {
    if (editor == NULL) {
        return;
    }

    SDL_SetRenderDrawColor(renderer, 30, 30, 50, 255);
    SDL_RenderClear(renderer);

    for (int r = 0; r < ROOM_ROWS; r++) {
        for (int c = 0; c < ROOM_COLS; c++) {
            SDL_Rect cell = {
                editor->grid_rect.x + c * ROOM_CELL_SIZE,
                editor->grid_rect.y + r * ROOM_CELL_SIZE,
                ROOM_CELL_SIZE,
                ROOM_CELL_SIZE
            };

            bool has_floor = room_tile_has(&editor->room, r, c, TILE_FLOOR);
            bool has_rock = room_tile_has(&editor->room, r, c, TILE_ROCK);
            bool has_door = room_tile_has(&editor->room, r, c, TILE_DOOR);
            bool has_chest = room_tile_has(&editor->room, r, c, TILE_CHEST);
            bool has_spawn_basic = room_tile_has(&editor->room, r, c, TILE_MONSTER_SPAWN_BASIC);
            bool has_spawn_tank = room_tile_has(&editor->room, r, c, TILE_MONSTER_SPAWN_TANK);
            bool has_spawn_shooter = room_tile_has(&editor->room, r, c, TILE_MONSTER_SPAWN_SHOOTER);
            bool has_spawn_boss = room_tile_has(&editor->room, r, c, TILE_MONSTER_SPAWN_BOSS);

            if (has_floor) {
                if (editor->texture_floor != NULL) {
                    SDL_RenderCopy(renderer, editor->texture_floor, NULL, &cell);
                } else {
                    SDL_SetRenderDrawColor(renderer, 90, 90, 90, 255);
                    SDL_RenderFillRect(renderer, &cell);
                }
            } else if (!has_rock && !has_door && !has_chest && !has_spawn_basic && !has_spawn_tank && !has_spawn_shooter && !has_spawn_boss) {
                SDL_SetRenderDrawColor(renderer, 40, 40, 60, 255);
                SDL_RenderFillRect(renderer, &cell);
            }

            if (has_rock) {
                if (editor->texture_rock != NULL) {
                    SDL_RenderCopy(renderer, editor->texture_rock, NULL, &cell);
                } else {
                    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
                    SDL_RenderFillRect(renderer, &cell);
                }
            }

            if (has_door) {
                if (editor->texture_door != NULL) {
                    SDL_RenderCopy(renderer, editor->texture_door, NULL, &cell);
                } else {
                    SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255);
                    SDL_RenderFillRect(renderer, &cell);
                }
            }

            if (has_chest) {
                if (editor->texture_chest != NULL) {
                    SDL_RenderCopy(renderer, editor->texture_chest, NULL, &cell);
                } else {
                    SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
                    SDL_RenderFillRect(renderer, &cell);
                }
            }

            if (has_spawn_basic) {
                if (editor->texture_spawn_basic != NULL) {
                    SDL_RenderCopy(renderer, editor->texture_spawn_basic, NULL, &cell);
                } else {
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                    SDL_RenderFillRect(renderer, &cell);
                }
            }

            if (has_spawn_tank) {
                if (editor->texture_spawn_tank != NULL) {
                    SDL_RenderCopy(renderer, editor->texture_spawn_tank, NULL, &cell);
                } else {
                    // Use same red square as preview for tank when no texture is provided
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                    SDL_RenderFillRect(renderer, &cell);
                }
            }

            if (has_spawn_shooter) {
                if (editor->texture_spawn_shooter != NULL) {
                    SDL_RenderCopy(renderer, editor->texture_spawn_shooter, NULL, &cell);
                } else {
                    // Shooter cell: green square (matching preview)
                    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                    SDL_RenderFillRect(renderer, &cell);
                }
            }

            if (has_spawn_boss) {
                if (editor->texture_spawn_boss != NULL) {
                    SDL_RenderCopy(renderer, editor->texture_spawn_boss, NULL, &cell);
                } else {
                    // Boss cell: purple square (matching preview)
                    SDL_SetRenderDrawColor(renderer, 128, 0, 255, 255);
                    SDL_RenderFillRect(renderer, &cell);
                }
            }

            SDL_SetRenderDrawColor(renderer, 20, 20, 40, 255);
            SDL_RenderDrawRect(renderer, &cell);
        }
    }

    if (editor->hovered_row >= 0 && editor->hovered_col >= 0) {
        SDL_Rect highlight = {
            editor->grid_rect.x + editor->hovered_col * ROOM_CELL_SIZE,
            editor->grid_rect.y + editor->hovered_row * ROOM_CELL_SIZE,
            ROOM_CELL_SIZE,
            ROOM_CELL_SIZE
        };

        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 120);
        SDL_RenderDrawRect(renderer, &highlight);
    }

    level_editor_render_selected_tile(editor, renderer);
}

void level_editor_cleanup(LevelEditor* editor) {
    if (editor == NULL) {
        return;
    }

    if (editor->texture_floor != NULL) {
        SDL_DestroyTexture(editor->texture_floor);
        editor->texture_floor = NULL;
    }

    if (editor->texture_rock != NULL) {
        SDL_DestroyTexture(editor->texture_rock);
        editor->texture_rock = NULL;
    }

    if (editor->texture_door != NULL) {
        SDL_DestroyTexture(editor->texture_door);
        editor->texture_door = NULL;
    }

    if (editor->texture_chest != NULL) {
        SDL_DestroyTexture(editor->texture_chest);
        editor->texture_chest = NULL;
    }

    if (editor->texture_spawn_basic != NULL) {
        SDL_DestroyTexture(editor->texture_spawn_basic);
        editor->texture_spawn_basic = NULL;
    }

    if (editor->texture_spawn_tank != NULL) {
        SDL_DestroyTexture(editor->texture_spawn_tank);
        editor->texture_spawn_tank = NULL;
    }

    if (editor->texture_spawn_shooter != NULL) {
        SDL_DestroyTexture(editor->texture_spawn_shooter);
        editor->texture_spawn_shooter = NULL;
    }

    if (editor->texture_spawn_boss != NULL) {
        SDL_DestroyTexture(editor->texture_spawn_boss);
        editor->texture_spawn_boss = NULL;
    }
}


