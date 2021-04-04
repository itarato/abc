#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <iostream>
#include <string>

#define WIN_WIDTH 1800
#define WIN_HEIGHT 600
#define SDL_WIN_FLAGS 0
#define SDL_RENDER_FLAGS (SDL_RENDERER_ACCELERATED)
#define SDL_COLOR_GREEN (SDL_Color{80, 240, 40})
#define SDL_COLOR_RED (SDL_Color{240, 40, 40})
#define LETTER_FIRST 'A'
#define LETTER_LAST 'C'
#define ABC_LEN (LETTER_LAST - LETTER_FIRST + 1)
#define PANIC(s)        \
  {                     \
    cerr << s << endl;  \
    exit(EXIT_FAILURE); \
  }

using namespace std;

char rand_char() {
  return LETTER_FIRST + rand() % (LETTER_LAST - LETTER_FIRST + 1);
}

class App {
 private:
  bool is_over;
  SDL_Window *win;
  SDL_Renderer *renderer;
  TTF_Font *font;
  char pressed_char;
  char answer_char;
  SDL_Texture *image_textures[ABC_LEN];

  void handle_input() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      SDL_Scancode scancode = e.key.keysym.scancode;

      switch (e.type) {
        case SDL_QUIT:
          is_over = true;
          break;
        case SDL_KEYDOWN:
          if (scancode >= SDL_SCANCODE_A && scancode <= SDL_SCANCODE_Z) {
            pressed_char = 'A' - SDL_SCANCODE_A + scancode;
          } else if (scancode == SDL_SCANCODE_ESCAPE) {
            is_over = true;
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
               answer_char - LETTER_FIRST);

    string pressed_char_string{pressed_char};
    draw_text(SDL_Rect{WIN_WIDTH / 3, 0, WIN_WIDTH / 3, WIN_HEIGHT},
              SDL_COLOR_RED, pressed_char_string.c_str());

    string answer_char_string{answer_char};
    draw_text(SDL_Rect{WIN_WIDTH / 3 * 2, 0, WIN_WIDTH / 3, WIN_HEIGHT},
              SDL_COLOR_GREEN, answer_char_string.c_str());
  }

  void present_scene() { SDL_RenderPresent(renderer); }

 public:
  App() : is_over(false), win(nullptr), renderer(nullptr), font(nullptr) {}
  ~App() {}

  void init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) PANIC("SDL Init Error");
    if (TTF_Init() == -1) PANIC("TTF_Init failed");

    int img_flags = IMG_INIT_JPG;
    if ((IMG_Init(IMG_INIT_JPG) & img_flags) == 0) PANIC("IMG_Init failed");

    win = SDL_CreateWindow("Lennox University", SDL_WINDOWPOS_CENTERED,
                           SDL_WINDOWPOS_CENTERED, WIN_WIDTH, WIN_HEIGHT,
                           SDL_WIN_FLAGS);

    if (win == nullptr) PANIC("Window creation error");

    renderer = SDL_CreateRenderer(win, -1, SDL_RENDER_FLAGS);

    if (renderer == nullptr) PANIC("Renderer creation error");

    font = TTF_OpenFont("fonts/Merriweather-Black.ttf", 600);
    if (font == nullptr) PANIC("Cannot open font");

    for (int i = LETTER_FIRST; i <= LETTER_LAST; i++) {
      char image_name[32];
      sprintf(image_name, "images/%c.jpg", i);
      SDL_RWops *rwops = SDL_RWFromFile(image_name, "rb");
      SDL_Surface *image_surface = IMG_LoadJPG_RW(rwops);
      if (image_surface == nullptr) PANIC("Cannot load image");

      image_textures[i - LETTER_FIRST] =
          SDL_CreateTextureFromSurface(renderer, image_surface);
      SDL_FreeSurface(image_surface);
    }

    new_game();
  }

  void new_game() {
    pressed_char = ' ';
    answer_char = rand_char();
  }

  void run() {
    while (!is_over) {
      prepare_scene();
      handle_input();
      present_scene();

      SDL_Delay(16);
    }
  }

  void cleanup() {
    TTF_CloseFont(font);
    TTF_Quit();

    IMG_Quit();

    for (int i = LETTER_FIRST; i <= LETTER_LAST; i++) {
      SDL_DestroyTexture(image_textures[i - LETTER_FIRST]);
    }

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

  void draw_image(SDL_Rect rect, int i) {
    SDL_RenderCopy(renderer, image_textures[i], NULL, &rect);
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
