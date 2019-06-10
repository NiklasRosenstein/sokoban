
#ifndef LEVELS_H
#define LEVELS_H

#include <stdbool.h>

#include "Soko.h"
#include "Board.h"

extern const char *DEFAULT_LEVEL;

bool load_level_from_buffer(const char *buf, long len);

bool load_level_from_file(const char *filepath);

#endif
