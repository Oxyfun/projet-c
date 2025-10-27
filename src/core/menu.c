#include "menu.h"
#include "../utils/assets.h"
#include "../utils/constants.h"
#include <stdio.h>

// Variable globale pour l'état du menu
MenuState g_menu_state = MENU_STATE_MAIN_MENU;

// Callbacks pour les boutons
static void on_play_clicked() {
    g_menu_state = MENU_STATE_GAME;
}

static void on_level_editor_clicked() {
    g_menu_state = MENU_STATE_LEVEL_EDITOR;
}

static void on_quit_clicked() {
    g_menu_state = MENU_STATE_QUIT;
}

// Initialisation d'un bouton
void button_init(Button* btn, int x, int y, int w, int h, SDL_Renderer* renderer, const char* path_normal, const char* path_hover, const char* path_pressed, void (*onClick)(void)) {
    btn->x = x;
    btn->y = y;
    btn->w = w;
    btn->h = h;
    btn->state = BUTTON_STATE_NORMAL;
    btn->onClick = onClick;
    
    // Charger les textures
    btn->texture_normal = load_texture(renderer, path_normal);
    btn->texture_hover = load_texture(renderer, path_hover);
    btn->texture_pressed = load_texture(renderer, path_pressed);
}

// Vérifier si la souris est sur le bouton
bool button_is_hovered(Button* btn, int mouse_x, int mouse_y) {
    return (mouse_x >= btn->x && mouse_x <= btn->x + btn->w && mouse_y >= btn->y && mouse_y <= btn->y + btn->h);
}

// Mise à jour d'un bouton
void button_update(Button* btn, int mouse_x, int mouse_y, bool mouse_clicked) {
    bool hovered = button_is_hovered(btn, mouse_x, mouse_y);
    
    if (hovered && mouse_clicked) {
        btn->state = BUTTON_STATE_PRESSED;
        if (btn->onClick) {
            btn->onClick();
        }
    } else if (hovered) {
        btn->state = BUTTON_STATE_HOVER;
    } else {
        btn->state = BUTTON_STATE_NORMAL;
    }
}

// Rendu d'un bouton
void button_render(SDL_Renderer* renderer, Button* btn) {
    SDL_Rect rect = { btn->x, btn->y, btn->w, btn->h };
    
    // Choisir la texture en fonction de l'état
    SDL_Texture* current_texture = NULL;
    switch (btn->state) {
        case BUTTON_STATE_NORMAL:
            current_texture = btn->texture_normal;
            break;
        case BUTTON_STATE_HOVER:
            current_texture = btn->texture_hover;
            break;
        case BUTTON_STATE_PRESSED:
            current_texture = btn->texture_pressed;
            break;
    }
    
    // Afficher la texture ou un rectangle de fallback
    if (current_texture) {
        SDL_RenderCopy(renderer, current_texture, NULL, &rect);
    } else {
        SDL_Color color;
        switch (btn->state) {
            case BUTTON_STATE_NORMAL:
                color = (SDL_Color){100, 100, 100, 255};
                break;
            case BUTTON_STATE_HOVER:
                color = (SDL_Color){150, 150, 150, 255};
                break;
            case BUTTON_STATE_PRESSED:
                color = (SDL_Color){80, 80, 80, 255};
                break;
        }
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &rect);
        
        // Bordure
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &rect);
    }
}

// Nettoyage d'un bouton
void button_cleanup(Button* btn) {
    if (btn->texture_normal) {
        SDL_DestroyTexture(btn->texture_normal);
        btn->texture_normal = NULL;
    }
    // Ne pas détruire hover/pressed s'ils pointent vers normal
    if (btn->texture_hover && btn->texture_hover != btn->texture_normal) {
        SDL_DestroyTexture(btn->texture_hover);
    }
    if (btn->texture_pressed && btn->texture_pressed != btn->texture_normal) {
        SDL_DestroyTexture(btn->texture_pressed);
    }
    btn->texture_hover = NULL;
    btn->texture_pressed = NULL;
}

// Initialisation du menu
void menu_init(Menu* menu, SDL_Renderer* renderer) {
    menu->mouse_x = 0;
    menu->mouse_y = 0;
    menu->mouse_clicked = false;
    
    // le fond du menu
    menu->background = load_texture(renderer, "assets/images/menu/background.png");

    int btn_width = 200;
    int btn_height = 60;
    int center_x = (WINDOW_WIDTH - btn_width) / 2;

    button_init(&menu->btn_play, center_x, 250, btn_width, btn_height, renderer,
                "assets/images/menu/btn_play_normal.png",
                "assets/images/menu/btn_play_hover.png",
                "assets/images/menu/btn_play_pressed.png",
                on_play_clicked);

    button_init(&menu->btn_level_editor, center_x, 330, btn_width, btn_height, renderer,
                "assets/images/menu/btn_editor_normal.png",
                "assets/images/menu/btn_editor_hover.png",
                "assets/images/menu/btn_editor_pressed.png",
                on_level_editor_clicked);

    button_init(&menu->btn_quit, 650, 520, 120, 40, renderer,
                "assets/images/menu/btn_quit_normal.png",
                "assets/images/menu/btn_quit_hover.png",
                "assets/images/menu/btn_quit_pressed.png",
                on_quit_clicked);
}

// Mise à jour du menu
void menu_update(Menu* menu, SDL_Event* event) {
    if (g_menu_state != MENU_STATE_MAIN_MENU) {
        return;
    }

    if (event->type == SDL_MOUSEMOTION) {
        menu->mouse_x = event->motion.x;
        menu->mouse_y = event->motion.y;
        menu->mouse_clicked = false;
    } else if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT) { //clique gauche
        menu->mouse_x = event->button.x;
        menu->mouse_y = event->button.y;
        menu->mouse_clicked = true;
    } else if (event->type == SDL_MOUSEBUTTONUP) {
        menu->mouse_clicked = false;
    }

    button_update(&menu->btn_play, menu->mouse_x, menu->mouse_y, menu->mouse_clicked);
    button_update(&menu->btn_level_editor, menu->mouse_x, menu->mouse_y, menu->mouse_clicked);
    button_update(&menu->btn_quit, menu->mouse_x, menu->mouse_y, menu->mouse_clicked);
}

void menu_render(SDL_Renderer* renderer, Menu* menu) {
    // Afficher le fond
    if (menu->background) {
        SDL_Rect bg_rect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
        SDL_RenderCopy(renderer, menu->background, NULL, &bg_rect);
    } else {
        SDL_SetRenderDrawColor(renderer, 20, 20, 30, 255);
        SDL_RenderClear(renderer);
    }
    
    // Afficher les boutons
    button_render(renderer, &menu->btn_play);
    button_render(renderer, &menu->btn_level_editor);
    button_render(renderer, &menu->btn_quit);

}

// Nettoyage du menu
void menu_cleanup(Menu* menu) {
    button_cleanup(&menu->btn_play);
    button_cleanup(&menu->btn_level_editor);
    button_cleanup(&menu->btn_quit);

    if (menu->background) {
        SDL_DestroyTexture(menu->background);
        menu->background = NULL;
    }
}

