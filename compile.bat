@echo off
echo Compilation du projet Binding of Bilo...

REM Compilation avec GCC
gcc -o The_Binding_of_Bilo ^
    src/core/main.c ^
    src/core/menu.c ^
    src/player/player.c ^
    src/player/projectile.c ^
    src/utils/assets.c ^
    -Isrc ^
    -lmingw32 -lSDL2main -lSDL2 -lSDL2_image

if %ERRORLEVEL% EQU 0 (
    echo Compilation reussie ! Executable: The_Binding_of_Bilo.exe
    echo Pour lancer le jeu: The_Binding_of_Bilo.exe
) else (
    echo Erreur lors de la compilation !
    exit /b 1
)

