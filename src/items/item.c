#define _CRT_SECURE_NO_WARNINGS
#include "item.h"
#include "../utils/assets.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ItemManager* item_manager_create(int initial_capacity) {
    ItemManager* manager = (ItemManager*)malloc(sizeof(ItemManager));
    if (!manager) {
        printf("Erreur: allocation memoire pour ItemManager\n");
        return NULL;
    }
    
    manager->items = (Item*)malloc(sizeof(Item) * initial_capacity);
    if (!manager->items) {
        printf("Erreur: allocation memoire pour les items\n");
        free(manager);
        return NULL;
    }
    
    manager->count = 0;
    manager->capacity = initial_capacity;
    
    return manager;
}

void item_manager_destroy(ItemManager* manager) {
    if (!manager) return;
    
    for (int i = 0; i < manager->count; i++) {
        item_cleanup(&manager->items[i]);
    }
    
    free(manager->items);
    free(manager);
}

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
        if (line[0] == '\n' || line[0] == '#') continue;
        
        char key[128], value[384];
        if (sscanf(line, "%127[^=]=%383[^\n]", key, value) == 2) {
            char* k = key;
            char* v = value;
            while (*k == ' ') k++;
            while (*v == ' ') v++;
            
            char* end = k + strlen(k) - 1;
            while (end > k && (*end == ' ' || *end == '\r' || *end == '\n')) {
                *end = '\0';
                end--;
            }
            end = v + strlen(v) - 1;
            while (end > v && (*end == ' ' || *end == '\r' || *end == '\n')) {
                *end = '\0';
                end--;
            }
            
            if (strcmp(k, "name") == 0) {
                strncpy(item->name, v, sizeof(item->name) - 1);
            }
            else if (strcmp(k, "description") == 0) {
                strncpy(item->description, v, sizeof(item->description) - 1);
            }
            else if (strcmp(k, "sprite") == 0) {
                snprintf(item->sprite_path, sizeof(item->sprite_path), "%s/%s", item_folder, v);
            }
            else {
                ItemStatType stat_type = item_string_to_stat_type(k);
                if (stat_type < STAT_COUNT) {
                    item->stat_modifiers[stat_type] = atof(v);
                }
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

bool item_manager_add(ItemManager* manager, Item* item) {
    if (!manager || !item) return false;
    
    if (manager->count >= manager->capacity) {
        int new_capacity = manager->capacity * 2;
        Item* new_items = (Item*)realloc(manager->items, sizeof(Item) * new_capacity);
        if (!new_items) {
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

void item_apply_to_player(const Item* item, Player* player) {
    if (!item || !player) return;
    
    printf("Application de l'item '%s' au joueur\n", item->name);
    
    player->max_health += (int)item->stat_modifiers[STAT_MAX_HEALTH];
    player->current_health += (int)item->stat_modifiers[STAT_MAX_HEALTH];
    
    player->speed += item->stat_modifiers[STAT_SPEED];
    player->projectile_damage += item->stat_modifiers[STAT_DAMAGE];
    player->fire_rate += item->stat_modifiers[STAT_FIRE_RATE];
    player->projectile_speed += item->stat_modifiers[STAT_PROJECTILE_SPEED];
    
    if (player->fire_rate > 0) {
        player->fire_interval = 1.0f / player->fire_rate;
    }
    
    if (player->current_health > player->max_health) {
        player->current_health = player->max_health;
    }
    if (player->speed < 0) player->speed = 0;
    if (player->projectile_damage < 1) player->projectile_damage = 1;
    if (player->fire_rate < 0.5f) player->fire_rate = 0.5f;
    if (player->projectile_speed < 50) player->projectile_speed = 50;
    
    printf("Nouvelles stats - PV: %d/%d, Speed: %.1f, Damage: %.1f, FireRate: %.2f\n",
           player->current_health, player->max_health, player->speed, 
           player->projectile_damage, player->fire_rate);
}

void item_render(SDL_Renderer* renderer, const Item* item) {
    if (!item || !item->texture || item->is_collected) return;
    
    SDL_Rect dest = { (int)item->x, (int)item->y, (int)item->w, (int)item->h };
    SDL_RenderCopy(renderer, item->texture, NULL, &dest);
}

bool item_check_collision(const Item* item, const Player* player) {
    if (!item || !player || item->is_collected) return false;
    
    return (player->x < item->x + item->w &&
            player->x + player->w > item->x &&
            player->y < item->y + item->h &&
            player->y + player->h > item->y);
}

void item_cleanup(Item* item) {
    if (!item) return;
    
    if (item->texture) {
        SDL_DestroyTexture(item->texture);
        item->texture = NULL;
    }
}

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

ItemStatType item_string_to_stat_type(const char* str) {
    if (strcmp(str, "max_health") == 0) return STAT_MAX_HEALTH;
    if (strcmp(str, "speed") == 0) return STAT_SPEED;
    if (strcmp(str, "damage") == 0) return STAT_DAMAGE;
    if (strcmp(str, "fire_rate") == 0) return STAT_FIRE_RATE;
    if (strcmp(str, "projectile_speed") == 0) return STAT_PROJECTILE_SPEED;
    return STAT_COUNT;
}

// charge tous les items depuis le dossier items
// pour ajouter un item il faut juste le mettre dans le tableau
int item_manager_load_all_from_directory(ItemManager* manager, const char* directory, SDL_Renderer* renderer) {
    if (manager == NULL || directory == NULL || renderer == NULL) {
        return 0;
    }

    int nombre_items_charges = 0;

    // liste des items a charger
    const char* item_names[] = {"coeur", "piment", "master_sword", NULL};
    
    int i = 0;
    while (item_names[i] != NULL) {
        char chemin_item[512];
        snprintf(chemin_item, sizeof(chemin_item), "%s/%s", directory, item_names[i]);
        
        char chemin_csv[512];
        snprintf(chemin_csv, sizeof(chemin_csv), "%s/stats.csv", chemin_item);
        
        FILE* fichier_test = fopen(chemin_csv, "r");
        if (fichier_test != NULL) {
            fclose(fichier_test);
            
            Item* item = item_load_from_csv(chemin_item, renderer);
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