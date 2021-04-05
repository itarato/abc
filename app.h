#ifndef APP_H
#define APP_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "image.h"
#include "sound.h"
#include "util.h"

class App {
 private:
  SDL_Window *win;
  SDL_Renderer *renderer;
  TTF_Font *font;
  char pressed_char;
  char answer_char;
  JPGImage *animal_image_textures[ABC_LEN];
  PNGImage *celebration_image;
  PNGImage *overlay_image;
  SoundEffect *victory_sound;
  int state;

  void handle_input();
  void prepare_stage();
  void draw_stage();

 public:
  App();
  ~App();
  void init();
  void new_game();
  void run();
  void cleanup();
  void draw_text(SDL_Point pos, int height, int align, SDL_Color text_color,
                 const char *msg);
  void draw_image(SDL_Point pos, Image *image);
};

#endif
