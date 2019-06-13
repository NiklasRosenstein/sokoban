#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL.h>
#include <SDL_image.h>

#include "Timer.h"
#include "Menu.h"
#include "View.h"

extern uint16_t GAME_TIME;

void game_loop();

#endif
