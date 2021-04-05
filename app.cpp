#include "app.h"

#include <string>

#include "util.h"

void App::handle_input() {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    SDL_Scancode scancode = e.key.keysym.scancode;

    switch (e.type) {
      case SDL_QUIT:
        should_finish = true;
        break;
      case SDL_KEYDOWN:
        if (scancode >= SDL_SCANCODE_A && scancode <= SDL_SCANCODE_Z) {
          pressed_char = 'A' - SDL_SCANCODE_A + scancode;

          if (pressed_char == answer_char) {
            state = STATE_WON_GAME;
          }
        } else if (scancode == SDL_SCANCODE_ESCAPE) {
          should_finish = true;
        }
        break;
      default:
        break;
    }
  }
}

void App::prepare_stage() {
  if (state == STATE_WON_GAME) {
    victory_sound->play();
  }

  SDL_SetRenderDrawColor(renderer, 42, 42, 42, 255);
  SDL_RenderClear(renderer);

  draw_image({0, 0}, animal_image_textures[answer_char - LETTER_FIRST]);

  draw_image({0, 0}, overlay_image);

  if (state == STATE_WON_GAME) {
    draw_image({0, 0}, celebration_image);
  }

  SDL_Color text_color;
  if (state == STATE_WON_GAME) {
    text_color = SDL_COLOR_GREEN;
  } else {
    text_color = SDL_COLOR_WHITE;
  }

  std::string pressed_char_string{pressed_char};
  draw_text({WIN_WIDTH - 96, 0}, 128, TEXT_ALIGN_CENTER, SDL_COLOR_WHITE,
            pressed_char_string.c_str());

  std::string answer_char_string{answer_char};
  draw_text({(int)(WIN_WIDTH / 3 * 2), -FONT_PADDING},
            WIN_HEIGHT - FONT_PADDING, TEXT_ALIGN_CENTER, text_color,
            answer_char_string.c_str());

  draw_text({(int)(WIN_WIDTH / 3 * 2), WIN_HEIGHT - 96}, 64, TEXT_ALIGN_CENTER,
            SDL_COLOR_BLACK, names[answer_char - 'A']);
}

void App::draw_stage() {
  Engine::draw_stage();

  if (state == STATE_WON_GAME) {
    SDL_Delay(2000);
    new_game();
  } else {
    SDL_Delay(16);
  }
}

App::App() : Engine("Lennox University"), state(STATE_NORMAL) {}

App::~App() {
  delete victory_sound;

  for (int i = LETTER_FIRST; i <= LETTER_LAST; i++) {
    delete animal_image_textures[i - LETTER_FIRST];
  }
  delete celebration_image;
  delete overlay_image;
}

void App::init() {
  Engine::init();

  for (int i = LETTER_FIRST; i <= LETTER_LAST; i++) {
    char image_name[32];
    sprintf(image_name, "images/%c.jpg", i);

    animal_image_textures[i - LETTER_FIRST] = new JPGImage;
    animal_image_textures[i - LETTER_FIRST]->init(renderer, image_name);
  }

  celebration_image = new PNGImage;
  celebration_image->init(renderer, "images/celebrate.png");

  overlay_image = new PNGImage;
  overlay_image->init(renderer, "images/overlay.png");

  victory_sound = new SoundEffect("sounds/victory.wav");

  new_game();
}

void App::new_game() {
  pressed_char = ' ';
  answer_char = rand_char();
  state = STATE_NORMAL;
}

void App::cleanup() {
  TTF_CloseFont(font);
  TTF_Quit();

  IMG_Quit();

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(win);
  SDL_Quit();
}
