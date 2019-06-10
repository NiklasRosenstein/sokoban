
#ifndef IMAGES_H
#define IMAGES_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include <SDL.h>
#include <SDL_image.h>

#include "Image_box.h"
#include "Image_boxtarget.h"
#include "Image_free.h"
#include "Image_player.h"
#include "Image_target.h"
#include "Image_wall.h"
#include "Levels.h"

SDL_Surface *IMG_BOX;
SDL_Surface *IMG_BOXTARGET;
SDL_Surface *IMG_FREE;
SDL_Surface *IMG_PLAYER;
SDL_Surface *IMG_TARGET;
SDL_Surface *IMG_WALL;

void deinitialize_images();

bool initialize_images();

#endif
