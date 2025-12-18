#define _CRT_SECURE_NO_WARNINGS
#include "item.h"
#include "../utils/assets.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// creer un gestionnaire d'items
ItemManager* item_manager_create(int initial_capacity) {
    ItemManager* manager;
    
    manager = (ItemManager*)malloc(sizeof(ItemManager));
    if (manager == NULL) {
        printf("Erreur: allocation memoire pour ItemManager\n");
        return NULL;
    }
    
    manager->items = (Item*)malloc(sizeof(Item) * initial_capacity);
    if (manager->items == NULL) {
        printf("Erreur: allocation memoire pour les items\n");
        free(manager);
        return NULL;
    }
    
    manager->count = 0;
    manager->capacity = initial_capacity;
    
    return manager;
}

// detruire un gestionnaire d'items
void item_manager_destroy(ItemManager* manager) {
    int i;
    
    if (manager == NULL) {
        return;
    }
    
    for (i = 0; i < manager->count; i++) {
        item_cleanup(&manager->items[i]);
    }
    
    free(manager->items);
    free(manager);
}

// charger un item depuis un fichier CSV
Item* item_load_from_csv(const char* item_folder, SDL_Renderer* renderer) {
    Item* item = (Item*)malloc(sizeof(Item));
    if (!item) {
        printf("Erreur: allocation memoire pour Item\n");
        return NULL;
    }
    
    memset(item, 0, sizeof(Item));
    item->w = 32;
    item->h = 32;
    item->is_collected = false;
    
    char csv_path[512];
    snprintf(csv_path, sizeof(csv_path), "%s/stats.csv", item_folder);
    
    FILE* file = fopen(csv_path, "r");
    if (!file) {
        printf("Erreur: impossible d'ouvrir %s\n", csv_path);
        free(item);
        return NULL;
    }
    
    char line[512];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '\n' || line[0] == '#') {
            continue;
        }
        
        char key[128];
        char value[384];
        
        int found_equals = 0;
        int key_pos = 0;
        int value_pos = 0;
        int i;
        
        for (i = 0; line[i] != '\0' && line[i] != '\n'; i++) {
            if (line[i] == '=') {
                found_equals = 1;
                key[key_pos] = '\0';
                i++;
                break;
            }
            key[key_pos] = line[i];
            key_pos++;
        }
        
        if (!found_equals) {
            continue;
        }
        
        for (; line[i] != '\0' && line[i] != '\n'; i++) {
            value[value_pos] = line[i];
            value_pos++;
        }
        value[value_pos] = '\0';
        
        while (key[0] == ' ') {
            for (i = 0; key[i] != '\0'; i++) {
                key[i] = key[i + 1];
            }
        }
        i = strlen(key) - 1;
        while (i >= 0 && (key[i] == ' ' || key[i] == '\r' || key[i] == '\n')) {
            key[i] = '\0';
            i--;
        }
        
        while (value[0] == ' ') {
            for (i = 0; value[i] != '\0'; i++) {
                value[i] = value[i + 1];
            }
        }
        i = strlen(value) - 1;
        while (i >= 0 && (value[i] == ' ' || value[i] == '\r' || value[i] == '\n')) {
            value[i] = '\0';
            i--;
        }
        
        if (strcmp(key, "name") == 0) {
            strncpy(item->name, value, sizeof(item->name) - 1);
            item->name[sizeof(item->name) - 1] = '\0';
        }
        else if (strcmp(key, "description") == 0) {
            strncpy(item->description, value, sizeof(item->description) - 1);
            item->description[sizeof(item->description) - 1] = '\0';
        }
        else if (strcmp(key, "sprite") == 0) {
            snprintf(item->sprite_path, sizeof(item->sprite_path), "%s/%s", item_folder, value);
        }
        else {
            ItemStatType stat_type = item_string_to_stat_type(key);
            if (stat_type < STAT_COUNT) {
                item->stat_modifiers[stat_type] = atof(value);
            }
        }
    }
    
    fclose(file);
    
    if (strlen(item->sprite_path) > 0) {
        item->texture = load_texture(renderer, item->sprite_path);
        if (!item->texture) {
            printf("Avertissement: impossible de charger la texture %s\n", item->sprite_path);
        }
    }
    
    printf("Item charge: %s (%s)\n", item->name, item->description);
    
    return item;
}

// ajouter un item au gestionnaire
bool item_manager_add(ItemManager* manager, Item* item) {
    if (manager == NULL || item == NULL) {
        return false;
    }
    
    if (manager->count >= manager->capacity) {
        int new_capacity = manager->capacity * 2;
        Item* new_items = (Item*)realloc(manager->items, sizeof(Item) * new_capacity);
        if (new_items == NULL) {
            printf("Erreur: impossible d'agrandir le tableau d'items\n");
            return false;
        }
        manager->items = new_items;
        manager->capacity = new_capacity;
    }
    
    manager->items[manager->count] = *item;
    manager->count++;
    
    return true;
}

// appliquer les effets d'un item au joueur
void item_apply_to_player(const Item* item, Player* player) {
    if (item == NULL || player == NULL) {
        return;
    }
    
    printf("Application de l'item '%s' au joueur\n", item->name);
    
    player->max_health = player->max_health + (int)item->stat_modifiers[STAT_MAX_HEALTH];
    player->current_health = player->current_health + (int)item->stat_modifiers[STAT_MAX_HEALTH];
    
    player->speed = player->speed + item->stat_modifiers[STAT_SPEED];
    player->projectile_damage = player->projectile_damage + item->stat_modifiers[STAT_DAMAGE];
    player->fire_rate = player->fire_rate + item->stat_modifiers[STAT_FIRE_RATE];
    player->projectile_speed = player->projectile_speed + item->stat_modifiers[STAT_PROJECTILE_SPEED];
    
    if (player->fire_rate > 0) {
        player->fire_interval = 1.0f / player->fire_rate;
    }
    
    if (player->current_health > player->max_health) {
        player->current_health = player->max_health;
    }
    if (player->speed < 0) {
        player->speed = 0;
    }
    if (player->projectile_damage < 1) {
        player->projectile_damage = 1;
    }
    if (player->fire_rate < 0.5f) {
        player->fire_rate = 0.5f;
    }
    if (player->projectile_speed < 50) {
        player->projectile_speed = 50;
    }
    
    printf("Nouvelles stats - PV: %d/%d, Speed: %.1f, Damage: %.1f, FireRate: %.2f\n",
           player->current_health, player->max_health, player->speed, 
           player->projectile_damage, player->fire_rate);
}

// afficher un item a l'ecran
void item_render(SDL_Renderer* renderer, const Item* item) {
    if (item == NULL) {
        return;
    }
    
    if (item->texture == NULL) {
        return;
    }
    
    if (item->is_collected) {
        return;
    }
    
    SDL_Rect dest;
    dest.x = (int)item->x;
    dest.y = (int)item->y;
    dest.w = (int)item->w;
    dest.h = (int)item->h;
    
    SDL_RenderCopy(renderer, item->texture, NULL, &dest);
}

// verifier la collision entre un item et le joueur
bool item_check_collision(const Item* item, const Player* player) {
    if (item == NULL || player == NULL) {
        return false;
    }
    
    if (item->is_collected) {
        return false;
    }
    
    if (player->x < item->x + item->w &&
        player->x + player->w > item->x &&
        player->y < item->y + item->h &&
        player->y + player->h > item->y) {
        return true;
    }
    
    return false;
}

// nettoyer les ressources d'un item
void item_cleanup(Item* item) {
    if (item == NULL) {
        return;
    }
    
    if (item->texture != NULL) {
        SDL_DestroyTexture(item->texture);
        item->texture = NULL;
    }
}

// convertir un type de stat en chaine de caracteres
const char* item_stat_type_to_string(ItemStatType type) {
    switch (type) {
        case STAT_MAX_HEALTH: return "max_health";
        case STAT_SPEED: return "speed";
        case STAT_DAMAGE: return "damage";
        case STAT_FIRE_RATE: return "fire_rate";
        case STAT_PROJECTILE_SPEED: return "projectile_speed";
        default: return "unknown";
    }
}

// convertir une chaine en type de stat
ItemStatType item_string_to_stat_type(const char* str) {
    if (strcmp(str, "max_health") == 0) {
        return STAT_MAX_HEALTH;
    }
    if (strcmp(str, "speed") == 0) {
        return STAT_SPEED;
    }
    if (strcmp(str, "damage") == 0) {
        return STAT_DAMAGE;
    }
    if (strcmp(str, "fire_rate") == 0) {
        return STAT_FIRE_RATE;
    }
    if (strcmp(str, "projectile_speed") == 0) {
        return STAT_PROJECTILE_SPEED;
    }
    return STAT_COUNT;
}

// charger tous les items depuis un repertoire
int item_manager_load_all_from_directory(ItemManager* manager, const char* directory, SDL_Renderer* renderer) {
    int nombre_items_charges = 0;
    int i;
    char chemin_item[512];
    char chemin_csv[512];
    FILE* fichier_test;
    Item* item;
    
    if (manager == NULL || directory == NULL || renderer == NULL) {
        return 0;
    }

    const char* item_names[] = {"coeur", "piment", "master_sword", NULL};
    
    i = 0;
    while (item_names[i] != NULL) {
        snprintf(chemin_item, sizeof(chemin_item), "%s/%s", directory, item_names[i]);
        snprintf(chemin_csv, sizeof(chemin_csv), "%s/stats.csv", chemin_item);
        
        fichier_test = fopen(chemin_csv, "r");
        if (fichier_test != NULL) {
            fclose(fichier_test);
            
            item = item_load_from_csv(chemin_item, renderer);
            if (item != NULL) {
                if (item_manager_add(manager, item)) {
                    nombre_items_charges++;
                    printf("Item charge: %s\n", item->name);
                }
                free(item);
            }
        } else {
            printf("Avertissement: dossier %s introuvable ou pas de stats.csv\n", item_names[i]);
        }
        
        i++;
    }

    return nombre_items_charges;
}