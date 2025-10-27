# Sprites du Menu - Guide de Création

## 📋 Liste des sprites nécessaires

Le système de menu nécessite **9 images PNG** :

### Bouton "JOUER" (200x60 pixels)

- `btn_play_normal.png` - État normal
- `btn_play_hover.png` - État survolé
- `btn_play_pressed.png` - État cliqué

### Bouton "ÉDITEUR DE NIVEAU" (200x60 pixels)

- `btn_editor_normal.png` - État normal
- `btn_editor_hover.png` - État survolé
- `btn_editor_pressed.png` - État cliqué

### Bouton "QUITTER" (120x40 pixels)

- `btn_quit_normal.png` - État normal
- `btn_quit_hover.png` - État survolé
- `btn_quit_pressed.png` - État cliqué

### Fond du menu (800x600 pixels - optionnel)

- `background.png` - Fond du menu principal

## 🎨 Création rapide avec des placeholders

Si vous n'avez pas encore de sprites, le système affichera des **rectangles colorés par défaut**.

### Option 1 : Placeholders simples (Paint/GIMP)

Vous pouvez créer des sprites simples avec Paint ou GIMP :

**Bouton JOUER (200x60)** :

- Normal : Rectangle gris clair #808080
- Hover : Rectangle gris plus clair #A0A0A0
- Pressed : Rectangle gris foncé #606060

**Bouton ÉDITEUR (200x60)** :

- Normal : Rectangle bleu foncé #4040A0
- Hover : Rectangle bleu moyen #6060C0
- Pressed : Rectangle bleu très foncé #303080

**Bouton QUITTER (120x40)** :

- Normal : Rectangle rouge foncé #A04040
- Hover : Rectangle rouge moyen #C06060
- Pressed : Rectangle rouge très foncé #803030

### Option 2 : Sprites avec texte

Utilisez un éditeur d'image pour ajouter du texte :

- Police : Arial Bold, taille 24 pour les gros boutons, 16 pour Quitter
- Couleur texte : Blanc (#FFFFFF)
- Centrez le texte dans le bouton

### Option 3 : Style Binding of Isaac

Pour un style plus proche de Binding of Isaac :

- Utilisez des bordures pixelisées épaisses (2-3 pixels)
- Couleurs sombres avec contraste élevé
- Effet d'ombre portée pour le texte
- Coins légèrement arrondis

## 📐 Positions dans le jeu

- **Bouton JOUER** : Centre horizontal (X=300), Y=250
- **Bouton ÉDITEUR** : Centre horizontal (X=300), Y=330
- **Bouton QUITTER** : Bas à droite (X=650, Y=520)

## 🛠️ Outils recommandés

### Gratuits

- **GIMP** : Éditeur d'image complet (comme Photoshop)
- **Paint.NET** : Simple et efficace pour Windows
- **Piskel** : Pour sprites pixel art en ligne
- **Aseprite** : Excellent pour pixel art (payant mais abordable)

### En ligne

- **Photopea** : Clone de Photoshop en ligne (photopea.com)
- **Canva** : Simple pour créer des boutons avec texte

## ⚙️ Format technique

- **Format** : PNG avec transparence
- **Profondeur** : 32 bits (RGBA)
- **Compression** : Sans perte
- **Transparence** : Optionnelle mais recommandée pour les coins arrondis

## 🚀 Démarrage rapide sans sprites

Le jeu fonctionne **sans sprites** ! Les boutons s'afficheront en rectangles colorés :

- Gris clair = Normal
- Gris plus clair = Hover
- Gris foncé = Pressé

Vous pouvez donc **tester le menu immédiatement** et ajouter les sprites plus tard !

## 📝 Notes

- Si un sprite manque, le système utilise automatiquement `_normal` pour les autres états
- Les chemins peuvent être modifiés dans `src/core/menu.c` (fonction `menu_init`)
- Le fond du menu est optionnel (fond noir par défaut)
