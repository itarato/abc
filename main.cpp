#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <unistd.h>

#include <iostream>
#include <string>

#define WIN_WIDTH 1800
#define WIN_HEIGHT 600
#define SDL_WIN_FLAGS 0
#define SDL_RENDER_FLAGS (SDL_RENDERER_ACCELERATED)

using namespace std;

class App {
 private:
  bool is_over;
  SDL_Window *win;
  SDL_Renderer *renderer;
  TTF_Font *font;
  char pressed_char;

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

    string s{pressed_char};
    draw_text(SDL_Rect{0, 0, WIN_WIDTH / 3, WIN_HEIGHT}, s.c_str());
    draw_image(SDL_Rect{WIN_WIDTH / 3 * 2, 0, WIN_WIDTH / 3, WIN_HEIGHT},
               "images/a.jpg");
  }

  void present_scene() { SDL_RenderPresent(renderer); }

 public:
  App()
      : is_over(false),
        win(nullptr),
        renderer(nullptr),
        font(nullptr),
        pressed_char(' ') {}
  ~App() {}

  void init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
      std::cerr << "SDL Init Error\n";
      exit(EXIT_FAILURE);
    }

    if (TTF_Init() == -1) {
      cerr << "TTF_Init failed\n";
      exit(EXIT_FAILURE);
    }

    int img_flags = IMG_INIT_JPG;
    if ((IMG_Init(IMG_INIT_JPG) & img_flags) == 0) {
      cerr << "IMG_Init failed\n";
      exit(EXIT_FAILURE);
    }

    win = SDL_CreateWindow("Lennox University", SDL_WINDOWPOS_CENTERED,
                           SDL_WINDOWPOS_CENTERED, WIN_WIDTH, WIN_HEIGHT,
                           SDL_WIN_FLAGS);

    if (win == nullptr) {
      cerr << "Window creation error\n";
      exit(EXIT_FAILURE);
    }

    renderer = SDL_CreateRenderer(win, -1, SDL_RENDER_FLAGS);

    if (renderer == nullptr) {
      cerr << "Renderer creation error\n";
      exit(EXIT_FAILURE);
    }

    font = TTF_OpenFont("fonts/Merriweather-Black.ttf", 600);
    if (font == nullptr) {
      cerr << "Cannot open font\n";
      exit(EXIT_FAILURE);
    }
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

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();
  }

  void draw_text(SDL_Rect rect, const char *msg) {
    SDL_Color text_color{200, 0, 100};
    SDL_Surface *text_surface = TTF_RenderText_Solid(font, msg, text_color);
    if (text_surface == nullptr) {
      cerr << "Cannot render text\n";
      exit(EXIT_FAILURE);
    }

    SDL_Texture *text = SDL_CreateTextureFromSurface(renderer, text_surface);
    SDL_RenderCopy(renderer, text, NULL, &rect);
    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text);
  }

  void draw_image(SDL_Rect rect, const char *image_name) {
    SDL_RWops *rwops = SDL_RWFromFile(image_name, "rb");
    SDL_Surface *image_surface = IMG_LoadJPG_RW(rwops);
    if (image_surface == nullptr) {
      cerr << "Cannot load image\n";
      exit(EXIT_FAILURE);
    }

    SDL_Texture *text = SDL_CreateTextureFromSurface(renderer, image_surface);
    SDL_RenderCopy(renderer, text, NULL, &rect);
    SDL_FreeSurface(image_surface);
    SDL_DestroyTexture(text);
  }
};

App app;

void cleanup() { app.cleanup(); }

int main() {
  app.init();
  app.run();

  atexit(cleanup);

  exit(EXIT_SUCCESS);
}
