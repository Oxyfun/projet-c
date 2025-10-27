# The Binding of Bilo

Un jeu 2D en C inspiré de The Binding of Isaac, développé avec SDL2.

## Équipe

- Nadir NIODIL
- Idriss GALLET
- Nolan LEMAITRE

## Prérequis

### Windows (MSYS2)

```bash
pacman -S mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image
```

## Compilation et exécution

### Méthode recommandée (Script batch)

```bash
.\compile.bat
```

### Avec Make

```bash
make # Compiler
make run # Compiler et exécuter
make clean # Nettoyer
```

### Compilation manuelle

```bash
gcc -o game.exe src/core/main.c src/player/player.c src/player/projectile.c src/utils/assets.c -Isrc -lmingw32 -lSDL2main -lSDL2 -lSDL2_image
```

## Contrôles

### Mouvement

- **Z** : Monter
- **Q** : Aller à gauche
- **S** : Descendre
- **D** : Aller à droite

### Tir

- **Flèche HAUT** : Tirer vers le haut
- **Flèche GAUCHE** : Tirer vers la gauche
- **Flèche BAS** : Tirer vers le bas
- **Flèche DROITE** : Tirer vers la droite

### Autres

- **ESC** : Quitter le jeu
- **Fermer la fenêtre** : Quitter le jeu

## État du projet

- ✅ Fenêtre SDL2 de base (800x600)
- ✅ Boucle principale avec gestion des événements
- ✅ Rendu 2D avec SDL_Renderer
- ✅ Déplacement du joueur avec delta time
- ✅ Sprites selon la direction
- ✅ Structure Player modulaire
- ✅ Système de tir
- ✅ Menu au lancement
- ⏳ Gestion des collisions
- ⏳ Ennemis et objets
- ⏳ Salles et transitions
- ⏳ Mini-boss

## Structure du projet

```
projet-c/
├── src/
│   ├── core/                       # Fichiers principaux
│   │   ├── main.c                  # Boucle de jeu
│   │   ├── menu.h                  # Déclarations du menu
│   │   └── menu.c                  # Implémentation du menu
│   ├── player/
│   │   ├── player.h                # Déclarations du joueur
│   │   ├── player.c                # Implémentation du joueur
│   │   ├── projectile.h            # Déclarations des projectiles
│   │   └── projectile.c            # Implémentation des projectiles
│   └── utils/                      # Utilitaires
│       ├── sdl_common.h            # Gestion SDL
│       ├── assets.h                # Déclarations des assets
│       ├── assets.c                # Chargement des textures
│       └── constants.h             # Constantes du jeu
├── assets/
│   └── images/
│       ├── menu/                   # Images du menu
│       ├── personnages/            # Sprites du joueur
│       └── projectiles/            # Sprites des projectiles
│           └── proj.png
├── compile.bat                     # Script de compilation Windows
├── Makefile                        # Makefile
├── The_Binding_of_Bilo.exe         # Jeu
└── README.md                       # Documentation du projet

```

## SDL2

Le projet gère automatiquement les différences d'includes SDL entre les systèmes :

- **Windows + MinGW** : `#include <SDL2/SDL.h>` (automatique)
- **Linux/Mac** : `#include <SDL.h>` (automatique)
