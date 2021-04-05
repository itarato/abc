#ifndef ENGINE_UTIL_H
#define ENGINE_UTIL_H

#include <iostream>

#define SDL_COLOR_GREEN (SDL_Color{80, 240, 40})
#define SDL_COLOR_RED (SDL_Color{240, 40, 40})
#define SDL_COLOR_BLUE (SDL_Color{40, 80, 240})
#define SDL_COLOR_WHITE (SDL_Color{240, 240, 240})
#define SDL_COLOR_BLACK (SDL_Color{40, 40, 40})

#define PANIC(s)                 \
  {                              \
    std::cerr << s << std::endl; \
    exit(EXIT_FAILURE);          \
  }

#endif
