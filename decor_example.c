// Exemple d'utilisation du système de grille pour les éléments de décor
// Ce fichier montre comment utiliser la grille pour créer des niveaux

#include "decor.h"

// Exemple de fonction pour créer un niveau avec la grille
void create_grid_level_example(DecorManager* decor_manager, SDL_Renderer* renderer) {
    // Créer une grille de 25x19 cases (800x608 pixels avec des cellules de 32x32)
    decor_manager_init(decor_manager, 25, 19, 200);
    
    // Créer un sol de base (ligne du bas)
    for (int x = 0; x < 25; x++) {
        decor_manager_set_grid_cell(decor_manager, x, 18, DECOR_FLOOR, renderer);
    }
    
    // Ajouter des murs sur les côtés
    for (int y = 0; y < 19; y++) {
        decor_manager_set_grid_cell(decor_manager, 0, y, DECOR_ROCK, renderer);  // Mur gauche
        decor_manager_set_grid_cell(decor_manager, 24, y, DECOR_ROCK, renderer); // Mur droit
    }
    
    // Créer des obstacles avec des pierres
    // Obstacle 1: Ligne de pierres horizontale
    for (int x = 5; x < 10; x++) {
        decor_manager_set_grid_cell(decor_manager, x, 10, DECOR_ROCK, renderer);
    }
    
    // Obstacle 2: Ligne de pierres verticale
    for (int y = 5; y < 12; y++) {
        decor_manager_set_grid_cell(decor_manager, 15, y, DECOR_ROCK, renderer);
    }
    
    // Obstacle 3: Forme en L
    decor_manager_set_grid_cell(decor_manager, 8, 5, DECOR_ROCK, renderer);
    decor_manager_set_grid_cell(decor_manager, 9, 5, DECOR_ROCK, renderer);
    decor_manager_set_grid_cell(decor_manager, 8, 6, DECOR_ROCK, renderer);
    
    printf("Niveau créé avec la grille: %d éléments actifs\n", decor_manager->count);
}

// Exemple de fonction pour créer un niveau depuis un tableau 2D
void create_level_from_array(DecorManager* decor_manager, SDL_Renderer* renderer) {
    // Exemple de niveau représenté par un tableau 2D
    // 0 = vide, 1 = sol, 2 = pierre
    int level_data[10][15] = {
        {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2}, // Ligne 0
        {2,0,0,0,0,0,0,0,0,0,0,0,0,0,2}, // Ligne 1
        {2,0,2,0,0,0,0,0,0,0,0,0,2,0,2}, // Ligne 2
        {2,0,0,0,0,0,0,0,0,0,0,0,0,0,2}, // Ligne 3
        {2,0,0,0,2,2,2,0,0,0,2,2,2,0,2}, // Ligne 4
        {2,0,0,0,0,0,0,0,0,0,0,0,0,0,2}, // Ligne 5
        {2,0,0,0,0,0,0,0,0,0,0,0,0,0,2}, // Ligne 6
        {2,0,0,0,0,0,0,0,0,0,0,0,0,0,2}, // Ligne 7
        {2,0,0,0,0,0,0,0,0,0,0,0,0,0,2}, // Ligne 8
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}  // Ligne 9 (sol)
    };
    
    // Initialiser le gestionnaire
    decor_manager_init(decor_manager, 15, 10, 100);
    
    // Créer le niveau depuis le tableau
    for (int y = 0; y < 10; y++) {
        for (int x = 0; x < 15; x++) {
            DecorType type = (DecorType)level_data[y][x];
            if (type != DECOR_EMPTY) {
                decor_manager_set_grid_cell(decor_manager, x, y, type, renderer);
            }
        }
    }
    
    printf("Niveau créé depuis le tableau: %d éléments actifs\n", decor_manager->count);
}

// Exemple de fonction pour vérifier les collisions avec la grille
bool check_collision_with_grid(DecorManager* decor_manager, float x, float y, float w, float h) {
    // Convertir les coordonnées de pixels en grille
    int grid_x, grid_y;
    decor_pixel_to_grid(x, y, &grid_x, &grid_y);
    
    // Vérifier les 4 coins de l'objet
    int corners[4][2] = {
        {grid_x, grid_y},                           // Coin haut-gauche
        {grid_x + (int)(w/GRID_CELL_SIZE), grid_y}, // Coin haut-droit
        {grid_x, grid_y + (int)(h/GRID_CELL_SIZE)}, // Coin bas-gauche
        {grid_x + (int)(w/GRID_CELL_SIZE), grid_y + (int)(h/GRID_CELL_SIZE)} // Coin bas-droit
    };
    
    for (int i = 0; i < 4; i++) {
        DecorElement* element = decor_get_element_at_grid(decor_manager, corners[i][0], corners[i][1]);
        if (element != NULL && element->solid) {
            return true; // Collision détectée
        }
    }
    
    return false; // Pas de collision
}

// Exemple de fonction pour détruire un élément à une position donnée
void destroy_element_at_position(DecorManager* decor_manager, float x, float y) {
    int grid_x, grid_y;
    decor_pixel_to_grid(x, y, &grid_x, &grid_y);
    
    DecorElement* element = decor_get_element_at_grid(decor_manager, grid_x, grid_y);
    if (element != NULL && element->destructible) {
        decor_manager_clear_grid_cell(decor_manager, grid_x, grid_y);
        printf("Élément destructible détruit à la grille (%d, %d)\n", grid_x, grid_y);
    }
}

// Exemple de fonction pour afficher l'état de la grille (debug)
void print_grid_state(DecorManager* decor_manager) {
    printf("\nÉtat de la grille (%dx%d):\n", decor_manager->grid_width, decor_manager->grid_height);
    printf("Légende: . = vide, S = sol, R = rocher\n\n");
    
    for (int y = 0; y < decor_manager->grid_height; y++) {
        for (int x = 0; x < decor_manager->grid_width; x++) {
            int cell_value = decor_manager->grid[y][x];
            switch (cell_value) {
                case DECOR_EMPTY:
                    printf(". ");
                    break;
                case DECOR_FLOOR:
                    printf("S ");
                    break;
                case DECOR_ROCK:
                    printf("R ");
                    break;
                default:
                    printf("%d ", cell_value);
                    break;
            }
        }
        printf("\n");
    }
    printf("\n");
}
