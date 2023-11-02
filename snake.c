#include "snake.h"

#if 0 
//FULLSCREEN
#define WINDOW_X 0
#define WINDOW_Y 0
#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

#else
// Stream
#define WINDOW_WIDTH 1770
#define WINDOW_HEIGHT 1405
#define WINDOW_X 10
#define WINDOW_Y -20
#endif

int main() {

  SDL_Window *window;
  SDL_Renderer *renderer;

  if (SDL_INIT_VIDEO < 0) {
    fprintf(stderr, "ERROR: SDL_INIT_VIDEO");
  }

  window = SDL_CreateWindow("Snake", WINDOW_X, WINDOW_Y, WINDOW_WIDTH,
                            WINDOW_HEIGHT, SDL_WINDOW_BORDERLESS);

  if (!window) {
    fprintf(stderr, "ERROR: !window");
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  if (!renderer) {
    fprintf(stderr, "!renderer");
  }

  SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 255);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);

  SDL_Delay(2000);

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
