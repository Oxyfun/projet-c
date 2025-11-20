#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../utils/sdl_common.h"
#include "../utils/constants.h"
#include "../utils/assets.h"
#include "../player/player.h"
#include "../levels/room.h"
#include "../levels/level_editor.h"
#include "../monsters/monster.h"
#include "menu.h"
#include <time.h>

static SDL_Rect create_room_rect(void) {
    SDL_Rect rect;
    rect.w = ROOM_COLS * ROOM_CELL_SIZE;
    rect.h = ROOM_ROWS * ROOM_CELL_SIZE;
    rect.x = (WINDOW_WIDTH - rect.w) / 2;
    rect.y = (WINDOW_HEIGHT - rect.h) / 2;
    return rect;
}

static bool world_to_room_cell(const SDL_Rect* room_rect, float x, float y, int* out_row, int* out_col) {
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

static void ensure_spawn_is_clear(Player* player, Room* room, const SDL_Rect* room_rect) {
    if (player == NULL || room == NULL || room_rect == NULL) {
        return;
    }

    float margin = 4.0f;
    float sample_points[4][2] = {
        { player->x + margin, player->y + margin },
        { player->x + player->w - margin, player->y + margin },
        { player->x + margin, player->y + player->h - margin },
        { player->x + player->w - margin, player->y + player->h - margin }
    };

    for (int i = 0; i < 4; i++) {
        int row = 0;
        int col = 0;
        if (world_to_room_cell(room_rect, sample_points[i][0], sample_points[i][1], &row, &col)) {
            room_remove_tile(room, row, col, TILE_ROCK);
            room_add_tile(room, row, col, TILE_FLOOR);
        }
    }
}

static void render_room(SDL_Renderer* renderer, const Room* room, const SDL_Rect* room_rect, SDL_Texture* texture_floor, SDL_Texture* texture_rock) {
    if (renderer == NULL || room == NULL || room_rect == NULL) {
        return;
    }

    for (int r = 0; r < ROOM_ROWS; r++) {
        for (int c = 0; c < ROOM_COLS; c++) {
            SDL_Rect cell = {
                room_rect->x + c * ROOM_CELL_SIZE,
                room_rect->y + r * ROOM_CELL_SIZE,
                ROOM_CELL_SIZE,
                ROOM_CELL_SIZE
            };

            bool has_floor = room_tile_has(room, r, c, TILE_FLOOR);
            bool has_rock = room_tile_has(room, r, c, TILE_ROCK);

            if (has_floor) {
                if (texture_floor != NULL) {
                    SDL_RenderCopy(renderer, texture_floor, NULL, &cell);
                }
                else {
                    SDL_SetRenderDrawColor(renderer, 90, 90, 90, 255);
                    SDL_RenderFillRect(renderer, &cell);
                }
            }
            else if (!has_rock) {
                SDL_SetRenderDrawColor(renderer, 40, 40, 60, 255);
                SDL_RenderFillRect(renderer, &cell);
            }

            if (has_rock) {
                if (texture_rock != NULL) {
                    SDL_RenderCopy(renderer, texture_rock, NULL, &cell);
                }
                else {
                    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
                    SDL_RenderFillRect(renderer, &cell);
                }
            }

        }
    }
}

int main(int argc, char* argv[]) {
    // Initialisation de SDL + vérif
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur lors de l'initialisation de SDL: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // Initialisation de SDL_image, que pour PNG pour l'instant
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        printf("Erreur lors de l'initialisation de SDL_image: %s\n", IMG_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    //initialise sdl_ttf
    TTF_Init();

    // Création de la fenêtre
    SDL_Window* window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (window == NULL) {
        printf("Erreur lors de la création de la fenêtre: %s\n", SDL_GetError());
        IMG_Quit();
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
    // Création du renderer avec VSync pour limiter les FPS (car ça laggait quand on fermait le jeu)
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        printf("Erreur lors de la création du renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    // Initialisation du menu
    Menu menu;
    menu_init(&menu, renderer);

    LevelEditor level_editor;
    bool level_editor_initialized = false;

    Room current_room;
    room_init(&current_room);
    SDL_Rect room_rect = create_room_rect();
    bool room_loaded = false;

    // Initialisation du joueur (sera initialisé quand on lance le jeu via le menu)
    Player player;
    bool player_initialized = false;

    TTF_Font* font = TTF_OpenFont("assets/fonts/Zombie.ttf", 24);
    (void)font;

    // Charger la texture des projectiles
    SDL_Texture* projectile_texture = load_texture(renderer, "assets/images/projectiles/proj.png");
    if (projectile_texture == NULL) {
        printf("Impossible de charger proj.png\n");
    }

    SDL_Texture* tile_floor_texture = load_texture(renderer, "assets/images/decor/Sprite-sol.png");
    if (tile_floor_texture == NULL) {
        printf("Impossible de charger Sprite-sol.png\n");
    }

    SDL_Texture* tile_rock_texture = load_texture(renderer, "assets/images/decor/Sprite-rock.png");
    if (tile_rock_texture == NULL) {
        printf("Impossible de charger Sprite-rock.png\n");
    }

    // Initialisation des projectiles
    Projectile projectiles[MAX_PROJECTILES];

    // Initialiser tous les projectiles comme inactifs
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        projectiles[i].active = false;
    }

    // Variables pour le delta time
    Uint32 last_time = SDL_GetTicks();

    // Boucle principale
    bool running = true;
    SDL_Event event;

    srand((unsigned int)time(NULL));

    while (running) {
        // Calcul du delta time, si ya pas ça la vitesse du joueur sera proportionnelle aux FPS ( c'est la diff entre 2 frames )
        Uint32 current_time_ms = SDL_GetTicks();
        float delta_time = (current_time_ms - last_time) / MS_TO_SECONDS;
        float current_time = current_time_ms / MS_TO_SECONDS;
        last_time = current_time_ms;

        // Gestion des événements
        while (SDL_PollEvent(&event)) {
            // Événements globaux
            if (event.type == SDL_QUIT) {
                running = false;
            }

            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                if (g_menu_state == MENU_STATE_LEVEL_EDITOR) {
                    g_menu_state = MENU_STATE_MAIN_MENU;
                }
                else {
                    running = false;
                }
                continue;
            }

            // Événements du menu
            if (g_menu_state == MENU_STATE_MAIN_MENU) {
                menu_update(&menu, &event);
            }
            else if (g_menu_state == MENU_STATE_LEVEL_EDITOR) {
                if (!level_editor_initialized) {
                    level_editor_init(&level_editor, renderer);
                    level_editor_initialized = true;
                }
                level_editor_handle_event(&level_editor, &event);
            }
        }

        if (g_menu_state != MENU_STATE_LEVEL_EDITOR && level_editor_initialized) {
            level_editor_cleanup(&level_editor);
            level_editor_initialized = false;
        }

        // Vérifier si on doit quitter
        if (g_menu_state == MENU_STATE_QUIT) {
            running = false;
        }

        // Initialiser le joueur si on lance le jeu
        if (g_menu_state == MENU_STATE_GAME && !player_initialized) {
            player_init(&player, renderer);
            player_initialized = true;
            printf("ZQSD pour bouger | Flèches pour tirer\n");

            room_init(&current_room);
            if (!room_load_random(&current_room, ROOMS_DIRECTORY)) {
                room_fill(&current_room, TILE_FLOOR);
            }
            room_loaded = true;
            ensure_spawn_is_clear(&player, &current_room, &room_rect);
        }

        // Mise à jour et rendu selon l'état
        if (g_menu_state == MENU_STATE_MAIN_MENU) {
            // Afficher le menu
            menu_render(renderer, &menu);
        }
        else if (g_menu_state == MENU_STATE_GAME && player_initialized) {
            if (!room_loaded) {
                room_init(&current_room);
                room_fill(&current_room, TILE_FLOOR);
                room_loaded = true;
            }

            // Mise à jour du jeu
            const Uint8* keys = SDL_GetKeyboardState(NULL); // quelle touche pressé
            player_update(&player, keys, delta_time, current_time, projectiles, MAX_PROJECTILES, projectile_texture, &current_room, &room_rect);

            // Mise à jour des projectiles
            for (int i = 0; i < MAX_PROJECTILES; i++) {
                if (projectiles[i].active) {
                    projectile_update(&projectiles[i], delta_time);
                }
            }

            // Afficher le jeu
            SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
            SDL_RenderClear(renderer);

            render_room(renderer, &current_room, &room_rect, tile_floor_texture, tile_rock_texture);

            player_render(renderer, &player);

            for (int i = 0; i < MAX_PROJECTILES; i++) {
                if (projectiles[i].active) {
                    projectile_render(renderer, &projectiles[i]);
                }
            }
        }
        else if (g_menu_state == MENU_STATE_LEVEL_EDITOR) {
            if (!level_editor_initialized) {
                level_editor_init(&level_editor, renderer);
                level_editor_initialized = true;
            }
            level_editor_update(&level_editor, delta_time);
            level_editor_render(&level_editor, renderer);
        }

        SDL_RenderPresent(renderer);

        // Backup si VSync ne marche pas
        SDL_Delay(1);
    }

    if (level_editor_initialized) {
        level_editor_cleanup(&level_editor);
    }

    // Nettoyage
    menu_cleanup(&menu);

    if (player_initialized) {
        player_cleanup(&player);
    }

    // Nettoyage des projectiles
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        projectile_cleanup(&projectiles[i]);
    }

    if (tile_floor_texture != NULL) {
        SDL_DestroyTexture(tile_floor_texture);
    }

    if (tile_rock_texture != NULL) {
        SDL_DestroyTexture(tile_rock_texture);
    }

    // Détruire la texture partagée des projectiles
    if (projectile_texture != NULL) {
        SDL_DestroyTexture(projectile_texture);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    TTF_Quit();

    exit(EXIT_SUCCESS);
}
