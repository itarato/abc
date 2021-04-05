#ifndef APP_H
#define APP_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "engine.h"
#include "image.h"
#include "sound.h"
#include "util.h"

class App : public Engine {
 private:
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
  void cleanup();
};

#endif
