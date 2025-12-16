#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../utils/sdl_common.h"
#include "../utils/constants.h"
#include "../utils/assets.h"
#include "../player/player.h"
#include "../levels/room.h"
#include "../levels/dungeon.h"
#include "../levels/level_editor.h"
#include "../monsters/monster.h"
<<<<<<< HEAD
#include "../monsters/monster_projectile.h"
=======
#include "../items/item.h"
>>>>>>> 51943c440a6468b9fbb32fc6db4747f11bfb43d9
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

<<<<<<< HEAD
static void setup_room_entities(Room* room, const SDL_Rect* room_rect, Monster* monsters, int max_monsters, 
    SDL_Texture* basic_up, SDL_Texture* basic_down, SDL_Texture* basic_left, SDL_Texture* basic_right,
    SDL_Texture* tank_up, SDL_Texture* tank_down, SDL_Texture* tank_left, SDL_Texture* tank_right, SDL_Texture* shooter_left, SDL_Texture* shooter_right,
    SDL_Texture* boss_up, SDL_Texture* boss_down, SDL_Texture* boss_left, SDL_Texture* boss_right,
    SDL_Texture* shooter_projectile_texture, SDL_Texture* boss_projectile_texture) {
    // Réinitialiser tous les monstres
=======
static void setup_room_entities(Room* room, const SDL_Rect* room_rect, Monster* monsters, int max_monsters, SDL_Texture* t_up, SDL_Texture* t_down, SDL_Texture* t_left, SDL_Texture* t_right) {
    // reset monstres
>>>>>>> 51943c440a6468b9fbb32fc6db4747f11bfb43d9
    for (int i = 0; i < max_monsters; i++) {
        monsters[i].alive = false;
    }

    if (room == NULL) return;

    // chercher spawns
    int spawned_count = 0;
    for (int r = 0; r < ROOM_ROWS; r++) {
        for (int c = 0; c < ROOM_COLS; c++) {
            if (room_tile_has(room, r, c, TILE_MONSTER_SPAWN_BASIC) || room_tile_has(room, r, c, TILE_MONSTER_SPAWN_TANK) || room_tile_has(room, r, c, TILE_MONSTER_SPAWN_SHOOTER) || room_tile_has(room, r, c, TILE_MONSTER_SPAWN_BOSS)) {
                if (spawned_count < max_monsters) {
                    float spawn_x = room_rect->x + c * ROOM_CELL_SIZE + (ROOM_CELL_SIZE - 48.0f) / 2;
                    float spawn_y = room_rect->y + r * ROOM_CELL_SIZE + (ROOM_CELL_SIZE - 48.0f) / 2;

                    MonsterType type = MONSTER_TYPE_BASIC;
                    SDL_Texture* tex_up = basic_up;
                    SDL_Texture* tex_down = basic_down;
                    SDL_Texture* tex_left = basic_left;
                    SDL_Texture* tex_right = basic_right;

                    if (room_tile_has(room, r, c, TILE_MONSTER_SPAWN_TANK)) {
                        type = MONSTER_TYPE_TANK;
                        tex_up = tank_up;
                        tex_down = tank_down;
                        tex_left = tank_left;
                        tex_right = tank_right;
                    } else if (room_tile_has(room, r, c, TILE_MONSTER_SPAWN_SHOOTER)) {
                        type = MONSTER_TYPE_SHOOTER;
                        tex_up = NULL;  // Shooter n'a pas de texture haut
                        tex_down = NULL;  // Shooter n'a pas de texture bas
                        tex_left = shooter_left;
                        tex_right = shooter_right;
                    } else if (room_tile_has(room, r, c, TILE_MONSTER_SPAWN_BOSS)) {
                        type = MONSTER_TYPE_BOSS;
                        tex_up = boss_up;
                        tex_down = boss_down;
                        tex_left = boss_left;
                        tex_right = boss_right;
                        // Adjust spawn position for larger boss
                        spawn_x = room_rect->x + c * ROOM_CELL_SIZE + (ROOM_CELL_SIZE - 96.0f) / 2;
                        spawn_y = room_rect->y + r * ROOM_CELL_SIZE + (ROOM_CELL_SIZE - 96.0f) / 2;
                    }

                    // Déterminer la texture de projectile
                    SDL_Texture* proj_tex = NULL;
                    if (type == MONSTER_TYPE_SHOOTER) {
                        proj_tex = shooter_projectile_texture;
                    } else if (type == MONSTER_TYPE_BOSS) {
                        proj_tex = boss_projectile_texture;
                    }

                    monster_init(&monsters[spawned_count], spawn_x, spawn_y, type, tex_up, tex_down, tex_left, tex_right, proj_tex);
                    spawned_count++;
                }
            }
        }
    }
}

static void render_room(SDL_Renderer* renderer, const Room* room, const SDL_Rect* room_rect, SDL_Texture* texture_floor, SDL_Texture* texture_rock, SDL_Texture* texture_door, SDL_Texture* texture_chest, SDL_Texture* texture_chest_opened) {
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
            bool has_door = room_tile_has(room, r, c, TILE_DOOR);
            bool has_chest = room_tile_has(room, r, c, TILE_CHEST);
            bool has_chest_opened = room_tile_has(room, r, c, TILE_CHEST_OPENED);

            if (has_floor) {
                if (texture_floor != NULL) {
                    SDL_RenderCopy(renderer, texture_floor, NULL, &cell);
                }
                else {
                    SDL_SetRenderDrawColor(renderer, 90, 90, 90, 255);
                    SDL_RenderFillRect(renderer, &cell);
                }
            }
            else if (!has_rock && !has_door && !has_chest && !has_chest_opened) {
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

            if (has_door) {
                if (texture_door != NULL) {
                    SDL_RenderCopy(renderer, texture_door, NULL, &cell);
                }
                else {
                    SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255);
                    SDL_RenderFillRect(renderer, &cell);
                }
            }

            if (has_chest) {
                if (texture_chest != NULL) {
                    SDL_RenderCopy(renderer, texture_chest, NULL, &cell);
                }
                else {
                    SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
                    SDL_RenderFillRect(renderer, &cell);
                }
            }

            if (has_chest_opened) {
                if (texture_chest_opened != NULL) {
                    SDL_RenderCopy(renderer, texture_chest_opened, NULL, &cell);
                }
                else {
                    SDL_SetRenderDrawColor(renderer, 180, 180, 100, 255);
                    SDL_RenderFillRect(renderer, &cell);
                }
            }

        }
    }
}

static void render_minimap(SDL_Renderer* renderer, const Dungeon* dungeon) {
    if (renderer == NULL || dungeon == NULL) return;

    // transparence
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    int cell_size = 15;
    int margin = 20;
    int start_x = WINDOW_WIDTH - (DUNGEON_MAP_SIZE * cell_size) - margin;
    int start_y = 20;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 100);
    SDL_Rect bg_rect = { start_x - 5, start_y - 5, DUNGEON_MAP_SIZE * cell_size + 10, DUNGEON_MAP_SIZE * cell_size + 10 };
    SDL_RenderFillRect(renderer, &bg_rect);

    for (int x = 0; x < DUNGEON_MAP_SIZE; x++) {
        for (int y = 0; y < DUNGEON_MAP_SIZE; y++) {
            // dessin que si la salle existe
            if (dungeon->has_room[x][y]) {
                SDL_Rect cell = {
                    start_x + x * cell_size,
                    start_y + y * cell_size,
                    cell_size - 2,
                    cell_size - 2
                };

                if (x == dungeon->current_map_x && y == dungeon->current_map_y) {
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 200);
                } else {
                    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 150);
                }
                SDL_RenderFillRect(renderer, &cell);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    // init SDL + verif
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur lors de l'initialisation de SDL: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // Initialisation de SDL_image pour PNG
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        printf("Erreur lors de l'initialisation de SDL_image: %s\n", IMG_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    //initialise sdl_ttf
    TTF_Init();

    // fenetre
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

    // renderer avec VSync limite FPS
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        printf("Erreur lors de la création du renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    // menu
    Menu menu;
    menu_init(&menu, renderer);

    LevelEditor level_editor;
    bool level_editor_initialized = false;

    // donjon
    Dungeon dungeon;
    dungeon_init(&dungeon);
    dungeon_load_templates(&dungeon, ROOMS_DIRECTORY);

    SDL_Rect room_rect = create_room_rect();
    bool game_started = false;
    
    //game over
    bool game_over_screen = false;
    Uint32 game_over_start_time = 0;

    // joueur init plus tard
    Player player;
    bool player_initialized = false;

    // monstres
    Monster monsters[MAX_MONSTERS];
    for (int i = 0; i < MAX_MONSTERS; i++) {
        monsters[i].alive = false;
    }

    // items de la salle
    Item room_items[MAX_ITEMS_PER_ROOM];
    int room_item_count = 0;

    TTF_Font* font = TTF_OpenFont("assets/fonts/Zombie.ttf", 24);

<<<<<<< HEAD
    // Charger la texture des projectiles du joueur
=======
    // texture projectiles
>>>>>>> 51943c440a6468b9fbb32fc6db4747f11bfb43d9
    SDL_Texture* projectile_texture = load_texture(renderer, "assets/images/projectiles/proj.png");
    if (projectile_texture == NULL) {
        printf("Impossible de charger proj.png\n");
    }

	// Charger les textures des projectiles spécifiques aux monstres
    SDL_Texture* projectile_shooter_texture = load_texture(renderer, "assets/images/projectiles/proj_shooter.png");
    if (projectile_shooter_texture == NULL) {
        printf("Impossible de charger proj_shooter.png\n");
    }
    SDL_Texture* projectile_boss_texture = load_texture(renderer, "assets/images/projectiles/proj_boss.png");
    if (projectile_boss_texture == NULL) {
        printf("Impossible de charger proj_boss.png\n");
    }


    SDL_Texture* tile_floor_texture = load_texture(renderer, "assets/images/decor/Sprite-sol.png");
    if (tile_floor_texture == NULL) {
        printf("Impossible de charger Sprite-sol.png\n");
    }

    SDL_Texture* tile_rock_texture = load_texture(renderer, "assets/images/decor/Sprite-rock.png");
    if (tile_rock_texture == NULL) {
        printf("Impossible de charger Sprite-rock.png\n");
    }

    SDL_Texture* tile_door_texture = load_texture(renderer, "assets/images/decor/Sprite-porte.png");
    if (tile_door_texture == NULL) {
        printf("Impossible de charger Sprite-porte.png\n");
    }

    SDL_Texture* tile_chest_texture = load_texture(renderer, "assets/images/decor/Sprite-coffre.png");
    if (tile_chest_texture == NULL) {
        printf("Impossible de charger Sprite-coffre.png\n");
    }

<<<<<<< HEAD
    // Chargement des textures des monstres BASIC
    SDL_Texture* monster_basic_up = load_texture(renderer, "assets/images/monstre/basique/basique_haut.png");
    SDL_Texture* monster_basic_down = load_texture(renderer, "assets/images/monstre/basique/basique_bas.png");
    SDL_Texture* monster_basic_left = load_texture(renderer, "assets/images/monstre/basique/basique_gauche.png");
    SDL_Texture* monster_basic_right = load_texture(renderer, "assets/images/monstre/basique/basique_droite.png");
=======
    SDL_Texture* tile_chest_opened_texture = load_texture(renderer, "assets/images/decor/Sprite-coffre-ouvert.png");
    if (tile_chest_opened_texture == NULL) {
        printf("Impossible de charger Sprite-coffre-ouvert.png\n");
    }

    // textures monstres
    SDL_Texture* monster_tex_up = load_texture(renderer, "assets/images/monstre/monstre_haut.png");
    SDL_Texture* monster_tex_down = load_texture(renderer, "assets/images/monstre/monstre_bas.png");
    SDL_Texture* monster_tex_left = load_texture(renderer, "assets/images/monstre/monstre_gauche.png");
    SDL_Texture* monster_tex_right = load_texture(renderer, "assets/images/monstre/monstre_droite.png");
>>>>>>> 51943c440a6468b9fbb32fc6db4747f11bfb43d9

    // Chargement des textures des monstres TANK
    SDL_Texture* monster_tank_up = load_texture(renderer, "assets/images/monstre/tank/tank_haut.png");
    SDL_Texture* monster_tank_down = load_texture(renderer, "assets/images/monstre/tank/tank_bas.png");
    SDL_Texture* monster_tank_left = load_texture(renderer, "assets/images/monstre/tank/tank_gauche.png");
    SDL_Texture* monster_tank_right = load_texture(renderer, "assets/images/monstre/tank/tank_droite.png");

    // Chargement des textures des monstres SHOOTER
    SDL_Texture* monster_shooter_left = load_texture(renderer, "assets/images/monstre/shooter/shooter_gauche.png");
    SDL_Texture* monster_shooter_right = load_texture(renderer, "assets/images/monstre/shooter/shooter_droite.png");

    // Chargement des textures du BOSS
    SDL_Texture* monster_boss_up = load_texture(renderer, "assets/images/monstre/boss/boss_haut.png");
    SDL_Texture* monster_boss_down = load_texture(renderer, "assets/images/monstre/boss/boss_bas.png");
    SDL_Texture* monster_boss_left = load_texture(renderer, "assets/images/monstre/boss/boss_gauche.png");
    SDL_Texture* monster_boss_right = load_texture(renderer, "assets/images/monstre/boss/boss_droite.png");

    if (monster_basic_down == NULL) {
        printf("Impossible de charger les textures du monstre\n");
    }

    ItemManager* item_manager = item_manager_create(10);
    if (item_manager) {
        int loaded_items = item_manager_load_all_from_directory(item_manager, "items", renderer);
        printf("=== %d items charges automatiquement ===\n", loaded_items);
        for (int i = 0; i < item_manager->count; i++) {
            printf("  [%d] %s\n", i, item_manager->items[i].name);
        }
    } else {
        printf("Erreur: impossible de creer l'item manager\n");
    }

    // projectiles
    Projectile projectiles[MAX_PROJECTILES];
    
    // desactive tous les projectiles au debut
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        projectiles[i].active = false;
    }

<<<<<<< HEAD
    // Initialisation des projectiles des monstres
    MonsterProjectile monster_projectiles[MAX_PROJECTILES];
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        monster_projectiles[i].active = false;
    }

    // Variable pour tracker si on doit nettoyer les projectiles
    bool last_menu_state_was_game = false;

    // Variables pour le delta time
=======
    // delta time
>>>>>>> 51943c440a6468b9fbb32fc6db4747f11bfb43d9
    Uint32 last_time = SDL_GetTicks();

    // boucle principale
    bool running = true;
    SDL_Event event;

    srand((unsigned int)time(NULL));

    while (running) {
        // Calcul du delta time entre frames pour pas que vitesse depend des FPS
        Uint32 current_time_ms = SDL_GetTicks();
        float delta_time = (current_time_ms - last_time) / MS_TO_SECONDS;
        float current_time = current_time_ms / MS_TO_SECONDS;
        last_time = current_time_ms;

        // events
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

            // events menu
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

        // quitter
        if (g_menu_state == MENU_STATE_QUIT) {
            running = false;
        }

<<<<<<< HEAD
        // Nettoyer les projectiles si on retourne au menu depuis le jeu
        if (g_menu_state == MENU_STATE_MAIN_MENU && last_menu_state_was_game) {
            for (int i = 0; i < MAX_PROJECTILES; i++) {
                projectiles[i].active = false;
                monster_projectiles[i].active = false;
            }
            last_menu_state_was_game = false;
        }

        // Tracker si on est en jeu
        if (g_menu_state == MENU_STATE_GAME) {
            last_menu_state_was_game = true;
        }

        // Initialiser le joueur et le donjon si on lance le jeu
=======
        // init joueur + donjon au debut du jeu
>>>>>>> 51943c440a6468b9fbb32fc6db4747f11bfb43d9
        if (g_menu_state == MENU_STATE_GAME && !player_initialized) {
            player_init(&player, renderer);
            player.current_health = player.max_health;
            player_initialized = true;
            printf("ZQSD pour bouger | Flèches pour tirer\n");

            // generation donjon
            dungeon_generate(&dungeon);
            game_started = true;

            Room* start_room = dungeon_get_current_room(&dungeon);
            setup_room_entities(start_room, &room_rect, monsters, MAX_MONSTERS, 
                monster_basic_up, monster_basic_down, monster_basic_left, monster_basic_right,
                monster_tank_up, monster_tank_down, monster_tank_left, monster_tank_right, monster_shooter_left, monster_shooter_right,
                monster_boss_up, monster_boss_down, monster_boss_left, monster_boss_right,
                projectile_shooter_texture, projectile_boss_texture);
            
            ensure_spawn_is_clear(&player, start_room, &room_rect);

            // Initialiser le compteur d'items
            room_item_count = 0;

        }

        // update + rendu selon etat
        if (g_menu_state == MENU_STATE_MAIN_MENU) {
            // rendu menu
            menu_render(renderer, &menu);
        }
        else if (g_menu_state == MENU_STATE_GAME && player_initialized) 
        {
            if (!game_started) {
                // Cas de secours si init a échoué
                dungeon_generate(&dungeon);
                game_started = true;
            }

           

            Room* current_room = dungeon_get_current_room(&dungeon);

            // update jeu
            const Uint8* keys = SDL_GetKeyboardState(NULL); // quelle touche pressé
            player_update(&player, keys, delta_time, current_time, projectiles, MAX_PROJECTILES, projectile_texture, current_room, &room_rect);

            // Transition de salle
            int room_row = 0;
            int room_col = 0;
            if (world_to_room_cell(&room_rect, player.x + player.w/2, player.y + player.h/2, &room_row, &room_col)) {
                if (room_tile_has(current_room, room_row, room_col, TILE_DOOR)) {
                    int direction = -1;
                    // Déterminer la direction selon la position dans la grille
                    if (room_row == 0) direction = 0; // Haut
                    else if (room_row == ROOM_ROWS - 1) direction = 1; // Bas
                    else if (room_col == 0) direction = 2; // Gauche
                    else if (room_col == ROOM_COLS - 1) direction = 3; // Droite

                    if (direction != -1) {
                        printf("Change de salle vers %d\n", direction);
                        float new_px, new_py;
                        if (dungeon_try_move(&dungeon, direction, &new_px, &new_py)) {
                            printf("Bien changé de salle\n");
                            // Convertir en coordonnées écran
                            player.x = new_px + room_rect.x;
                            player.y = new_py + room_rect.y;
                            
                            // on setup les entités
                            current_room = dungeon_get_current_room(&dungeon);
                            setup_room_entities(current_room, &room_rect, monsters, MAX_MONSTERS,
                                monster_basic_up, monster_basic_down, monster_basic_left, monster_basic_right,
                                monster_tank_up, monster_tank_down, monster_tank_left, monster_tank_right, monster_shooter_left, monster_shooter_right,
                                monster_boss_up, monster_boss_down, monster_boss_left, monster_boss_right,
                                projectile_shooter_texture, projectile_boss_texture);
                            
                            // on setup les projectiles
<<<<<<< HEAD
                            for (int i = 0; i < MAX_PROJECTILES; i++) {
                                projectiles[i].active = false;
                                monster_projectiles[i].active = false;
                            }
=======
                            for (int i = 0; i < MAX_PROJECTILES; i++) projectiles[i].active = false;

                            // Reinitialiser les items de la salle
                            room_item_count = 0;
>>>>>>> 51943c440a6468b9fbb32fc6db4747f11bfb43d9
                        }
                    }
                }
            }

<<<<<<< HEAD
            // Mise à jour des monstres (uniquement si pas de game over)
            if (!game_over_screen) {
                for (int i = 0; i < MAX_MONSTERS; i++) {
                    if (monsters[i].alive) {
                        monster_update(&monsters[i], player.x, player.y, delta_time, current_room, &room_rect, monster_projectiles, MAX_PROJECTILES, projectile_texture, current_time);
                    }
                }
            }

            // Mise à jour des projectiles des monstres (uniquement si pas de game over)
            if (!game_over_screen) {
                for (int i = 0; i < MAX_PROJECTILES; i++) {
                    if (monster_projectiles[i].active) {
                        monster_projectile_update(&monster_projectiles[i], delta_time);

                        SDL_Rect projRect = {
                            (int)monster_projectiles[i].x,
                            (int)monster_projectiles[i].y,
                            (int)monster_projectiles[i].size,
                            (int)monster_projectiles[i].size
                        };

                        // Vérifier collision avec le joueur
                        SDL_Rect playerRect = {
                            (int)player.x,
                            (int)player.y,
                            (int)player.w,
                            (int)player.h
                        };

                        if (SDL_HasIntersection(&projRect, &playerRect)) {
                            monster_projectiles[i].active = false;
                            player.current_health -= (int)monster_projectiles[i].damage;
                            
                            if (player.current_health <= 0) {
                                printf("game over\n");
                                game_over_screen = true;
                                game_over_start_time = SDL_GetTicks();
                            }
                        }
                    }
=======
            // update monstres
            for (int i = 0; i < MAX_MONSTERS; i++) {
                if (monsters[i].alive) {
                    monster_follow(&monsters[i], player.x, player.y, delta_time, current_room, &room_rect);
>>>>>>> 51943c440a6468b9fbb32fc6db4747f11bfb43d9
                }
            }

            // update projectiles
            for (int i = 0; i < MAX_PROJECTILES; i++) {
                if (projectiles[i].active) {
                    projectile_update(&projectiles[i], delta_time);

                    SDL_Rect projRect = {
                        (int)projectiles[i].x,
                        (int)projectiles[i].y,
                        (int)projectiles[i].size,
                        (int)projectiles[i].size
                    };

<<<<<<< HEAD
                    // Vérifier collision avec CHAQUE monstre (seulement si pas game over)
                    if (!game_over_screen) {
                        for (int m = 0; m < MAX_MONSTERS; m++) {
                            if (monsters[m].alive && projectiles[i].active) {
                                SDL_Rect monsterRect = {
                                    (int)monsters[m].x,
                                    (int)monsters[m].y,
                                    (int)monsters[m].w,
                                    (int)monsters[m].h
                                };

                                if (SDL_HasIntersection(&projRect, &monsterRect)) {
                                    projectiles[i].active = false;
                                    monsters[m].current_health -= 1;
                                    if (monsters[m].current_health <= 0) {
                                        monsters[m].alive = false;
                                    }
=======
                    // verif collision avec tous les monstres
                    for (int m = 0; m < MAX_MONSTERS; m++) {
                        if (monsters[m].alive && projectiles[i].active) {
                            SDL_Rect monsterRect = {
                                (int)monsters[m].x,
                                (int)monsters[m].y,
                                (int)monsters[m].w,
                                (int)monsters[m].h
                            };

                            if (SDL_HasIntersection(&projRect, &monsterRect)) {
                                projectiles[i].active = false;
                                monsters[m].current_health -= player.projectile_damage;
                                if (monsters[m].current_health <= 0) {
                                    monsters[m].alive = false;
>>>>>>> 51943c440a6468b9fbb32fc6db4747f11bfb43d9
                                }
                            }
                        }
                    }
                }
            }
            
            //affiche le gameover pendant 5 sec puis revien au menu
            if (game_over_screen) {

                // fond noir transparent
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
                SDL_Rect overlay = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
                SDL_RenderFillRect(renderer, &overlay);

                // affichage du message
                TTF_Font* font = TTF_OpenFont("assets/fonts/Zombie.ttf", 64);
                SDL_Color red = { 255, 0, 0, 255 };
                SDL_Surface* surface = TTF_RenderText_Blended(font, "You Died", red);
                SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

                SDL_Rect pos = { WINDOW_WIDTH / 2 - surface->w / 2, WINDOW_HEIGHT / 2 - surface->h / 2, surface->w, surface->h };
                SDL_RenderCopy(renderer, texture, NULL, &pos);

                SDL_FreeSurface(surface);
                SDL_DestroyTexture(texture);
                TTF_CloseFont(font);

                SDL_RenderPresent(renderer);

                // après 5 secondes - retour menu
                if (SDL_GetTicks() - game_over_start_time >= 2000) {
                    g_menu_state = MENU_STATE_MAIN_MENU;
                    player_initialized = false;
                    game_started = false;
                    game_over_screen = false;
                }

                continue; // empeche le jeu de continue pendant le Game Over
            }
            
            // rendu jeu
            SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
            SDL_RenderClear(renderer);

            render_room(renderer, current_room, &room_rect, tile_floor_texture, tile_rock_texture, tile_door_texture, tile_chest_texture, tile_chest_opened_texture);

            // rendu items
            for (int i = 0; i < room_item_count; i++) {
                item_render(renderer, &room_items[i]);
            }

            player_render(renderer, &player);
            
            for (int i = 0; i < MAX_MONSTERS; i++) {
                if (monsters[i].alive) {
                    monster_render(renderer, &monsters[i]);
                }
            }

            // affichage stats joueur en haut a gauche
            SDL_Color white = { 255, 255, 255, 255 };
            int y_offset = 10;
            
            char hp_text[64];
            sprintf(hp_text, "HP: %d/%d", player.current_health, player.max_health);
            SDL_Surface* hp_surface = TTF_RenderText_Blended(font, hp_text, white);
            SDL_Texture* hp_texture = SDL_CreateTextureFromSurface(renderer, hp_surface);
            SDL_Rect hp_rect = { 20, y_offset, hp_surface->w, hp_surface->h };
            SDL_RenderCopy(renderer, hp_texture, NULL, &hp_rect);
            SDL_FreeSurface(hp_surface);
            SDL_DestroyTexture(hp_texture);
            y_offset += 30;
            
            char dmg_text[64];
            sprintf(dmg_text, "Degats: %.1f", player.projectile_damage);
            SDL_Surface* dmg_surface = TTF_RenderText_Blended(font, dmg_text, white);
            SDL_Texture* dmg_texture = SDL_CreateTextureFromSurface(renderer, dmg_surface);
            SDL_Rect dmg_rect = { 20, y_offset, dmg_surface->w, dmg_surface->h };
            SDL_RenderCopy(renderer, dmg_texture, NULL, &dmg_rect);
            SDL_FreeSurface(dmg_surface);
            SDL_DestroyTexture(dmg_texture);
            y_offset += 30;
            
            char speed_text[64];
            sprintf(speed_text, "Vitesse: %.1f", player.speed);
            SDL_Surface* speed_surface = TTF_RenderText_Blended(font, speed_text, white);
            SDL_Texture* speed_texture = SDL_CreateTextureFromSurface(renderer, speed_surface);
            SDL_Rect speed_rect = { 20, y_offset, speed_surface->w, speed_surface->h };
            SDL_RenderCopy(renderer, speed_texture, NULL, &speed_rect);
            SDL_FreeSurface(speed_surface);
            SDL_DestroyTexture(speed_texture);
            y_offset += 30;
            
            char fire_text[64];
            sprintf(fire_text, "Cadence: %.2f", player.fire_rate);
            SDL_Surface* fire_surface = TTF_RenderText_Blended(font, fire_text, white);
            SDL_Texture* fire_texture = SDL_CreateTextureFromSurface(renderer, fire_surface);
            SDL_Rect fire_rect = { 20, y_offset, fire_surface->w, fire_surface->h };
            SDL_RenderCopy(renderer, fire_texture, NULL, &fire_rect);
            SDL_FreeSurface(fire_surface);
            SDL_DestroyTexture(fire_texture);

            // minimap
            render_minimap(renderer, &dungeon);

            for (int i = 0; i < MAX_PROJECTILES; i++) {
                if (projectiles[i].active) {
                    projectile_render(renderer, &projectiles[i]);
                }
            }

<<<<<<< HEAD
            // Rendu des projectiles des monstres
            for (int i = 0; i < MAX_PROJECTILES; i++) {
                if (monster_projectiles[i].active) {
                    monster_projectile_render(renderer, &monster_projectiles[i]);
                }
            }

            // Hitbox du joueur
=======
            // collision joueur + items
            for (int i = 0; i < room_item_count; i++) {
                if (item_check_collision(&room_items[i], &player)) {
                    item_apply_to_player(&room_items[i], &player);
                    room_items[i].is_collected = true;
                }
            }

            // collision joueur + coffres adjacents
            int player_cell_x = (int)((player.x + player.w / 2 - room_rect.x) / ROOM_CELL_SIZE);
            int player_cell_y = (int)((player.y + player.h / 2 - room_rect.y) / ROOM_CELL_SIZE);
            
            // Vérifier les 4 cases adjacentes au joueur
            int adjacent_cells[4][2] = {
                {player_cell_x, player_cell_y - 1},  // Haut
                {player_cell_x, player_cell_y + 1},  // Bas
                {player_cell_x - 1, player_cell_y},  // Gauche
                {player_cell_x + 1, player_cell_y}   // Droite
            };
            
            for (int i = 0; i < 4; i++) {
                int check_x = adjacent_cells[i][0];
                int check_y = adjacent_cells[i][1];
                
                if (check_x >= 0 && check_x < ROOM_COLS && check_y >= 0 && check_y < ROOM_ROWS) {
                    if (room_tile_has(current_room, check_y, check_x, TILE_CHEST)) {
                        // Vérifier si le joueur touche vraiment le coffre (collision précise)
                        SDL_Rect chest_rect = {
                            room_rect.x + check_x * ROOM_CELL_SIZE,
                            room_rect.y + check_y * ROOM_CELL_SIZE,
                            ROOM_CELL_SIZE,
                            ROOM_CELL_SIZE
                        };
                        
                        SDL_Rect player_rect = {
                            (int)player.x,
                            (int)player.y,
                            (int)player.w,
                            (int)player.h
                        };
                        
                        // collision joueur + coffre
                        if (player_rect.x < chest_rect.x + chest_rect.w &&
                            player_rect.x + player_rect.w > chest_rect.x &&
                            player_rect.y < chest_rect.y + chest_rect.h &&
                            player_rect.y + player_rect.h > chest_rect.y) {
                            
                            // changer le sprite pour coffre ouvert
                            room_remove_tile(current_room, check_y, check_x, TILE_CHEST);
                            room_add_tile(current_room, check_y, check_x, TILE_CHEST_OPENED);
                            
                            if (room_item_count < MAX_ITEMS_PER_ROOM && item_manager && item_manager->count > 0) {
                                // seed avec SDL_GetTicks + position
                                unsigned int seed_mix = SDL_GetTicks() + check_x * 97 + check_y * 37 + room_item_count * 17;
                                srand(seed_mix);
                                int random_value = rand();
                                int random_index = random_value % item_manager->count;
                                printf("DEBUG: %d items, seed=%u, rand=%d, index: %d (%s)\n", 
                                       item_manager->count, seed_mix, random_value, random_index,
                                       item_manager->items[random_index].name);
                                
                                room_items[room_item_count] = item_manager->items[random_index];
                                
                                room_items[room_item_count].x = room_rect.x + check_x * ROOM_CELL_SIZE + (ROOM_CELL_SIZE - room_items[room_item_count].w) / 2;
                                room_items[room_item_count].y = room_rect.y + check_y * ROOM_CELL_SIZE + (ROOM_CELL_SIZE - room_items[room_item_count].h) / 2;
                                room_items[room_item_count].is_collected = false;
                                room_item_count++;
                                
                                printf("Coffre ouvert ! Item '%s' obtenu\n", room_items[room_item_count - 1].name);
                            }
                            
                            break; // Un seul coffre à la fois
                        }
                    }
                }
            }

            // hitbox joueur
>>>>>>> 51943c440a6468b9fbb32fc6db4747f11bfb43d9
            SDL_Rect playerRect = {
                (int)player.x,
                (int)player.y,
                (int)player.w,
                (int)player.h
            };

            // cooldown pour éviter perte instant
            static Uint32 lastHitTime = 0; 
            Uint32 now = SDL_GetTicks();

            // Collision avec les monstres (seulement si pas game over)
            if (!game_over_screen) {
                for (int m = 0; m < MAX_MONSTERS; m++) {    
                    if (!monsters[m].alive) continue;

                    SDL_Rect monsterRect = {
                        (int)monsters[m].x,
                        (int)monsters[m].y,
                        (int)monsters[m].w,
                        (int)monsters[m].h
                    };

                    // collision = le joueur prend -1 hp
                    if (SDL_HasIntersection(&playerRect, &monsterRect)) {
                        if (now - lastHitTime > 600) {  // 0.6s d'invincibilité
                            player.current_health--;

                            printf("le joueur à pris un hit ! HP = %d\n", player.current_health);

                            lastHitTime = now;

                            if (player.current_health <= 0) {
                                printf("game over\n");
                                game_over_screen = true;
                                game_over_start_time = SDL_GetTicks();
                            }
                        }
                    }
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

        // backup si VSync marche pas
        SDL_Delay(1);
    }

    if (level_editor_initialized) {
        level_editor_cleanup(&level_editor);
    }

    // cleanup
    menu_cleanup(&menu);

    if (player_initialized) {
        player_cleanup(&player);
    }

    // cleanup des projectiles
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        projectile_cleanup(&projectiles[i]);
    }

    // cleanup de l'item manager
    if (item_manager) {
        item_manager_destroy(item_manager);
    }

    if (tile_floor_texture != NULL) {
        SDL_DestroyTexture(tile_floor_texture);
    }

    if (tile_rock_texture != NULL) {
        SDL_DestroyTexture(tile_rock_texture);
    }

    if (tile_door_texture != NULL) {
        SDL_DestroyTexture(tile_door_texture);
    }

    if (tile_chest_texture != NULL) {
        SDL_DestroyTexture(tile_chest_texture);
    }

    if (tile_chest_opened_texture != NULL) {
        SDL_DestroyTexture(tile_chest_opened_texture);
    }

    // detruit texture projectiles
    if (projectile_texture != NULL) {
        SDL_DestroyTexture(projectile_texture);
    }

<<<<<<< HEAD
    // Détruire les textures spécifiques des projectiles
    if (projectile_shooter_texture != NULL) {
        SDL_DestroyTexture(projectile_shooter_texture);
    }

    if (projectile_boss_texture != NULL) {
        SDL_DestroyTexture(projectile_boss_texture);
    }

    // Détruire les textures des monstres BASIC
    if (monster_basic_up != NULL) SDL_DestroyTexture(monster_basic_up);
    if (monster_basic_down != NULL) SDL_DestroyTexture(monster_basic_down);
    if (monster_basic_left != NULL) SDL_DestroyTexture(monster_basic_left);
    if (monster_basic_right != NULL) SDL_DestroyTexture(monster_basic_right);

    // Détruire les textures des monstres TANK
    if (monster_tank_up != NULL) SDL_DestroyTexture(monster_tank_up);
    if (monster_tank_down != NULL) SDL_DestroyTexture(monster_tank_down);
    if (monster_tank_left != NULL) SDL_DestroyTexture(monster_tank_left);
    if (monster_tank_right != NULL) SDL_DestroyTexture(monster_tank_right);

    // Détruire les textures des monstres SHOOTER
    if (monster_shooter_left != NULL) SDL_DestroyTexture(monster_shooter_left);
    if (monster_shooter_right != NULL) SDL_DestroyTexture(monster_shooter_right);

    // Détruire les textures du BOSS
    if (monster_boss_up != NULL) SDL_DestroyTexture(monster_boss_up);
    if (monster_boss_down != NULL) SDL_DestroyTexture(monster_boss_down);
    if (monster_boss_left != NULL) SDL_DestroyTexture(monster_boss_left);
    if (monster_boss_right != NULL) SDL_DestroyTexture(monster_boss_right);
=======
    // detruit textures monstres
    if (monster_tex_up != NULL) SDL_DestroyTexture(monster_tex_up);
    if (monster_tex_down != NULL) SDL_DestroyTexture(monster_tex_down);
    if (monster_tex_left != NULL) SDL_DestroyTexture(monster_tex_left);
    if (monster_tex_right != NULL) SDL_DestroyTexture(monster_tex_right);
>>>>>>> 51943c440a6468b9fbb32fc6db4747f11bfb43d9

    // items nettoyes via item_manager

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    TTF_Quit();

    exit(EXIT_SUCCESS);
}
