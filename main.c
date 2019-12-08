/**
 * ================================================
 *
 *         Copyright 2019 Manoel Vilela
 *
 *         Author: Manoel Vilela
 *        Contact: manoel_vilela@engineer.com
 *
 * ===============================================
 */

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 600
#define GAME_TITLE "CConway"
#define BLOCK_SIZE 10
#define FIELD_SIZE 60
#define BOMBS FIELD_SIZE*4

static int field[FIELD_SIZE][FIELD_SIZE];


void init_sdl() {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        fprintf(stderr, "could not initialize sdl2: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
}

SDL_Window *create_window() {
    SDL_Window* window = NULL;

    window = SDL_CreateWindow(GAME_TITLE,
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              SCREEN_WIDTH,
                              SCREEN_HEIGHT,
                              SDL_WINDOW_SHOWN);
    return window;
}

SDL_Surface *create_screen_surface(SDL_Window *window) {
    return SDL_GetWindowSurface(window);
}

void init_field() {
    int bombs = BOMBS;
    int cells = FIELD_SIZE * FIELD_SIZE;
    while (cells > 0 && bombs > 0) {
        int x = rand() % FIELD_SIZE;
        int y = rand() % FIELD_SIZE;
        if (field[x][y] == 0) {
            if (rand() % 2 == 0) {
                field[x][y] = 1;
                bombs--;
            } else {
                field[x][y] = -1;
            }
            cells--;
        } else if (field[x][y] == -1 || field[x][y] == 1) {
            continue;
        }
    }
}

void render_field(SDL_Renderer* renderer) {
    // Set render color to red ( background will be rendered in this color )
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    // Clear winow
    SDL_RenderClear(renderer);

    const int field_gap = 2;
    for (int i = 0; i < FIELD_SIZE; ++i) {
        for (int j = 0; j < FIELD_SIZE; ++j) {
            SDL_Rect r;
            r.x = i * BLOCK_SIZE;
            r.y = j * BLOCK_SIZE;
            r.h = BLOCK_SIZE - field_gap;
            r.w = BLOCK_SIZE - field_gap;
            if (field[i][j] == 1) {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            } else {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            }
            SDL_RenderFillRect(renderer, &r);
        }
    }
    // Render the rect to the screen
    SDL_RenderPresent(renderer);

}

void press_field(int x, int y) {
    int f_x = x / BLOCK_SIZE;
    int f_y = y / BLOCK_SIZE;
    if (f_x < FIELD_SIZE && f_y < FIELD_SIZE) {
        field[f_x][f_y] = !field[f_x][f_y];
    }
}

int neighbours(int x, int y) {
    int count = 0;
    const int n = FIELD_SIZE;
    if (field[(x + 1) % n][y] == 1) ++count;
    if (field[(x - 1) % n][y] == 1) ++count;
    if (field[x][(y + 1) % n] == 1) ++count;
    if (field[x][(y - 1) % n] == 1) ++count;
    if (field[(x + 1) % n][(y + 1) % n] == 1) ++count;
    if (field[(x - 1) % n][(y - 1) % n] == 1) ++count;
    if (field[(x + 1) % n][(y - 1) % n] == 1) ++count;
    if (field[(x - 1) % n][(y + 1) % n] == 1) ++count;
    return count;
}

void next_generation() {
    for (int i = 0; i < FIELD_SIZE; i++) {
        for (int j = 0; j < FIELD_SIZE; j++) {
            int k = neighbours(i, j);
            int x = field[i][j];
            if (x == 1 && (k == 2 || k == 3)) {
                continue;
            } else if (x != 1 && k == 3) {
                field[i][j] = 1;
            } else if (x == 1) {
                field[i][j] = 0;
            }
        }
    }
}


void game_loop(SDL_Renderer* renderer) {
    SDL_Event event;
    int quit = false;
    int generation_flag = true;
    render_field(renderer);
    while (!quit)
    {
        SDL_PollEvent(&event);

        switch (event.type) {
        case SDL_QUIT: {
            quit = true;
            break;
        }
        case SDL_MOUSEBUTTONUP: {
            SDL_MouseButtonEvent *e = (SDL_MouseButtonEvent *) &event;
            press_field(e->x, e->y);
            render_field(renderer);
            break;
        }
        case SDL_KEYDOWN: {
            switch(event.key.keysym.sym)  {
            case SDLK_SPACE: {
                generation_flag = !generation_flag;
                break;
            }
            case SDLK_ESCAPE: {
                quit = true;
                break;
            }
            }
        }

        }

        if (generation_flag) {
            next_generation();
        }

        render_field(renderer);
        // delay
        SDL_Delay(1000/60);
    }
}


int main(void) {
    init_sdl();
    init_field();
    SDL_Window *window = create_window();

    if (window == NULL) {
        fprintf(stderr, "could not create window: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // Setup renderer
    SDL_Renderer* renderer = NULL;
    renderer =  SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    game_loop(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
