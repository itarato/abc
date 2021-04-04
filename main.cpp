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

#define LETTER_FIRST 'A'
#define LETTER_LAST 'K'
#define ABC_LEN (LETTER_LAST - LETTER_FIRST + 1)
#define FONT_PADDING 64

#define PANIC(s)        \
  {                     \
    cerr << s << endl;  \
    exit(EXIT_FAILURE); \
  }

using namespace std;

char rand_char() {
  return LETTER_FIRST + rand() % (LETTER_LAST - LETTER_FIRST + 1);
}

class SoundEffect {
 private:
  SDL_AudioSpec wav_spec;
  Uint32 wav_length;
  Uint8 *wav_buffer;
  SDL_AudioDeviceID deviceId;

 public:
  SoundEffect(const char *effect_file_name) {
    if (!SDL_LoadWAV(effect_file_name, &wav_spec, &wav_buffer, &wav_length))
      PANIC("Cannot load wav");

    deviceId = SDL_OpenAudioDevice(NULL, 0, &wav_spec, NULL, 0);
    if (!deviceId) PANIC("Cannot open audio device");
  }

  ~SoundEffect() {
    SDL_CloseAudioDevice(deviceId);
    SDL_FreeWAV(wav_buffer);
  }

  void play() {
    if (SDL_QueueAudio(deviceId, wav_buffer, wav_length) == -1)
      PANIC("Cannot queue audio");

    SDL_PauseAudioDevice(deviceId, 0);
  }
};

class App {
 private:
  SDL_Window *win;
  SDL_Renderer *renderer;
  TTF_Font *font;
  char pressed_char;
  char answer_char;
  SDL_Texture *animal_image_textures[ABC_LEN];
  SDL_Texture *celebration_image_texture;
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
               animal_image_textures[answer_char - LETTER_FIRST]);

    SDL_Color pressed_color;
    if (state == STATE_WON_GAME) {
      pressed_color = SDL_COLOR_GREEN;
    } else {
      pressed_color = SDL_COLOR_RED;
    }

    string pressed_char_string{pressed_char};
    draw_text(SDL_Rect{(WIN_WIDTH / 3) + FONT_PADDING, 0,
                       (WIN_WIDTH / 3) - (FONT_PADDING << 1), WIN_HEIGHT},
              pressed_color, pressed_char_string.c_str());

    string answer_char_string{answer_char};
    draw_text(SDL_Rect{(WIN_WIDTH / 3 * 2) + FONT_PADDING, 0,
                       (WIN_WIDTH / 3) - (FONT_PADDING << 1), WIN_HEIGHT},
              SDL_COLOR_GREEN, answer_char_string.c_str());

    if (state == STATE_WON_GAME) {
      draw_image(SDL_Rect{0, 0, WIN_WIDTH, WIN_HEIGHT},
                 celebration_image_texture);

      victory_sound->play();
    }
  }

  void present_scene() { SDL_RenderPresent(renderer); }

 public:
  App() : win(nullptr), renderer(nullptr), font(nullptr), state(STATE_NORMAL) {}
  ~App() {}

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
      SDL_RWops *rwops = SDL_RWFromFile(image_name, "rb");
      SDL_Surface *image_surface = IMG_LoadJPG_RW(rwops);
      if (image_surface == nullptr) PANIC("Cannot load animal image");

      animal_image_textures[i - LETTER_FIRST] =
          SDL_CreateTextureFromSurface(renderer, image_surface);
      SDL_FreeSurface(image_surface);
    }

    SDL_RWops *rwops = SDL_RWFromFile("images/celebrate.png", "rb");
    SDL_Surface *image_surface = IMG_LoadPNG_RW(rwops);
    if (image_surface == nullptr) PANIC("Cannot load celebration image");

    celebration_image_texture =
        SDL_CreateTextureFromSurface(renderer, image_surface);
    SDL_FreeSurface(image_surface);

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
        SDL_Delay(3000);
      }

      SDL_Delay(16);
    }
  }

  void cleanup() {
    TTF_CloseFont(font);
    TTF_Quit();

    IMG_Quit();

    for (int i = LETTER_FIRST; i <= LETTER_LAST; i++) {
      SDL_DestroyTexture(animal_image_textures[i - LETTER_FIRST]);
    }
    SDL_DestroyTexture(celebration_image_texture);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();
  }

  void draw_text(SDL_Rect rect, SDL_Color text_color, const char *msg) {
    SDL_Surface *text_surface = TTF_RenderText_Solid(font, msg, text_color);
    if (text_surface == nullptr) PANIC("Cannot render text");

    SDL_Texture *text = SDL_CreateTextureFromSurface(renderer, text_surface);
    SDL_RenderCopy(renderer, text, NULL, &rect);
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
