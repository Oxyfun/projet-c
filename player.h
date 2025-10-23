#ifndef PLAYER_H // Au cas ou on inclut plusieurs fois le fichier
#define PLAYER_H

#include <SDL2/SDL.h>
#include <stdbool.h>

// Structure joueur - version basique et optimisée
typedef struct {
    // Position
    float x, y;
    
    // Vitesse
    float vx, vy;
    
    // Dimensions
    float w, h;
    
    // État
    bool alive;
    
    // Textures pour les différentes directions
    SDL_Texture* texture_up;    // personnage_haut.png
    SDL_Texture* texture_down; // personnage_bas.png
    SDL_Texture* texture_left; // personnage_gauche.png
    SDL_Texture* texture_right; // personnage_droite.png
    SDL_Texture* current_texture; // Texture actuellement affichée
    
    // Direction actuelle
    int direction; // 0=bas, 1=haut, 2=gauche, 3=droite
} Player;

// Fonctions essentielles
void player_init(Player* p, SDL_Renderer* renderer);
void player_update(Player* p, const Uint8* keys, float dt);
void player_render(SDL_Renderer* r, Player* p);
void player_cleanup(Player* p);

#endif
