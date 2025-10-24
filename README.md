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
gcc -o game main.c player.c -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lm

# Exécuter
./game.exe
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

## Structure du projet

```
Projet C/
├── main.c                    # Code principal et boucle de jeu
├── player.h                  # Déclarations (Player, Projectile)
├── player.c                  # Implémentation du joueur et projectiles
├── game.exe                  # Exécutable compilé
├── assets/                   # Ressources graphiques
│   └── images/
│       ├── personnages/
│       │   ├── personnage_haut.png
│       │   ├── personnage_bas.png
│       │   ├── personnage_gauche.png
│       │   └── personnage_droite.png
│       └── projectiles/
│           └── proj.png
└── README.md
```
