ifeq ($(OS),Windows_NT)
    detected_OS := Windows
else
    detected_OS := $(shell uname -s)
endif

CC = gcc
CFLAGS = -Wall -Wextra -Isrc

ifeq ($(detected_OS),Windows)
    LDFLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf
    TARGET = The_Binding_of_Bilo.exe
    CLEAN_CMD = rm -f $(TARGET)
    RUN_CMD = ./$(TARGET)
else
    CFLAGS += $(shell sdl2-config --cflags)
    LDFLAGS = $(shell sdl2-config --libs) -lSDL2_image -lSDL2_ttf -lm
    TARGET = The_Binding_of_Bilo
    CLEAN_CMD = rm -f $(TARGET)
    RUN_CMD = ./$(TARGET)
endif

SOURCES = src/core/main.c \
          src/core/menu.c \
          src/items/item.c \
          src/levels/dungeon.c \
          src/levels/level_editor.c \
          src/levels/room.c \
          src/monsters/monster.c \
          src/monsters/monster_projectile.c \
          src/player/player.c \
          src/player/projectile.c \
          src/utils/assets.c

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET) $(LDFLAGS)
	@echo "Compilation reussie ! Executable: $(TARGET)"

clean:
	$(CLEAN_CMD)

