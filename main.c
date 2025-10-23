#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> // pour les boolean
#include <SDL2/SDL.h> // !!! SDL2 our windows à voir pour linux

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

    // Boucle principale
    bool running = true;
    SDL_Event event;

    printf("Fenêtre SDL2 créée avec succès !\n");
    printf("Appuyez sur ESC ou fermez la fenêtre pour quitter.\n");

    while (running) {
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

        // Rendu
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Fond noir
        SDL_RenderClear(renderer); // Clear toute la fenêtre
        
        // Ici on affichera le jeu

        SDL_RenderPresent(renderer); // Affiche le rendu
    }

    // Nettoyage
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    printf("Programme terminé.\n");
    exit(EXIT_SUCCESS);
}
