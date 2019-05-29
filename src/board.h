
#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>
#include <stdint.h>

#include <SDL.h>

#include "image.h"
#include "board.h"


char BOARD[];

const uint8_t BOARD_HEIGHT;
const uint8_t BOARD_WIDTH;

const char BOARD_FREE;
const char BOARD_BOX;
const char BOARD_TARGET;
const char BOARD_BOXTARGET;
const char BOARD_WALL;

SDL_Surface *WINSURFACE;
SDL_Window *WINDOW;

char board_get(int x, int y);

void board_set(int x, int y, char b);

int board_targets_left();

void show_fields(const int xy[][2], const int length);

void show_field(const int x, const int y);

void show_board();


#endif
