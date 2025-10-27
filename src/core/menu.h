#ifndef MENU_H
#define MENU_H

#include "../utils/sdl_common.h"
#include <stdbool.h>

// États du menu/jeu
typedef enum {
    MENU_STATE_MAIN_MENU,
    MENU_STATE_GAME,
    MENU_STATE_LEVEL_EDITOR,
    MENU_STATE_QUIT
} MenuState;

// États d'un bouton
typedef enum {
    BUTTON_STATE_NORMAL,
    BUTTON_STATE_HOVER,
    BUTTON_STATE_PRESSED
} ButtonState;

// Structure pour un bouton
typedef struct {
    int x, y; // Position
    int w, h; // Dimensions
    ButtonState state; // État actuel
    
    // Textures pour les différents états
    SDL_Texture* texture_normal;
    SDL_Texture* texture_hover;
    SDL_Texture* texture_pressed;
    
    // Fonction callback lors du clic
    void (*onClick)(void);
} Button;

// Structure du menu
typedef struct {
    // Boutons du menu principal
    Button btn_play;
    Button btn_level_editor;
    Button btn_quit;
    
    // Texture de fond du menu
    SDL_Texture* background;
    
    // Position de la souris
    int mouse_x;
    int mouse_y;
    bool mouse_clicked;
} Menu;

// Fonctions du menu
void menu_init(Menu* menu, SDL_Renderer* renderer);
void menu_update(Menu* menu, SDL_Event* event);
void menu_render(SDL_Renderer* renderer, Menu* menu);
void menu_cleanup(Menu* menu);

// Fonctions utilitaires pour les boutons
void button_init(Button* btn, int x, int y, int w, int h, SDL_Renderer* renderer, const char* path_normal, const char* path_hover, const char* path_pressed, void (*onClick)(void));
void button_update(Button* btn, int mouse_x, int mouse_y, bool mouse_clicked);
void button_render(SDL_Renderer* renderer, Button* btn);
void button_cleanup(Button* btn);
bool button_is_hovered(Button* btn, int mouse_x, int mouse_y);

// Variable globale pour l'état du menu
extern MenuState g_menu_state;

#endif

