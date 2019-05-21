
#ifndef IMAGE_H
#define IMAGE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "image_box.h"
#include "image_player.h"
#include "image_wall.h"

char* b64_to_binary(const char** b64_buffer);

#endif
