#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL.h>
#include <SDL_image.h>

#include "image.h"

bool loadIMG(SDL_Surface**);

bool initialize(SDL_Window**);

int main(int argc, char* argv[]);


#endif
