# The Binding of Bilo

Un jeu 2D en C inspiré de The Binding of Isaac, développé avec SDL2.

## Équipe

- Nadir NIODIL
- Idriss GALLET
- Nolan LEMAITRE

## Prérequis

### Windows (MSYS2)

```bash
pacman -S mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image mingw-w64-x86_64-SDL2_ttf
```

## Compilation et exécution

### Méthode recommandée (Script batch)

```bash
.\compile.bat
```

### Avec Make

```bash
make
```

### Compilation manuelle

```bash
gcc -o game.exe src/core/main.c src/core/menu.c src/levels/level_editor.c src/levels/room.c src/monsters/monster.c src/player/player.c src/player/projectile.c src/utils/assets.c -Isrc -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf
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
- ✅ Monstres (IA de suivi simple)
- ✅ Gestion des salles
- ✅ Éditeur de niveau
- ✅ Système de collision
- ⏳ Objets et items
- ⏳ Transitions entre salles
- ⏳ Mini-boss

## Structure du projet

```
projet-c/
├── src/
│   ├── core/                       # Fichiers principaux
│   │   ├── main.c                  # Boucle de jeu
│   │   ├── menu.h/.c               # Gestion du menu
│   ├── levels/                     # Gestion des niveaux
│   │   ├── level_editor.h/.c       # Éditeur de niveau
│   │   ├── room.h/.c               # Gestion des salles
│   ├── monsters/                   # Gestion des ennemis
│   │   ├── monster.h/.c            # IA et stats des monstres
│   ├── player/
│   │   ├── player.h/.c             # Joueur
│   │   ├── projectile.h/.c         # Projectiles
│   └── utils/                      # Utilitaires
│       ├── sdl_common.h            # Inclusions SDL multi-plateforme
│       ├── assets.h/.c             # Chargement des assets
│       └── constants.h             # Constantes du jeu
├── assets/
│   ├── images/
│   │   ├── menu/                   # Images du menu
│   │   ├── personnages/            # Sprites du joueur
│   │   └── projectiles/            # Sprites des projectiles
│   └── fonts/                      # Polices d'écriture (TTF)
├── Makefile                        # Makefile multi-plateforme
├── The_Binding_of_Bilo(.exe)       # Exécutable du jeu
└── README.md                       # Documentation du projet
```

## SDL2

Le projet gère automatiquement les différences d'includes SDL entre les systèmes :

- **Windows + MinGW** : `#include <SDL2/SDL.h>` (automatique)
- **Linux/Mac** : `#include <SDL.h>` (automatique)
