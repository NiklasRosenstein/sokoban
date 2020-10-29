
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

extern SDL_Texture *IMG_BOX;
extern SDL_Texture *IMG_BOXTARGET;
extern SDL_Texture *IMG_FREE;
extern SDL_Texture *IMG_PLAYER;
extern SDL_Texture *IMG_TARGET;
extern SDL_Texture *IMG_WALL;

void deinitialize_images();

bool initialize_images();

#endif
