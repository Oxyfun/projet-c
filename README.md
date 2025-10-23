# Binding of Isaac Clone

Un jeu 2D en C inspiré de The Binding of Isaac, développé avec SDL2.

## Équipe

- Nadir NIODIL
- Idriss GALLET
- Nolan LEMAITRE

## Prérequis

### Windows (MSYS2)

```bash
pacman -S mingw-w64-x86_64-SDL2
```

## Compilation et exécution

### Windows (MSYS2)

```bash
# Compiler
gcc -o game main.c -lmingw32 -lSDL2main -lSDL2

# Ou utiliser le script batch
./compile.bat

# Exécuter
./game.exe
```

## Contrôles actuels

- **ESC** : Quitter le jeu

## État du projet

- ✅ Fenêtre SDL2 de base (800x600)
- ✅ Boucle principale avec gestion des événements
- ✅ Rendu 2D avec SDL_Renderer
- ⏳ Déplacement du joueur
- ⏳ Système de tir
- ⏳ Gestion des collisions
- ⏳ Ennemis et objets
- ⏳ Salles et transitions
- ⏳ Mini-boss

## Structure du projet

```
Projet C/
├── main.c              # Code principal du jeu
└── README.md           # Documentation
```
