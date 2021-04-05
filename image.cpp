#include "image.h"

#include "util.h"

Image::Image() : text(nullptr) {}

void Image::init(SDL_Renderer *renderer, const char *file_name) {
  SDL_RWops *rwops = SDL_RWFromFile(file_name, "rb");
  SDL_Surface *surf = load_surface(rwops);
  if (surf == nullptr) PANIC("Cannot load image");

  w = surf->w;
  h = surf->h;

  text = SDL_CreateTextureFromSurface(renderer, surf);
  SDL_FreeSurface(surf);
}

Image::~Image() {
  if (text) SDL_DestroyTexture(text);
}

SDL_Surface *JPGImage::load_surface(SDL_RWops *rwops) {
  return IMG_LoadJPG_RW(rwops);
}

SDL_Surface *PNGImage::load_surface(SDL_RWops *rwops) {
  return IMG_LoadPNG_RW(rwops);
}
