#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL.h>
#include <SDL_image.h>

#include "Images.h"
#include "Board.h"
#include "Soko.h"


bool initialize();

void deinitialize();

void do_command(SDL_Keycode key);

int main();


#endif
