
#ifndef IMAGES_H
#define IMAGES_H

#include <stdio.h>
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

SDL_Texture *IMG_BOX;
SDL_Texture *IMG_BOXTARGET;
SDL_Texture *IMG_FREE;
SDL_Texture *IMG_PLAYER;
SDL_Texture *IMG_TARGET;
SDL_Texture *IMG_WALL;

void deinitialize_images();

bool initialize_images();

#endif
