#include "app.h"

#include <string>

#include "util.h"

void App::handle_input() {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    SDL_Scancode scancode = e.key.keysym.scancode;

    switch (e.type) {
      case SDL_QUIT:
        exit_next_frame();
        break;
      case SDL_KEYDOWN:
        if (scancode >= SDL_SCANCODE_A && scancode <= SDL_SCANCODE_Z) {
          pressed_char = 'A' - SDL_SCANCODE_A + scancode;

          if (pressed_char == answer_char) {
            state = STATE_WON_GAME;
          }
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

  draw_image_rel({0.0, 0.0, 1.0 / 3.0, 1.0},
                 animal_image_textures[answer_char - LETTER_FIRST]);

  draw_image_rel({0.0, 0.0, 1.0, 1.0}, overlay_image);

  if (state == STATE_WON_GAME) {
    draw_image_rel({0.0, 0.0, 1.0, 1.0}, celebration_image);
  }

  SDL_Color text_color;
  if (state == STATE_WON_GAME) {
    text_color = SDL_COLOR_GREEN;
  } else {
    text_color = SDL_COLOR_WHITE;
  }

  std::string pressed_char_string{pressed_char};
  draw_text_rel({0.95, 0.05}, 0.2, TEXT_ALIGN_CENTER, SDL_COLOR_WHITE,
                pressed_char_string.c_str());

  std::string answer_char_string{answer_char};
  answer_char_string.push_back(' ');
  answer_char_string.push_back(answer_char | 0x20);
  draw_text_rel({2.0 / 3.0, 0.0}, 0.7, TEXT_ALIGN_CENTER, text_color,
                answer_char_string.c_str());

  draw_text_rel({2.0 / 3.0, 0.8}, 0.15, TEXT_ALIGN_CENTER, SDL_COLOR_BLACK,
                names[answer_char - 'A']);
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

App::App()
    : Engine("Lennox University", "fonts/Ubuntu-Bold.ttf", {1800, 600}),
      state(STATE_NORMAL) {}

App::~App() {
  delete victory_sound;

  for (int i = LETTER_FIRST; i <= LETTER_LAST; i++) {
    delete animal_image_textures[i - LETTER_FIRST];
  }
  delete celebration_image;
  delete overlay_image;

  Engine::~Engine();
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

  char candidate;

  for (;;) {
    candidate = rand_char();
    if (answer_char != candidate) break;
  }

  answer_char = candidate;

  state = STATE_NORMAL;
}
