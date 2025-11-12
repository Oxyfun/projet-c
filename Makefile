# Makefile pour le projet Binding of Isaac

# Compilateur et flags
CC = gcc
CFLAGS = -Wall -Wextra -Isrc
LDFLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image

# Nom de l'exécutable
TARGET = The_Binding_of_Bilo.exe

# Fichiers source
SOURCES = src/core/main.c \
          src/core/menu.c \
          src/levels/level_editor.c \
          src/levels/room.c \
          src/player/player.c \
          src/player/projectile.c \
          src/utils/assets.c

# Fichiers objets (générés à partir des sources)
OBJECTS = $(SOURCES:.c=.o)

# Règle par défaut : compiler le projet
all: $(TARGET)

# Règle pour créer l'exécutable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "Compilation reussie ! Executable: $(TARGET)"

# Règle pour compiler les fichiers .c en .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyer les fichiers objets et l'exécutable
clean:
	del /Q src\core\*.o src\player\*.o src\utils\*.o $(TARGET) 2>nul || echo Dossier deja propre.

# Recompiler complètement
rebuild: clean all

# Compiler et lancer le jeu
run: $(TARGET)
	.\$(TARGET)

.PHONY: all clean rebuild run

