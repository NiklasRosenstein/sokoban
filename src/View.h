#ifndef VIEW_H
#define VIEW_H

#include "stdbool.h"

#include <SDL.h>

#include "Board.h"
#include "Soko.h"

extern SDL_Window   *WINDOW;
extern SDL_Renderer *WINRENDERER;

enum Direction {
    FACING_RIGHT,
    FACING_LEFT,
    FACING_UP,
    FACING_DOWN
};

bool initialize();

void deinitialize();

void initialize_scaler();

bool render();

bool render_animated(const SDL_Texture *text, int x, int y, uint16_t frame_time, uint8_t frames, bool overlay);

bool render_with_direction(const SDL_Texture *text, int x, int y, enum Direction direction);

bool render_with_variation(const SDL_Texture *text, int x, int y, int variations, int commonness);

bool render_permutated(const SDL_Texture *text, int x, int y);

bool render_static(const SDL_Texture *text, int x, int y);

#endif
