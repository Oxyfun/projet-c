# The Binding of Bilo

Un jeu 2D en C inspiré de The Binding of Isaac, développé avec SDL2.

## Équipe

- Nadir NIODIL
- Idriss GALLET
- Nolan LEMAITRE

## Prérequis

### Windows (MSYS2)

```bash
pacman -S --needed mingw-w64-x86_64-toolchain mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image mingw-w64-x86_64-SDL2_ttf
```

## Compilation et exécution

### Avec Make

```bash
make
```

Puis lancer :

```bash
./The_Binding_of_Bilo.exe
```

### Compilation manuelle

```bash
gcc -Wall -Wextra -Isrc \
  src/core/main.c src/core/menu.c \
  src/items/item.c \
  src/levels/dungeon.c src/levels/level_editor.c src/levels/room.c \
  src/monsters/monster.c src/monsters/monster_projectile.c \
  src/player/player.c src/player/projectile.c \
  src/utils/assets.c \
  -o The_Binding_of_Bilo.exe \
  -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf
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

### Éditeur de niveau

- **Clic Gauche** : Poser une tuile
- **Clic Droit** : Effacer une tuile
- **1** : Sélectionner Sol
- **2** : Sélectionner Rocher (Obstacle)
- **3** : Sélectionner Porte (doit être sur les bords)
- **4** : Sélectionner Coffre (Obstacle)
- **I** : Placer un monstre **Basique**
- **O** : Placer un monstre **Tank**
- **P** : Placer un monstre **Shooter**
- **B** : Placer un **Mini-boss**
- **6** : Placer l'item **Cœur**
- **7** : Placer l'item **Piment**
- **0** : Vide
- **S** : Sauvegarder la salle (crée un fichier CSV)
- **L** : Charger une salle aléatoire
- **C** : Vider la salle
- **ESC** : Retour au menu

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
- ✅ Monstres (IA de suivi, collision, dégâts)
- ✅ Gestion des salles
- ✅ Éditeur de niveau
- ✅ Système de collision
- ✅ Génération procédurale de donjon (Donjon aléatoire)
- ✅ Transitions entre salles
- ✅ Minimap
- ✅ Objets et items (coffres, loot, application de stats)
- ✅ Mini-boss

## Structure du projet

```
projet-c/
├── items/                          # Items (données + sprites)
├── rooms/                          # Salles au format CSV
├── src/
│   ├── core/                       # Fichiers principaux
│   │   ├── main.c                  # Boucle de jeu
│   │   ├── menu.h/.c               # Gestion du menu
│   ├── items/                      # Système d'items
│   │   ├── item.h/.c               # Chargement + application d'items
│   ├── levels/                     # Gestion des niveaux
│   │   ├── level_editor.h/.c       # Éditeur de niveau
│   │   ├── room.h/.c               # Gestion des salles
│   │   ├── dungeon.h/.c            # Génération procédurale du donjon
│   ├── monsters/                   # Gestion des ennemis
│   │   ├── monster.h/.c            # IA et stats des monstres
│   │   ├── monster_projectile.h/.c # Projectiles des monstres
│   ├── player/
│   │   ├── player.h/.c             # Joueur
│   │   ├── projectile.h/.c         # Projectiles
│   └── utils/                      # Utilitaires
│       ├── sdl_common.h            # Inclusions SDL multi-plateforme
│       ├── assets.h/.c             # Chargement des assets
│       └── constants.h             # Constantes du jeu
├── assets/
│   ├── images/
│   │   ├── decor/                  # Sprites du décor
│   │   ├── menu/                   # Images du menu
│   │   ├── monstre/                # Sprites du monstre
│   │   ├── personnages/            # Sprites du joueur
│   │   └── projectiles/            # Sprites des projectiles
│   └── fonts/                      # Polices d'écriture (TTF)
├── Makefile                        # Makefile multi-plateforme
└── README.md                       # Documentation du projet
```

## SDL2

Le projet gère automatiquement les différences d'includes SDL entre les systèmes :

- **Windows + MinGW** : `#include <SDL2/SDL.h>` (automatique)
- **Linux/Mac** : `#include <SDL.h>` (automatique)
