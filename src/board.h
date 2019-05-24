
#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>
#include <stdint.h>

#include <SDL.h>

#include "image.h"

const char *board;

const uint8_t BOARD_HEIGHT;
const uint8_t BOARD_WIDTH;

const char BOARD_FREE;
const char BOARD_BOX;
const char BOARD_Target;
const char BOARD_BOXTarget;
const char BOARD_WALL;

SDL_Surface *WINSURFACE;
SDL_Window *WINDOW;

int board_targets_left();

void draw_fields(const int xy[][2], const int length);

void draw_field(const int x, const int y);

void draw_board();
#endif
