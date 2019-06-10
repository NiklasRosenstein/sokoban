
#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>
#include <stdint.h>

#include <SDL.h>

#include "Images.h"
#include "Board.h"
#include "Soko.h"


extern char *BOARD;

extern uint8_t BOARD_HEIGHT;
extern uint8_t BOARD_WIDTH;

extern const char BOARD_FREE;
extern const char BOARD_BOX;
extern const char BOARD_TARGET;
extern const char BOARD_BOXTARGET;
extern const char BOARD_WALL;

extern SDL_Surface *WINSURFACE;
extern SDL_Window *WINDOW;

char board_get(int x, int y);

void board_set(int x, int y, char b);

int board_targets_left();

void show_fields(const int xy[][2], const int length);

void show_field(const int x, const int y);

void show_board();

void deinitialize_board();

#endif
