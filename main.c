#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> // pour les boolean
#include <SDL2/SDL.h> // !!! SDL2 pour windows à voir pour linux
#include <SDL2/SDL_image.h>
#include "player.h" //  structure du joueur
#include "decor.h"  // système de grille pour le décor

// Constantes pour la fenêtre du jeu
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_TITLE "Binding of Isaac"

int main(int argc, char *argv[]) {
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

    // Création du renderer, c'est ce qui y aura dans la fenêtre
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Erreur lors de la création du renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    // Initialisation du joueur
    Player player;
    player_init(&player, renderer);
    
    // Initialisation du système de grille pour le décor
    // La grille permet de placer des éléments (sol, rochers) à des positions précises
    DecorManager decor_manager;
    decor_manager_init(&decor_manager, 25, 19, 200); // Grille 25x19 cases, max 200 éléments
    
    // Création du niveau de test avec rochers et sol
    printf("Création du niveau de test...\n");
    
    // Créer le sol de base (ligne du bas de la grille)
    // Le joueur marchera sur ces éléments de sol
    int x, y;
    for (x = 0; x < 25; x++) {
        decor_manager_set_grid_cell(&decor_manager, x, 18, DECOR_FLOOR, renderer);
    }
    
    // Créer des murs sur les côtés pour délimiter la zone de jeu
    // Ces murs empêchent le joueur de sortir de la zone
    for (y = 0; y < 19; y++) {
        decor_manager_set_grid_cell(&decor_manager, 0, y, DECOR_ROCK, renderer);  // Mur gauche
        decor_manager_set_grid_cell(&decor_manager, 24, y, DECOR_ROCK, renderer); // Mur droit
    }
    
    // Créer des obstacles avec des rochers pour tester les collisions
    // Obstacle 1: Ligne horizontale de rochers (le joueur ne peut pas passer à travers)
    for (x = 5; x < 10; x++) {
        decor_manager_set_grid_cell(&decor_manager, x, 10, DECOR_ROCK, renderer);
    }
    
    // Obstacle 2: Ligne verticale de rochers (mur vertical)
    for (y = 5; y < 12; y++) {
        decor_manager_set_grid_cell(&decor_manager, 15, y, DECOR_ROCK, renderer);
    }
    
    // Obstacle 3: Forme en L (pour tester les collisions en coin)
    decor_manager_set_grid_cell(&decor_manager, 8, 5, DECOR_ROCK, renderer);
    decor_manager_set_grid_cell(&decor_manager, 9, 5, DECOR_ROCK, renderer);
    decor_manager_set_grid_cell(&decor_manager, 8, 6, DECOR_ROCK, renderer);
    
    // Quelques rochers isolés pour créer un niveau intéressant
    decor_manager_set_grid_cell(&decor_manager, 3, 8, DECOR_ROCK, renderer);
    decor_manager_set_grid_cell(&decor_manager, 20, 7, DECOR_ROCK, renderer);
    decor_manager_set_grid_cell(&decor_manager, 12, 14, DECOR_ROCK, renderer);
    
    printf("Niveau créé avec %d éléments de décor\n", decor_manager.count);
    
    // Initialisation des projectiles
    const int MAX_PROJECTILES = 50;  // Maximum 50 projectiles à la fois
    Projectile projectiles[MAX_PROJECTILES];
    
    // Initialiser tous les projectiles comme inactifs
    int i;
    for (i = 0; i < MAX_PROJECTILES; i++) {
        projectiles[i].active = false;
    }
    
    // Variables pour le delta time
    Uint32 last_time = SDL_GetTicks();
    
    // Boucle principale
    bool running = true;
    SDL_Event event;

    printf("Jeu lance !\n");
    printf("ZQSD pour bouger, fleches pour tirer\n");
    printf("ESC pour quitter\n");

    while (running) {
        // Calcul du delta time, si ya pas ça la vitesse du joueur sera proportionnelle aux FPS
        Uint32 current_time = SDL_GetTicks();
        float delta_time = (current_time - last_time) / 1000.0f;
        last_time = current_time;
        
        // Gestion des événements
        while (SDL_PollEvent(&event)) { // On récupère les événements
            switch (event.type) {
                case SDL_QUIT: // Si on ferme la fenêtre via la croix
                    running = false;
                    break;
                case SDL_KEYDOWN: // Si on appuie sur une touche
                    if (event.key.keysym.sym == SDLK_ESCAPE) { // Si on appuie sur ESC
                        running = false;
                    }
                    break;
            }
        }
        
        // Mise à jour du joueur avec collision
        const Uint8* keys = SDL_GetKeyboardState(NULL);
        player_update(&player, keys, delta_time, projectiles, MAX_PROJECTILES, renderer, &decor_manager);
        
        // Mise à jour des projectiles avec collision
        for (i = 0; i < MAX_PROJECTILES; i++) {
            if (projectiles[i].active) {
                projectile_update(&projectiles[i], delta_time, &decor_manager);
            }
        }

        // Rendu
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255); // Fond gris
        SDL_RenderClear(renderer); // Clear toute la fenêtre
        
        // Rendu des éléments de décor (en premier, en arrière-plan)
        decor_manager_render_all(renderer, &decor_manager);
        
        // Rendu du joueur
        player_render(renderer, &player); // Affiche le joueur
        
        // Rendu des projectiles
        for (i = 0; i < MAX_PROJECTILES; i++) {
            if (projectiles[i].active) {
                projectile_render(renderer, &projectiles[i]);
            }
        }

        SDL_RenderPresent(renderer); // Affiche le rendu
    }

    // Nettoyage
    player_cleanup(&player);
    
    // Nettoyage des éléments de décor
    decor_manager_cleanup(&decor_manager);
    
    // Nettoyage des projectiles
    for (i = 0; i < MAX_PROJECTILES; i++) {
        projectile_cleanup(&projectiles[i]);
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    printf("Programme terminé.\n");
    exit(EXIT_SUCCESS);
}
