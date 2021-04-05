#include "app.h"

#include <string>

#include "util.h"

void App::handle_input() {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    SDL_Scancode scancode = e.key.keysym.scancode;

    switch (e.type) {
      case SDL_QUIT:
        state = STATE_EXIT;
        break;
      case SDL_KEYDOWN:
        if (scancode >= SDL_SCANCODE_A && scancode <= SDL_SCANCODE_Z) {
          pressed_char = 'A' - SDL_SCANCODE_A + scancode;

          if (pressed_char == answer_char) {
            state = STATE_WON_GAME;
          }
        } else if (scancode == SDL_SCANCODE_ESCAPE) {
          state = STATE_EXIT;
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

  draw_image(SDL_Rect{0, 0, WIN_WIDTH / 3, WIN_HEIGHT},
             animal_image_textures[answer_char - LETTER_FIRST]->text);

  draw_image(SDL_Rect{0, 0, WIN_WIDTH, WIN_HEIGHT}, overlay_image->text);

  if (state == STATE_WON_GAME) {
    draw_image(SDL_Rect{0, 0, WIN_WIDTH, WIN_HEIGHT}, celebration_image->text);
  }

  SDL_Color text_color;
  if (state == STATE_WON_GAME) {
    text_color = SDL_COLOR_GREEN;
  } else {
    text_color = SDL_COLOR_WHITE;
  }

  std::string pressed_char_string{pressed_char};
  draw_text(SDL_Point{WIN_WIDTH - 96, 0}, 128, TEXT_ALIGN_CENTER,
            SDL_COLOR_WHITE, pressed_char_string.c_str());

  std::string answer_char_string{answer_char};
  draw_text(SDL_Point{(int)(WIN_WIDTH / 3 * 2), -FONT_PADDING},
            WIN_HEIGHT - FONT_PADDING, TEXT_ALIGN_CENTER, text_color,
            answer_char_string.c_str());

  draw_text(SDL_Point{(int)(WIN_WIDTH / 3 * 2), WIN_HEIGHT - 96}, 64,
            TEXT_ALIGN_CENTER, SDL_COLOR_BLACK, names[answer_char - 'A']);
}

void App::draw_stage() {
  SDL_RenderPresent(renderer);

  if (state == STATE_WON_GAME) {
    SDL_Delay(2000);
    new_game();
  } else {
    SDL_Delay(16);
  }
}

App::App()
    : win(nullptr), renderer(nullptr), font(nullptr), state(STATE_NORMAL) {}
App::~App() {
  delete victory_sound;

  for (int i = LETTER_FIRST; i <= LETTER_LAST; i++) {
    delete animal_image_textures[i - LETTER_FIRST];
  }
  delete celebration_image;
  delete overlay_image;
}

void App::init() {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) PANIC("SDL Init Error");
  if (TTF_Init() == -1) PANIC("TTF_Init failed");

  int img_flags = IMG_INIT_JPG | IMG_INIT_PNG;
  if ((IMG_Init(img_flags) & img_flags) != img_flags) PANIC("IMG_Init failed");

  win = SDL_CreateWindow("Lennox University", SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED, WIN_WIDTH, WIN_HEIGHT,
                         SDL_WIN_FLAGS);

  if (win == nullptr) PANIC("Window creation error");

  renderer = SDL_CreateRenderer(win, -1, SDL_RENDER_FLAGS);

  if (renderer == nullptr) PANIC("Renderer creation error");

  font = TTF_OpenFont("fonts/Ubuntu-Bold.ttf", 256);
  if (font == nullptr) PANIC("Cannot open font");

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

void App::run() {
  while (state != STATE_EXIT) {
    handle_input();
    prepare_stage();
    draw_stage();
  }
}

void App::cleanup() {
  TTF_CloseFont(font);
  TTF_Quit();

  IMG_Quit();

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(win);
  SDL_Quit();
}

void App::draw_text(SDL_Point pos, int height, int align, SDL_Color text_color,
                    const char *msg) {
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

void App::draw_image(SDL_Rect rect, SDL_Texture *text) {
  SDL_RenderCopy(renderer, text, NULL, &rect);
}
