#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL_image.h>
#include "decor.h"

// Déclaration externe de la fonction load_texture définie dans player.c
extern SDL_Texture* load_texture(SDL_Renderer* renderer, const char* path);

// Initialisation d'un élément de décor avec coordonnées de grille
void decor_element_init(DecorElement* element, int grid_x, int grid_y, DecorType type, int id, SDL_Renderer* renderer) {
    element->grid_x = grid_x;
    element->grid_y = grid_y;
    element->type = type;
    element->id = id;
    element->solid = decor_is_solid(type);
    element->destructible = decor_is_destructible(type);
    element->active = true;
    
    // Conversion des coordonnées de grille en pixels
    decor_grid_to_pixel(grid_x, grid_y, &element->x, &element->y);
    element->w = GRID_CELL_SIZE;
    element->h = GRID_CELL_SIZE;
    
    // Chargement de la texture
    const char* texture_path = decor_get_texture_path(type);
    element->texture = load_texture(renderer, texture_path);
    
    if (element->texture == NULL) {
        printf("ATTENTION: Impossible de charger %s pour le type %d\n", texture_path, type);
    }
}

// Rendu d'un élément de décor
void decor_element_render(SDL_Renderer* renderer, DecorElement* element) {
    if (!element->active) return;
    
    SDL_Rect rect = {
        (int)element->x,
        (int)element->y,
        (int)element->w,
        (int)element->h
    };
    
    // Rendu de la texture si disponible
    if (element->texture != NULL) {
        SDL_RenderCopy(renderer, element->texture, NULL, &rect);
    } else {
        // Fallback : couleur selon le type
        switch (element->type) {
            case DECOR_FLOOR:
                SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); // Gris pour le sol
                break;
            case DECOR_ROCK:
                SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);   // Gris foncé pour les pierres
                break;
        }
        SDL_RenderFillRect(renderer, &rect);
    }
}

// Nettoyage d'un élément de décor
void decor_element_cleanup(DecorElement* element) {
    if (element->texture != NULL) {
        SDL_DestroyTexture(element->texture);
        element->texture = NULL;
    }
}

// Initialisation du gestionnaire de grille
void decor_manager_init(DecorManager* manager, int grid_width, int grid_height, int capacity) {
    // Vérifier les limites
    if (grid_width > MAX_GRID_WIDTH) grid_width = MAX_GRID_WIDTH;
    if (grid_height > MAX_GRID_HEIGHT) grid_height = MAX_GRID_HEIGHT;
    
    manager->grid_width = grid_width;
    manager->grid_height = grid_height;
    manager->next_id = 1;
    
    // Initialiser la grille avec des cases vides
    for (int y = 0; y < grid_height; y++) {
        for (int x = 0; x < grid_width; x++) {
            manager->grid[y][x] = DECOR_EMPTY;
        }
    }
    
    // Allouer la mémoire pour les éléments
    manager->elements = (DecorElement*)malloc(capacity * sizeof(DecorElement));
    if (manager->elements == NULL) {
        printf("Erreur: Impossible d'allouer la mémoire pour les éléments de décor\n");
        manager->capacity = 0;
        manager->count = 0;
        return;
    }
    
    manager->capacity = capacity;
    manager->count = 0;
    
    // Initialiser tous les éléments comme inactifs
    for (int i = 0; i < capacity; i++) {
        manager->elements[i].active = false;
        manager->elements[i].texture = NULL;
    }
}

// Placer un élément dans une cellule de la grille
void decor_manager_set_grid_cell(DecorManager* manager, int grid_x, int grid_y, DecorType type, SDL_Renderer* renderer) {
    // Vérifier les limites de la grille
    if (grid_x < 0 || grid_x >= manager->grid_width || 
        grid_y < 0 || grid_y >= manager->grid_height) {
        printf("ATTENTION: Coordonnées de grille hors limites (%d, %d)\n", grid_x, grid_y);
        return;
    }
    
    // Si la case n'est pas vide, la vider d'abord
    if (manager->grid[grid_y][grid_x] != DECOR_EMPTY) {
        decor_manager_clear_grid_cell(manager, grid_x, grid_y);
    }
    
    // Si c'est une case vide, ne rien faire
    if (type == DECOR_EMPTY) {
        return;
    }
    
    // Vérifier la capacité
    if (manager->count >= manager->capacity) {
        printf("ATTENTION: Capacité maximale du gestionnaire de décor atteinte\n");
        return;
    }
    
    // Trouver un slot libre
    for (int i = 0; i < manager->capacity; i++) {
        if (!manager->elements[i].active) {
            // Initialiser l'élément
            decor_element_init(&manager->elements[i], grid_x, grid_y, type, manager->next_id, renderer);
            
            // Mettre à jour la grille
            manager->grid[grid_y][grid_x] = manager->next_id;
            
            // Incrémenter les compteurs
            manager->next_id++;
            manager->count++;
            
            printf("Élément placé à la grille (%d, %d) avec ID %d\n", grid_x, grid_y, manager->next_id - 1);
            return;
        }
    }
}

// Vider une cellule de la grille
void decor_manager_clear_grid_cell(DecorManager* manager, int grid_x, int grid_y) {
    // Vérifier les limites
    if (grid_x < 0 || grid_x >= manager->grid_width || 
        grid_y < 0 || grid_y >= manager->grid_height) {
        return;
    }
    
    int element_id = manager->grid[grid_y][grid_x];
    if (element_id == DECOR_EMPTY) {
        return; // Déjà vide
    }
    
    // Trouver et supprimer l'élément
    for (int i = 0; i < manager->capacity; i++) {
        if (manager->elements[i].active && manager->elements[i].id == element_id) {
            decor_element_cleanup(&manager->elements[i]);
            manager->elements[i].active = false;
            manager->count--;
            break;
        }
    }
    
    // Vider la grille
    manager->grid[grid_y][grid_x] = DECOR_EMPTY;
}

// Rendu de tous les éléments de décor
void decor_manager_render_all(SDL_Renderer* renderer, DecorManager* manager) {
    for (int i = 0; i < manager->capacity; i++) {
        if (manager->elements[i].active) {
            decor_element_render(renderer, &manager->elements[i]);
        }
    }
}

// Nettoyage du gestionnaire de décor
void decor_manager_cleanup(DecorManager* manager) {
    if (manager->elements != NULL) {
        // Nettoyer tous les éléments
        for (int i = 0; i < manager->capacity; i++) {
            decor_element_cleanup(&manager->elements[i]);
        }
        
        free(manager->elements);
        manager->elements = NULL;
    }
    
    manager->capacity = 0;
    manager->count = 0;
}

// Conversion des coordonnées de grille en pixels
void decor_grid_to_pixel(int grid_x, int grid_y, float* pixel_x, float* pixel_y) {
    *pixel_x = grid_x * GRID_CELL_SIZE;
    *pixel_y = grid_y * GRID_CELL_SIZE;
}

// Conversion des coordonnées de pixels en grille
void decor_pixel_to_grid(float pixel_x, float pixel_y, int* grid_x, int* grid_y) {
    *grid_x = (int)(pixel_x / GRID_CELL_SIZE);
    *grid_y = (int)(pixel_y / GRID_CELL_SIZE);
}

// Obtenir l'élément à une position de grille donnée
DecorElement* decor_get_element_at_grid(DecorManager* manager, int grid_x, int grid_y) {
    // Vérifier les limites
    if (grid_x < 0 || grid_x >= manager->grid_width || 
        grid_y < 0 || grid_y >= manager->grid_height) {
        return NULL;
    }
    
    int element_id = manager->grid[grid_y][grid_x];
    if (element_id == DECOR_EMPTY) {
        return NULL;
    }
    
    // Trouver l'élément par ID
    for (int i = 0; i < manager->capacity; i++) {
        if (manager->elements[i].active && manager->elements[i].id == element_id) {
            return &manager->elements[i];
        }
    }
    
    return NULL;
}

// Obtenir le chemin de la texture selon le type
const char* decor_get_texture_path(DecorType type) {
    switch (type) {
        case DECOR_FLOOR:
            return "assets/images/decor/sol.png";
        case DECOR_ROCK:
            return "assets/images/decor/rock.png";
        default:
            return "assets/images/decor/sol.png"; // Par défaut
    }
}

// Vérifier si un type de décor est solide
bool decor_is_solid(DecorType type) {
    switch (type) {
        case DECOR_EMPTY:
        case DECOR_FLOOR:
            return false;  // Le sol et les cases vides ne sont pas solides
        case DECOR_ROCK:
            return true;   // Les pierres sont solides
        default:
            return false;
    }
}

// Vérifier si un type de décor est destructible
bool decor_is_destructible(DecorType type) {
    switch (type) {
        case DECOR_EMPTY:
        case DECOR_FLOOR:
            return false;  // Le sol et les cases vides ne sont pas destructibles
        case DECOR_ROCK:
            return true;   // Les pierres peuvent être détruites
        default:
            return false;
    }
}

// Fonction de collision pour le joueur (vérifie les 4 coins intelligemment)
bool decor_check_collision_with_grid(void* manager, float x, float y, float w, float h) {
    DecorManager* decor_manager = (DecorManager*)manager;
    if (decor_manager == NULL) return false;
    
    // Convertir les coordonnées de pixels en grille
    int grid_x, grid_y;
    decor_pixel_to_grid(x, y, &grid_x, &grid_y);
    
    // Calculer les 4 coins de l'objet en coordonnées de grille
    int corners[4][2] = {
        {grid_x, grid_y},                           // Coin haut-gauche
        {grid_x + (int)(w/GRID_CELL_SIZE), grid_y}, // Coin haut-droit
        {grid_x, grid_y + (int)(h/GRID_CELL_SIZE)}, // Coin bas-gauche
        {grid_x + (int)(w/GRID_CELL_SIZE), grid_y + (int)(h/GRID_CELL_SIZE)} // Coin bas-droit
    };
    
    // Vérifier chaque coin
    for (int i = 0; i < 4; i++) {
        DecorElement* element = decor_get_element_at_grid(decor_manager, corners[i][0], corners[i][1]);
        if (element != NULL && element->solid) {
            return true; // Collision détectée
        }
    }
    
    return false; // Pas de collision
}

// Collision horizontale (gauche/droite)
// Vérifie plusieurs points sur la hauteur pour détecter toutes les collisions
// Si au moins un point touche un obstacle, le mouvement est bloqué
bool decor_check_horizontal_collision(void* manager, float x, float y, float w, float h) {
    DecorManager* decor_manager = (DecorManager*)manager;
    if (decor_manager == NULL) return false;
    
    // Convertir les coordonnées de pixels en grille
    int grid_x, grid_y;
    decor_pixel_to_grid(x, y, &grid_x, &grid_y);
    
    // Vérifier 3 points sur la hauteur (haut, milieu, bas)
    // Ça évite que le joueur passe à travers les obstacles en coin
    int check_points = 3;
    float step = h / (check_points - 1);
    
    for (int i = 0; i < check_points; i++) {
        float check_y = y + i * step;
        int dummy_x, check_grid_y;
        decor_pixel_to_grid(0, check_y, &dummy_x, &check_grid_y);
        
        // Vérifier le côté gauche
        DecorElement* left_element = decor_get_element_at_grid(decor_manager, grid_x, check_grid_y);
        if (left_element != NULL && left_element->solid) {
            return true; // Collision !
        }
        
        // Vérifier le côté droit
        DecorElement* right_element = decor_get_element_at_grid(decor_manager, grid_x + (int)(w/GRID_CELL_SIZE), check_grid_y);
        if (right_element != NULL && right_element->solid) {
            return true; // Collision !
        }
    }
    
    return false; // Pas de collision
}

// Collision verticale haut
// Vérifie plusieurs points sur la largeur pour détecter les collisions avec le plafond
// Si au moins un point touche un obstacle, le mouvement vers le haut est bloqué
bool decor_check_vertical_collision_up(void* manager, float x, float y, float w, float h) {
    DecorManager* decor_manager = (DecorManager*)manager;
    if (decor_manager == NULL) return false;
    
    // Convertir les coordonnées de pixels en grille
    int grid_x, grid_y;
    decor_pixel_to_grid(x, y, &grid_x, &grid_y);
    
    // Vérifier 3 points sur la largeur (gauche, centre, droite)
    // Ça évite que le joueur passe à travers les obstacles en coin
    int check_points = 3;
    float step = w / (check_points - 1);
    
    for (int i = 0; i < check_points; i++) {
        float check_x = x + i * step;
        int check_grid_x, dummy_y;
        decor_pixel_to_grid(check_x, 0, &check_grid_x, &dummy_y);
        
        // Vérifier collision avec le plafond
        DecorElement* top_element = decor_get_element_at_grid(decor_manager, check_grid_x, grid_y);
        if (top_element != NULL && top_element->solid) {
            return true; // Collision !
        }
    }
    
    return false; // Pas de collision
}

// Collision verticale bas
// Vérifie plusieurs points sur la largeur pour détecter les collisions avec le sol
// Si au moins un point touche un obstacle, le mouvement vers le bas est bloqué
bool decor_check_vertical_collision_down(void* manager, float x, float y, float w, float h) {
    DecorManager* decor_manager = (DecorManager*)manager;
    if (decor_manager == NULL) return false;
    
    // Convertir les coordonnées de pixels en grille
    int grid_x, grid_y;
    decor_pixel_to_grid(x, y, &grid_x, &grid_y);
    
    // Calculer la position future des pieds du joueur
    // Ça évite que le joueur "flotte" au-dessus du sol
    float future_feet_y = y + h - 8; // Les 8 pixels du bas représentent les pieds
    int dummy_x, future_feet_grid_y;
    decor_pixel_to_grid(0, future_feet_y, &dummy_x, &future_feet_grid_y);
    
    // Vérifier 3 points sur la largeur (gauche, centre, droite)
    // Ça évite que le joueur passe à travers les obstacles en coin
    int check_points = 3;
    float step = w / (check_points - 1);
    
    for (int i = 0; i < check_points; i++) {
        float check_x = x + i * step;
        int check_grid_x, dummy_y;
        decor_pixel_to_grid(check_x, 0, &check_grid_x, &dummy_y);
        
        // Vérifier collision avec le sol
        DecorElement* bottom_element = decor_get_element_at_grid(decor_manager, check_grid_x, future_feet_grid_y);
        if (bottom_element != NULL && bottom_element->solid) {
            return true; // Collision !
        }
    }
    
    return false; // Pas de collision
}

// Fonction de collision pour les projectiles (vérifie le centre)
bool decor_check_projectile_collision(void* manager, float x, float y, float size) {
    DecorManager* decor_manager = (DecorManager*)manager;
    if (decor_manager == NULL) return false;
    
    int grid_x, grid_y;
    decor_pixel_to_grid(x, y, &grid_x, &grid_y);
    
    DecorElement* element = decor_get_element_at_grid(decor_manager, grid_x, grid_y);
    return (element != NULL && element->solid);
}
