#ifndef DECOR_H
#define DECOR_H

#include <SDL2/SDL.h>
#include <stdbool.h>

// Types d'éléments de décor
typedef enum {
    DECOR_EMPTY = 0,    // Case vide
    DECOR_FLOOR = 1,    // Sol
    DECOR_ROCK = 2      // Pierre
} DecorType;

// Configuration de la grille
#define GRID_CELL_SIZE 32    // Taille d'une cellule en pixels
#define MAX_GRID_WIDTH 50    // Largeur maximale de la grille
#define MAX_GRID_HEIGHT 30   // Hauteur maximale de la grille

// Structure pour un élément de décor
typedef struct {
    // Position et dimensions
    float x, y;         // Position en pixels
    float w, h;         // Largeur et hauteur
    
    // Position dans la grille
    int grid_x, grid_y; // Coordonnées dans la grille
    
    // Type d'élément
    DecorType type;     // Type de décor (sol ou pierre)
    int id;             // ID unique de l'élément
    
    // Propriétés physiques
    bool solid;         // Si l'élément est solide (bloque le passage)
    bool destructible;  // Si l'élément peut être détruit
    
    // Visuel
    SDL_Texture* texture;   // Texture de l'élément
    
    // État
    bool active;        // Si l'élément est actif/visible
} DecorElement;

// Structure pour gérer une grille de décor
typedef struct {
    // Grille 2D pour stocker les IDs des éléments
    int grid[MAX_GRID_HEIGHT][MAX_GRID_WIDTH];
    
    // Dimensions de la grille
    int grid_width;
    int grid_height;
    
    // Éléments de décor actifs
    DecorElement* elements;  // Tableau d'éléments
    int count;              // Nombre d'éléments actifs
    int capacity;           // Capacité maximale du tableau
    
    // Compteur d'ID pour les nouveaux éléments
    int next_id;
} DecorManager;

// Fonctions pour les éléments de décor
void decor_element_init(DecorElement* element, int grid_x, int grid_y, DecorType type, int id, SDL_Renderer* renderer);
void decor_element_render(SDL_Renderer* renderer, DecorElement* element);
void decor_element_cleanup(DecorElement* element);

// Fonctions pour le gestionnaire de grille
void decor_manager_init(DecorManager* manager, int grid_width, int grid_height, int capacity);
void decor_manager_set_grid_cell(DecorManager* manager, int grid_x, int grid_y, DecorType type, SDL_Renderer* renderer);
void decor_manager_clear_grid_cell(DecorManager* manager, int grid_x, int grid_y);
void decor_manager_render_all(SDL_Renderer* renderer, DecorManager* manager);
void decor_manager_cleanup(DecorManager* manager);

// Fonctions utilitaires pour la grille
void decor_grid_to_pixel(int grid_x, int grid_y, float* pixel_x, float* pixel_y);
void decor_pixel_to_grid(float pixel_x, float pixel_y, int* grid_x, int* grid_y);
DecorElement* decor_get_element_at_grid(DecorManager* manager, int grid_x, int grid_y);

// Fonctions utilitaires pour les types
const char* decor_get_texture_path(DecorType type);
bool decor_is_solid(DecorType type);
bool decor_is_destructible(DecorType type);

// Fonctions de collision
bool decor_check_collision_with_grid(void* manager, float x, float y, float w, float h);
bool decor_check_horizontal_collision(void* manager, float x, float y, float w, float h);
bool decor_check_vertical_collision_up(void* manager, float x, float y, float w, float h);
bool decor_check_vertical_collision_down(void* manager, float x, float y, float w, float h);
bool decor_check_projectile_collision(void* manager, float x, float y, float size);

#endif
