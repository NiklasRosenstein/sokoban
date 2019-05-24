#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL.h>
#include <SDL_image.h>

#include "image.h"
#include "board.h"

bool loadIMG_from_file(SDL_Surface **image, const char *filename);

bool loadIMG_from_buffer(SDL_Surface **image, const char *buffer, const int length);

bool initialize(SDL_Window**);

int main(int argc, char* argv[]);


#endif
