#include "snake.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_rect.h>
#include <stdlib.h>
#include <time.h>
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

#define GRID_SIZE 20
#define GRID_DIM 1000

enum {
  SNAKE_UP,
  SNAKE_DOWN,
  SNAKE_LEFT,
  SNAKE_RIGHT,
};

typedef struct {
  int x;
  int y;
} apple;

apple Apple;

struct snake {
  int x;
  int y;
  int dir;

  struct snake *next;
};

typedef struct snake Snake;

Snake *head;
Snake *tail;

void init_snake() {
  Snake *new = malloc(sizeof(Snake));
  new->x = rand() % GRID_SIZE / 2 + (GRID_SIZE / 4);
  new->y = rand() % GRID_SIZE / 2 + (GRID_SIZE / 4);
  new->dir = SNAKE_UP;
  new->next = NULL;

  head = new;
  tail = new;
}

void increase_snake() {
  Snake *new = malloc(sizeof(Snake));

  switch (tail->dir) {
  case SNAKE_UP:
    new->x = tail->x;
    new->y = tail->y + 1;
    break;
  case SNAKE_DOWN:
    new->x = tail->x;
    new->y = tail->y - 1;
    break;
  case SNAKE_LEFT:
    new->x = tail->x + 1;
    new->y = tail->y;
    break;
  case SNAKE_RIGHT:
    new->x = tail->x - 1;
    new->y = tail->y;
    break;
  }

  new->dir = tail->dir;
  new->next = NULL;
  tail->next = new;
  tail = new;
}

void move_snake() {
  int prev_x = head->x;
  int prev_y = head->y;
  int prev_dir = head->dir;

  switch (head->dir) {
  case SNAKE_UP:
    head->y--;
    break;
  case SNAKE_DOWN:
    head->y++;
    break;
  case SNAKE_LEFT:
    head->x--;
    break;
  case SNAKE_RIGHT:
    head->x++;
    break;
  }

  Snake *track = head;

  if (track->next != NULL) {
    track = track->next;
  }

  while (track != NULL) {
    int save_x = track->x;
    int save_y = track->y;
    int save_dir = track->dir;

    track->x = prev_x;
    track->y = prev_y;
    track->dir = prev_dir;

    track = track->next;

    prev_x = save_x;
    prev_y = save_y;
    prev_dir = save_dir;
  }
}

void reset_snake() {
  Snake *track = head;
  Snake *temp;

  while (track != NULL) {
    temp = track;
    track = track->next;
    free(temp);
  }

  init_snake();
  increase_snake();
  increase_snake();
  increase_snake();
}

void render_snake(SDL_Renderer *renderer, int x, int y) {
  SDL_SetRenderDrawColor(renderer, 0x9C, 0xC2, 0x32, 255);

  int seg_size = GRID_DIM / GRID_SIZE;
  SDL_Rect seg;
  seg.w = seg_size;
  seg.h = seg_size;

  Snake *track = head;

  while (track != NULL) {
    seg.x = x + track->x * seg_size;
    seg.y = y + track->y * seg_size;
    SDL_RenderFillRect(renderer, &seg);
    track = track->next;
  }
}

void render_grid(SDL_Renderer *renderer, int x, int y) {
  SDL_SetRenderDrawColor(renderer, 0x55, 0x55, 0xFF, 255);
#if 0 
  int cell_size = GRID_DIM / GRID_SIZE;
  SDL_Rect cell;
  cell.w = cell_size;
  cell.h = cell_size;

  for (int i = 0; i < GRID_SIZE; i++) {
    for (int j = 0; j < GRID_SIZE; j++) {
      cell.x = x + (i * cell_size);
      cell.y = y + (j * cell_size);
      SDL_RenderDrawRect(renderer, &cell);
    }
  }
#else
  SDL_Rect outline;
  outline.x = x;
  outline.y = y;
  outline.w = GRID_DIM;
  outline.h = GRID_DIM;

  SDL_RenderDrawRect(renderer, &outline);

#endif
  return;
}

void gen_apple() {
  bool is_snake;

  do {
    is_snake = false;
    Apple.x = rand() % GRID_SIZE;
    Apple.y = rand() % GRID_SIZE;

    Snake *track = head;

    while (track != NULL) {
      if (track->x == Apple.x && track->y == Apple.y) {
        is_snake = true;
      }

      track = track->next;
    }
  } while (is_snake);
}

void render_apple(SDL_Renderer *renderer, int x, int y) {
  SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 255);
  int apple_size = GRID_DIM / GRID_SIZE;
  SDL_Rect apple_rect;
  apple_rect.w = apple_size;
  apple_rect.h = apple_size;
  apple_rect.x = x + Apple.x * apple_size;
  apple_rect.y = y + Apple.y * apple_size;
  SDL_RenderFillRect(renderer, &apple_rect);
}

void eat_apple() {
  if (head->x == Apple.x && head->y == Apple.y) {
    gen_apple();
    increase_snake();
  }
}

void detect_crash() {
  if (head->x < 0 || head->x >= GRID_SIZE || head->y < 0 ||
      head->y >= GRID_SIZE) {
    reset_snake();
  }

  Snake *track = head;

  if (track->next != NULL) {
    track = track->next;
  }

  while (track != NULL) {
    if (head->x == track->x && track->y == head->y) {
      reset_snake();
      break;
    }
    track = track->next;
  }
}

int main() {
  srand(time(0));

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "ERROR: SDL_INIT_VIDEO\n");
    return 1;
  }

  SDL_Window *window =
      SDL_CreateWindow("Snake", WINDOW_X, WINDOW_Y, WINDOW_WIDTH, WINDOW_HEIGHT,
                       SDL_WINDOW_BORDERLESS);

  if (!window) {
    fprintf(stderr, "ERROR: !window\n");
    SDL_Quit();
    return 1;
  }

  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  if (!renderer) {
    fprintf(stderr, "!renderer\n");
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  int grid_x = (WINDOW_WIDTH / 2) - (GRID_DIM / 2);
  int grid_y = (WINDOW_HEIGHT / 2) - (GRID_DIM / 2);

  init_snake();
  increase_snake();
  increase_snake();
  increase_snake();
  gen_apple();

  bool quit = false;
  SDL_Event event;

  while (!quit) {
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        quit = true;
        break;
      case SDL_KEYUP:
        break;
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
        case SDLK_ESCAPE:
          quit = true;
          break;
        case SDLK_UP:
          head->dir = SNAKE_UP;
          break;
        case SDLK_DOWN:
          head->dir = SNAKE_DOWN;
          break;
        case SDLK_LEFT:
          head->dir = SNAKE_LEFT;
          break;
        case SDLK_RIGHT:
          head->dir = SNAKE_RIGHT;
          break;
        }
        break;
      }
    }

    SDL_RenderClear(renderer);
    // RENDER LOOP
    move_snake();
    eat_apple();
    detect_crash();
    render_grid(renderer, grid_x, grid_y);
    render_snake(renderer, grid_x, grid_y);
    render_apple(renderer, grid_x, grid_y);
    // RENDER LOOP END

    SDL_SetRenderDrawColor(renderer, 0x11, 0x11, 0x11, 255);
    SDL_RenderPresent(renderer);
    SDL_Delay(100);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
