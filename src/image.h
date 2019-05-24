
#ifndef IMAGE_H
#define IMAGE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include <SDL.h>

#include "image_box.h"
#include "image_boxtarget.h"
#include "image_free.h"
#include "image_player.h"
#include "image_target.h"
#include "image_wall.h"

SDL_Surface *IMG_BOX;
SDL_Surface *IMG_BOXTARGET;
SDL_Surface *IMG_FREE;
SDL_Surface *IMG_PLAYER;
SDL_Surface *IMG_TARGET;
SDL_Surface *IMG_WALL;

void deinitialize_images();

bool initialize_images();

bool b64_decode(char **out, size_t *out_len, const char* b64);

#endif
