#ifndef ENGINE_ENGINE_H
#define ENGINE_ENGINE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "image.h"
#include "sound.h"
#include "types.h"
#include "util.h"

#define TEXT_ALIGN_CENTER 0
#define TEXT_ALIGN_LEFT 1

class Engine {
 private:
  SDL_Rect rel_to_abs(RelRect rect);
  SDL_Point rel_to_abs(RelPoint pos);
  double win_ratio();

 protected:
  SDL_Window *win;
  SDL_Renderer *renderer;
  TTF_Font *font;
  const char *title;
  bool should_finish;
  const char *font_path;
  SDL_Point win_size;

  virtual void handle_input() = 0;
  virtual void prepare_stage() = 0;
  virtual void draw_stage();
  void exit_next_frame();

 public:
  Engine(const char *title, const char *font_path, SDL_Point win_size);
  virtual ~Engine();
  virtual void init();
  void run();

  void draw_text(SDL_Point pos, int height, int align, SDL_Color text_color,
                 const char *msg);
  void draw_text_rel(RelPoint pos, double height, int align,
                     SDL_Color text_color, const char *msg);

  void draw_image(SDL_Point pos, Image *image);
  void draw_image_rel(RelRect rect, Image *image);
};

#endif
