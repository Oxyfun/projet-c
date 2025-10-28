#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../utils/sdl_common.h"
#include "../utils/constants.h"
#include "../utils/assets.h"
#include "../player/player.h"
#include "menu.h"

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
    
    // Initialisation du joueur (sera initialisé quand on lance le jeu via le menu)
    Player player;
    bool player_initialized = false;

    // Initialisation des projectiles
    Projectile projectiles[MAX_PROJECTILES];

    // Charger la texture des projectiles
    SDL_Texture* projectile_texture = load_texture(renderer, "assets/images/projectiles/proj.png");
    if (projectile_texture == NULL) {
        printf("Impossible de charger proj.png\n");
    }

    // Initialiser tous les projectiles comme inactifs
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        projectiles[i].active = false;
    }

    // Variables pour le delta time
    Uint32 last_time = SDL_GetTicks();

    // Boucle principale
    bool running = true;
    SDL_Event event;

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
            
            // echap ferme le jeu
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                running = false;
            }
            
            // Événements du menu
            if (g_menu_state == MENU_STATE_MAIN_MENU) {
                menu_update(&menu, &event);
            }
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
        }
        
        // Mise à jour et rendu selon l'état
        if (g_menu_state == MENU_STATE_MAIN_MENU) {
            // Afficher le menu
            menu_render(renderer, &menu);
        } else if (g_menu_state == MENU_STATE_GAME && player_initialized) {
            // Mise à jour du jeu
            const Uint8* keys = SDL_GetKeyboardState(NULL); // quelle touche pressé
            player_update(&player, keys, delta_time, current_time, projectiles, MAX_PROJECTILES, projectile_texture);
            
            // Mise à jour des projectiles
            for (int i = 0; i < MAX_PROJECTILES; i++) {
                if (projectiles[i].active) {
                    projectile_update(&projectiles[i], delta_time);
                }
            }
            
            // Afficher le jeu
            SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
            SDL_RenderClear(renderer);
            
            player_render(renderer, &player);
            
            for (int i = 0; i < MAX_PROJECTILES; i++) {
                if (projectiles[i].active) {
                    projectile_render(renderer, &projectiles[i]);
                }
            }
        } else if (g_menu_state == MENU_STATE_LEVEL_EDITOR) {
            // Afficher l'éditeur
            SDL_SetRenderDrawColor(renderer, 30, 30, 50, 255);
            SDL_RenderClear(renderer);
            // à faire
        }

        SDL_RenderPresent(renderer);
        
        // Backup si VSync ne marche pas
        SDL_Delay(1);
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
    
    // Détruire la texture partagée des projectiles
    if (projectile_texture != NULL) {
        SDL_DestroyTexture(projectile_texture);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    exit(EXIT_SUCCESS);
}
