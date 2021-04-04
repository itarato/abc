#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <iostream>
#include <string>

#define STATE_NORMAL 0
#define STATE_EXIT 1
#define STATE_WON_GAME 2

#define WIN_WIDTH 1800
#define WIN_HEIGHT 600

#define SDL_WIN_FLAGS 0
#define SDL_RENDER_FLAGS (SDL_RENDERER_ACCELERATED)
#define SDL_COLOR_GREEN (SDL_Color{80, 240, 40})
#define SDL_COLOR_RED (SDL_Color{240, 40, 40})
#define SDL_COLOR_BLUE (SDL_Color{40, 80, 240})
#define SDL_COLOR_WHITE (SDL_Color{240, 240, 240})
#define SDL_COLOR_BLACK (SDL_Color{40, 40, 40})

#define LETTER_FIRST 'A'
#define LETTER_LAST 'Z'
#define ABC_LEN (LETTER_LAST - LETTER_FIRST + 1)
#define FONT_PADDING 64

#define PANIC(s)        \
  {                     \
    cerr << s << endl;  \
    exit(EXIT_FAILURE); \
  }

static const char *names[] = {
    "Antelope", "Bumble bee",   "Cat",     "Dog",     "Elephant", "Fox",
    "Gorilla",  "Hippopotamus", "Iguana",  "Jaguar",  "Koala",    "Lemur",
    "Meerkat",  "Nautilus",     "Ostrich", "Panther", "Quokka",   "Rabbit",
    "Snake",    "Tapir",        "Urchin",  "Vulture", "Wolf",     "Xerus",
    "Yak",      "Zebra"};

using namespace std;

char rand_char() {
  return LETTER_FIRST + rand() % (LETTER_LAST - LETTER_FIRST + 1);
}

class SoundEffect {
 private:
  SDL_AudioSpec wav_spec;
  Uint32 wav_length;
  Uint8 *wav_buffer;
  SDL_AudioDeviceID device_id;

 public:
  SoundEffect(const char *effect_file_name) {
    if (!SDL_LoadWAV(effect_file_name, &wav_spec, &wav_buffer, &wav_length))
      PANIC("Cannot load wav");

    device_id = SDL_OpenAudioDevice(NULL, 0, &wav_spec, NULL, 0);
    if (!device_id) PANIC("Cannot open audio device");
  }

  ~SoundEffect() {
    SDL_CloseAudioDevice(device_id);
    SDL_FreeWAV(wav_buffer);
  }

  void play() {
    stop();
    if (SDL_QueueAudio(device_id, wav_buffer, wav_length) == -1)
      PANIC("Cannot queue audio");

    SDL_PauseAudioDevice(device_id, 0);
  }

  void stop() { SDL_ClearQueuedAudio(device_id); }
};

class Image {
 protected:
  virtual SDL_Surface *load_surface(SDL_RWops *rwops) = 0;

  Image() : text(nullptr) {}

 public:
  SDL_Texture *text;

  void init(SDL_Renderer *renderer, const char *file_name) {
    SDL_RWops *rwops = SDL_RWFromFile(file_name, "rb");
    SDL_Surface *surf = load_surface(rwops);
    if (surf == nullptr) PANIC("Cannot load image");

    text = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
  }

  ~Image() {
    if (text) SDL_DestroyTexture(text);
  }
};

class JPGImage final : public Image {
 protected:
  SDL_Surface *load_surface(SDL_RWops *rwops) { return IMG_LoadJPG_RW(rwops); }
};

class PNGImage final : public Image {
 protected:
  SDL_Surface *load_surface(SDL_RWops *rwops) { return IMG_LoadPNG_RW(rwops); }
};

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

  void handle_input() {
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

  void prepare_scene() {
    SDL_SetRenderDrawColor(renderer, 42, 42, 42, 255);
    SDL_RenderClear(renderer);

    draw_image(SDL_Rect{0, 0, WIN_WIDTH / 3, WIN_HEIGHT},
               animal_image_textures[answer_char - LETTER_FIRST]->text);

    draw_image(SDL_Rect{0, 0, WIN_WIDTH, WIN_HEIGHT}, overlay_image->text);

    if (state == STATE_WON_GAME) {
      draw_image(SDL_Rect{0, 0, WIN_WIDTH, WIN_HEIGHT},
                 celebration_image->text);

      victory_sound->play();
    }

    SDL_Color text_color;
    if (state == STATE_WON_GAME) {
      text_color = SDL_COLOR_GREEN;
    } else {
      text_color = SDL_COLOR_WHITE;
    }

    string pressed_char_string{pressed_char};
    draw_text(SDL_Rect{WIN_WIDTH - 96, 32, 64, 64}, SDL_COLOR_WHITE,
              pressed_char_string.c_str());

    string answer_char_string{answer_char};
    draw_text(SDL_Rect{(int)(WIN_WIDTH / 3 * 1.5) + FONT_PADDING, -FONT_PADDING,
                       (WIN_WIDTH / 3) - (FONT_PADDING << 1),
                       WIN_HEIGHT - FONT_PADDING},
              text_color, answer_char_string.c_str());

    int name_len = strlen(names[answer_char - 'A']);
    draw_text(SDL_Rect{(int)(WIN_WIDTH / 3 * 2) - (name_len * 32),
                       WIN_HEIGHT - 96, name_len * 64, 64},
              SDL_COLOR_BLACK, names[answer_char - 'A']);
  }

  void present_scene() { SDL_RenderPresent(renderer); }

 public:
  App() : win(nullptr), renderer(nullptr), font(nullptr), state(STATE_NORMAL) {}
  ~App() {
    delete victory_sound;

    for (int i = LETTER_FIRST; i <= LETTER_LAST; i++) {
      delete animal_image_textures[i - LETTER_FIRST];
    }
    delete celebration_image;
    delete overlay_image;
  }

  void init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) PANIC("SDL Init Error");
    if (TTF_Init() == -1) PANIC("TTF_Init failed");

    int img_flags = IMG_INIT_JPG | IMG_INIT_PNG;
    if ((IMG_Init(img_flags) & img_flags) != img_flags)
      PANIC("IMG_Init failed");

    win = SDL_CreateWindow("Lennox University", SDL_WINDOWPOS_CENTERED,
                           SDL_WINDOWPOS_CENTERED, WIN_WIDTH, WIN_HEIGHT,
                           SDL_WIN_FLAGS);

    if (win == nullptr) PANIC("Window creation error");

    renderer = SDL_CreateRenderer(win, -1, SDL_RENDER_FLAGS);

    if (renderer == nullptr) PANIC("Renderer creation error");

    font = TTF_OpenFont("fonts/Ubuntu-Bold.ttf", 600);
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

  void new_game() {
    pressed_char = ' ';
    answer_char = rand_char();
    state = STATE_NORMAL;
  }

  void run() {
    while (state != STATE_EXIT) {
      handle_input();
      prepare_scene();
      present_scene();

      if (state == STATE_WON_GAME) {
        new_game();
        SDL_Delay(2000);
      }

      SDL_Delay(16);
    }
  }

  void cleanup() {
    TTF_CloseFont(font);
    TTF_Quit();

    IMG_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();
  }

  void draw_text(SDL_Rect rect, SDL_Color text_color, const char *msg) {
    SDL_Surface *text_surface = TTF_RenderText_Solid(font, msg, text_color);
    if (text_surface == nullptr) PANIC("Cannot render text");

    int optimal_w;
    int optimal_h;
    if (TTF_SizeText(font, msg, &optimal_w, &optimal_h) == -1)
      PANIC("Cannot check text size");

    SDL_Texture *text = SDL_CreateTextureFromSurface(renderer, text_surface);
    SDL_Rect final_rect{rect.x, rect.y, (int)((rect.h * optimal_w) / optimal_h),
                        rect.h};
    SDL_RenderCopy(renderer, text, NULL, &final_rect);
    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text);
  }

  void draw_image(SDL_Rect rect, SDL_Texture *text) {
    SDL_RenderCopy(renderer, text, NULL, &rect);
  }
};

App app;

void cleanup() { app.cleanup(); }

int main() {
  srand(time(NULL));

  app.init();
  app.run();

  atexit(cleanup);

  exit(EXIT_SUCCESS);
}
