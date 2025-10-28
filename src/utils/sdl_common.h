#ifndef SDL_COMMON_H
#define SDL_COMMON_H

#if defined(_WIN32) && defined(__MINGW32__)
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_image.h>
#else
    #include <SDL.h>
    #include <SDL_image.h>
#endif

#endif
