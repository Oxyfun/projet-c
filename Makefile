# Makefile pour le projet Binding of Isaac

# Détection de l'OS
ifeq ($(OS),Windows_NT)
    detected_OS := Windows
else
    detected_OS := $(shell uname -s)
endif

CC = gcc
CFLAGS = -Wall -Wextra -Isrc

# Configuration selon l'OS
ifeq ($(detected_OS),Windows)
    # Configuration Windows (MinGW)
    LDFLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf
    TARGET = The_Binding_of_Bilo.exe
    CLEAN_OBJS_CMD = del /Q src\core\*.o src\levels\*.o src\monsters\*.o src\player\*.o src\utils\*.o 2>nul
    CLEAN_CMD = $(CLEAN_OBJS_CMD) && del /Q $(TARGET) 2>nul || echo Dossier deja propre.
    RUN_CMD = .\\$(TARGET)
else
    # Configuration Linux / WSL
    # Utilisation de sdl2-config pour obtenir les bons chemins
    CFLAGS += $(shell sdl2-config --cflags)
    LDFLAGS = $(shell sdl2-config --libs) -lSDL2_image -lSDL2_ttf -lm
    TARGET = The_Binding_of_Bilo
    CLEAN_OBJS_CMD = rm -f src/core/*.o src/levels/*.o src/monsters/*.o src/player/*.o src/utils/*.o
    CLEAN_CMD = $(CLEAN_OBJS_CMD) && rm -f $(TARGET)
    RUN_CMD = ./$(TARGET)
endif

# Fichiers source
SOURCES = src/core/main.c \
          src/core/menu.c \
          src/levels/level_editor.c \
          src/levels/room.c \
          src/monsters/monster.c \
          src/player/player.c \
          src/player/projectile.c \
          src/utils/assets.c

# Fichiers objets
OBJECTS = $(SOURCES:.c=.o)

# Règle par défaut
all: $(TARGET)

# Création de l'exécutable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "Compilation reussie ! Executable: $(TARGET)"
	@echo "Nettoyage des fichiers objets..."
	$(CLEAN_OBJS_CMD)

# Compilation des fichiers objets
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage complet (objets + exe)
clean:
	$(CLEAN_CMD)

# Recompiler
rebuild: clean all

# Lancer
run: $(TARGET)
	$(RUN_CMD)

.PHONY: all clean rebuild run
