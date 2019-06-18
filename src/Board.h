#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>
#include <stdint.h>

#include <SDL.h>

#include "Images.h"
#include "View.h"

extern char *BOARD;

extern uint8_t BOARD_WIDTH;
extern uint8_t BOARD_HEIGHT;

extern const char BOARD_FREE;
extern const char BOARD_BOX;
extern const char BOARD_TARGET;
extern const char BOARD_BOXTARGET;
extern const char BOARD_WALL;

uint8_t board_targets_left();

char board_get(uint8_t x, uint8_t y);

void board_set(uint8_t x, uint8_t y, char b);

void draw_board();

void deinitialize_board();

#endif
