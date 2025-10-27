#ifndef ASSETS_H
#define ASSETS_H

#include "sdl_common.h"

// Fonction pour charger une texture depuis un fichier
SDL_Texture* load_texture(SDL_Renderer* renderer, const char* path);

#endif
