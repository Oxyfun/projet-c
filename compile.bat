@echo off
echo Compilation du projet Binding of Isaac...

REM Compilation avec GCC
gcc -o game.exe ^
    src/core/main.c ^
    src/player/player.c ^
    src/player/projectile.c ^
    src/utils/assets.c ^
    -Isrc ^
    -lmingw32 -lSDL2main -lSDL2 -lSDL2_image

if %ERRORLEVEL% EQU 0 (
    echo Compilation reussie ! Executable: game.exe
    echo Pour lancer le jeu: game.exe
) else (
    echo Erreur lors de la compilation !
    exit /b 1
)

