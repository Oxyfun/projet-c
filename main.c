#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> // pour les boolean
#include <SDL2/SDL.h> // !!! SDL2 pour windows à voir pour linux
#include <SDL2/SDL_image.h>
#include "player.h" //  structure du joueur

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
    
    // Variables pour le delta time
    Uint32 last_time = SDL_GetTicks();
    
    // Boucle principale
    bool running = true;
    SDL_Event event;

    printf("Fenêtre SDL2 créée avec succès !\n");
    printf("Utilisez ZQSD ou les flèches pour bouger.\n");
    printf("Appuyez sur ESC ou fermez la fenêtre pour quitter.\n");

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
        
        // Mise à jour du joueur
        const Uint8* keys = SDL_GetKeyboardState(NULL);
        player_update(&player, keys, delta_time);

        // Rendu
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255); // Fond gris
        SDL_RenderClear(renderer); // Clear toute la fenêtre
        
        // Rendu du joueur
        player_render(renderer, &player); // Afiche le joueur

        SDL_RenderPresent(renderer); // Affiche le rendu
    }

    // Nettoyage
    player_cleanup(&player);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    printf("Programme terminé.\n");
    exit(EXIT_SUCCESS);
}
