#include "assets.h"
#include <stdio.h>

// Fonction pour charger une texture depuis un fichier
SDL_Texture* load_texture(SDL_Renderer* renderer, const char* path) {
    SDL_Surface* surface = IMG_Load(path);
    if (surface == NULL) {
        printf("Erreur chargement image %s: %s\n", path, IMG_GetError());
        return NULL;
    }
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    if (texture == NULL) {
        printf("Erreur création texture %s: %s\n", path, SDL_GetError());
        return NULL;
    }
    
    return texture;
}
