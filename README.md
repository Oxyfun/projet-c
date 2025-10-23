# Binding of Isaac Clone

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

### Windows (MSYS2)

```bash
# Compiler
gcc -o game main.c player.c -lmingw32 -lSDL2main -lSDL2 -lSDL2_image

# Exécuter
./game.exe
```

## Contrôles actuels

- **Z / Flèche Haut** : Monter
- **S / Flèche Bas** : Descendre
- **Q / Flèche Gauche** : Aller à gauche
- **D / Flèche Droite** : Aller à droite
- **ESC** : Quitter le jeu

## État du projet

- ✅ Fenêtre SDL2 de base (800x600)
- ✅ Boucle principale avec gestion des événements
- ✅ Rendu 2D avec SDL_Renderer
- ✅ Déplacement du joueur avec delta time
- ✅ Sprites selon la direction
- ✅ Structure Player modulaire
- ⏳ Système de tir
- ⏳ Gestion des collisions
- ⏳ Ennemis et objets
- ⏳ Salles et transitions
- ⏳ Mini-boss

## Structure du projet

```
projet-c/
├── assets/             # Ressources graphiques
│   ├── personnage_bas.png
│   ├── personnage_haut.png
│   ├── personnage_gauche.png
│   └── personnage_droite.png
├── main.c              # Code principal et boucle de jeu
├── player.h            # Déclarations de la structure Player
├── player.c            # Implémentation du joueur (init, update, render)
├── game.exe            # Exécutable compilé
└── README.md           # Documentation
```
