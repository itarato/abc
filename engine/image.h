#ifndef ENGINE_IMAGE_H
#define ENGINE_IMAGE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class Image {
 protected:
  virtual SDL_Surface *load_surface(SDL_RWops *rwops) = 0;
  Image();

 public:
  int w;
  int h;
  SDL_Texture *text;
  void init(SDL_Renderer *renderer, const char *file_name);
  ~Image();
};

class JPGImage final : public Image {
 protected:
  SDL_Surface *load_surface(SDL_RWops *rwops);
};

class PNGImage final : public Image {
 protected:
  SDL_Surface *load_surface(SDL_RWops *rwops);
};

#endif