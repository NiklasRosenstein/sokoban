#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL.h>
#include <SDL_image.h>

#include "image.h"
#include "board.h"
#include "soko.h"


bool initialize();

void deinitialize();

void do_command(SDL_Keycode key);

int main(int argc, char* argv[]);


#endif
