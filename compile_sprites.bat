@echo off
echo Compilation du generateur de sprites...
gcc -o create_sprites create_sprites.c -lmingw32 -lSDL2main -lSDL2 -lSDL2_image
if %ERRORLEVEL% EQU 0 (
    echo Compilation reussie !
    echo.
    echo Creation des sprites...
    create_sprites.exe
    echo.
    echo Sprites prets ! Vous pouvez lancer le jeu.
) else (
    echo Erreur lors de la compilation !
)

