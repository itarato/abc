#include "engine.h"

#include <string>

#include "util.h"

void Engine::draw_stage() { SDL_RenderPresent(renderer); }

Engine::Engine(const char *title, const char *font_path, SDL_Point win_size)
    : win(nullptr),
      renderer(nullptr),
      font(nullptr),
      title(title),
      should_finish(false),
      font_path(font_path),
      win_size(win_size) {}

Engine::~Engine() {}

void Engine::init() {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) PANIC("SDL Init Error");
  if (TTF_Init() == -1) PANIC("TTF_Init failed");

  int img_flags = IMG_INIT_JPG | IMG_INIT_PNG;
  if ((IMG_Init(img_flags) & img_flags) != img_flags) PANIC("IMG_Init failed");

  win = SDL_CreateWindow("Lennox University", SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED, win_size.x, win_size.y,
                         SDL_WIN_FLAGS);

  if (win == nullptr) PANIC("Window creation error");

  renderer = SDL_CreateRenderer(win, -1, SDL_RENDER_FLAGS);

  if (renderer == nullptr) PANIC("Renderer creation error");

  font = TTF_OpenFont(font_path, 256);
  if (font == nullptr) PANIC("Cannot open font");
}

void Engine::run() {
  while (!should_finish) {
    handle_input();
    prepare_stage();
    draw_stage();
  }
}

void Engine::cleanup() {
  TTF_CloseFont(font);
  TTF_Quit();

  IMG_Quit();

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(win);
  SDL_Quit();
}

void Engine::draw_text(SDL_Point pos, int height, int align,
                       SDL_Color text_color, const char *msg) {
  SDL_Surface *text_surface = TTF_RenderText_Solid(font, msg, text_color);
  if (text_surface == nullptr) PANIC("Cannot render text");

  int optimal_w;
  int optimal_h;
  if (TTF_SizeText(font, msg, &optimal_w, &optimal_h) == -1)
    PANIC("Cannot check text size");

  SDL_Texture *text = SDL_CreateTextureFromSurface(renderer, text_surface);
  // Prepare with align-left, adjust later.
  SDL_Rect final_rect{pos.x, pos.y, (int)((height * optimal_w) / optimal_h),
                      height};

  if (align == TEXT_ALIGN_CENTER) final_rect.x -= (final_rect.w >> 1);

  SDL_RenderCopy(renderer, text, NULL, &final_rect);
  SDL_FreeSurface(text_surface);
  SDL_DestroyTexture(text);
}

void Engine::draw_image(SDL_Point pos, Image *image) {
  SDL_Rect rect{pos.x, pos.y, image->w, image->h};
  SDL_RenderCopy(renderer, image->text, NULL, &rect);
}

void Engine::exit_next_frame() { should_finish = true; }
