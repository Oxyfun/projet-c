#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../utils/sdl_common.h"
#include "../player/player.h"
#include "menu.h"

// Constantes pour la fenêtre du jeu
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_TITLE "Binding of Isaac"

int main(int argc, char* argv[]) {
    // Initialisation de SDL + vérif
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur lors de l'initialisation de SDL: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // Initialisation de SDL_image
    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
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
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    // Création du renderer avec VSync pour limiter automatiquement les FPS (car ça lag quand on ferme le jeu)
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        printf("Erreur lors de la création du renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
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
    enum { MAX_PROJECTILES = 50 };
    Projectile projectiles[MAX_PROJECTILES];

    // Initialiser tous les projectiles comme inactifs
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        projectiles[i].active = false;
        projectiles[i].texture = NULL;
    }

    // Variables pour le delta time
    Uint32 last_time = SDL_GetTicks();

    // Boucle principale
    bool running = true;
    SDL_Event event;

    while (running) {
        // Calcul du delta time, si ya pas ça la vitesse du joueur sera proportionnelle aux FPS
        Uint32 current_time = SDL_GetTicks();
        float delta_time = (current_time - last_time) / 1000.0f;
        last_time = current_time;

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
        
        if (g_menu_state == MENU_STATE_GAME && player_initialized) {
            // Mise à jour du jeu
            const Uint8* keys = SDL_GetKeyboardState(NULL); // quelle touche pressé
            player_update(&player, keys, delta_time, projectiles, MAX_PROJECTILES, renderer);
            
            // Mise à jour des projectiles
            for (int i = 0; i < MAX_PROJECTILES; i++) {
                if (projectiles[i].active) {
                    projectile_update(&projectiles[i], delta_time);
                }
            }
        } else if (g_menu_state == MENU_STATE_LEVEL_EDITOR) {
            // ajouter ici l'éditeur de niveau dans le futur
        }

        SDL_RenderClear(renderer);
        
        if (g_menu_state == MENU_STATE_MAIN_MENU) {
            // Afficher le menu
            menu_render(renderer, &menu);
        } else if (g_menu_state == MENU_STATE_GAME && player_initialized) {
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
        if (projectiles[i].texture != NULL) {
            projectile_cleanup(&projectiles[i]);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    exit(EXIT_SUCCESS);
}

