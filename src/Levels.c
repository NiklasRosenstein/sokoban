
#include "Levels.h"

const char *DEFAULT_LEVEL =
    "Sokoban\n"
    "##########\n"
    "#@       #\n"
    "#  ####  #\n"
    "#  #.    #\n"
    "# $#     #\n"
    "#  # $   #\n"
    "#  ##.#  #\n"
    "#     #  #\n"
    "#        #\n"
    "##########";

static bool parse_level(const char *raw, long len) {
    // Read width and height of level
    uint8_t w = 0;
    uint8_t h = 0;
    for (int i = 0; i < len; i++) {
        uint8_t line_w = 0;
        while (i < len && raw[i] != '\n') {
            line_w++;
            i++;
        }
        w = (line_w > w) ? line_w : w;
        h++;
    }

    // Parse level
    int soko_x = -1;
    int soko_y = -1;

    char *level = malloc((h * w) * sizeof(char));
    if (level == NULL) {
        fprintf(stderr, "Could not allocate memory for file buffer\n");
        return false;
    }
    for (int i = 0; i < h * w; i++) {
        level[i] = BOARD_FREE;
    }
    {
        int i, j;
        for (i = 0, j = 0; i < len; i++, j++) {
            char c = raw[i];

            // If there is a newline we may have to add padding to
            // the end of the line if it is not already done.
            if (c == '\n') {
                if (j % w != 0) {
                    int spaces = w - (j % w);

                    for (int k = 0; k < spaces; k++) {
                        level[j + k] = BOARD_FREE;
                        j++;
                    }

                    // j has been incremented one too many times
                    j--;
                } else {
                    // Don't add newline character
                    j--;
                }
                continue;
            }

            // @ marks the start of Sokoban
            if (c == '@') {
                if (soko_x != -1 || soko_y != -1) {
                    fprintf(stderr,
                            "Sokoban position has already been set in this level!\n"
                            "Overwriting old Sokoban position (%d,%d) with (%d,%d)\n",
                            soko_x, soko_y, j % w, j / w);
                }
                soko_x = j % w;
                soko_y = j / w;
                level[j] = BOARD_FREE;
                continue;
            }

            // + marks Sokoban standing on a target
            if (c == '+') {
                if (soko_x != -1 || soko_y != -1) {
                    fprintf(stderr,
                            "Sokoban position has already been set in this level!\n"
                            "Overwriting old Sokoban position (%d,%d) with (%d,%d)\n",
                            soko_x, soko_y, j % w, j / w);
                }
                level[j] = BOARD_TARGET;
                continue;
            }

            // * marks a box on a target
            if (c == '*') {
                level[j] = BOARD_BOXTARGET;
                continue;
            }

            // # marks a wall
            if (c == '#') {
                level[j] = BOARD_WALL;
                continue;
            }

            // . marks a target
            if (c == '.') {
                level[j] = BOARD_TARGET;
                continue;
            }

            // $ marks a box
            if (c == '$') {
                level[j] = BOARD_BOX;
                continue;
            }

            // A space marks a free field
            if (c == ' ') {
                level[j] = BOARD_FREE;
                continue;
            }

            fprintf(stderr, "Found illegal field '%c' in level!\n"
                    "Assuming as wall\n", c);
            level[j] = BOARD_WALL;
        }
    }

    //printf("Loaded level:\n");
    //for (int y = 0; y < h; y++) {
    //    for (int x = 0; x < w; x++) {
    //        printf("%c", level[y * w + x]);
    //    }
    //    printf("\n");
    //}
    //printf("\n");


    // Apply loaded level
    if (BOARD != NULL) {
        free(BOARD);
    }
    BOARD = level;
    BOARD_WIDTH  = w;
    BOARD_HEIGHT = h;
    SOKO_X = soko_x;
    SOKO_Y = soko_y;

    // After loading level reinitialize the scaler
    initialize_scaler();

    return true;
}

/**
 * @brief Loads a level from a give file
 *
 * @param filepath The path to the level file
 *
 * @return True if loading was successful, false otherwise
 */
bool load_level_from_file(const char *filepath) {
    char *buf;
    long len;

    // Read file
    {
        FILE *f = fopen(filepath, "r");
        if (f == NULL) {
            fprintf(stderr, "Could not open level file '%s'\n",
                    filepath);
            return false;
        }

        fseek(f, 0, SEEK_END);
        len = ftell(f);
        fseek(f, 0, SEEK_SET);
        buf = malloc(len);

        if (buf == NULL) {
            fprintf(stderr, "Could not allocate memory for file buffer\n");
            fclose(f);
            free(buf);
            return false;
        }

        fread(buf, 1, len, f);
        fclose(f);
    }

    bool ret = load_level_from_buffer(buf, len);
    free(buf);
    return ret;
}

/**
 * @brief Loads a level from a buffer
 *
 * @param buf The buffer containing the level
 * @param len The length of the given buffer
 *
 * @return True if loading was successful, false otherwise
 */
bool load_level_from_buffer(const char *buf, long len) {
    // TODO: Stick to correct Sok level format:
    // http://www.sokobano.de/wiki/index.php?title=Sok_format
    // http://www.sokobano.de/wiki/index.php?title=Level_format

    char *buf2;
    long len2;
    // Extract level (Skip first line)
    {
        // Check how many characters are in first line
        len2 = len;
        for (int i = 0; i < len; i++) {
            len2--;
            if (buf[i] == '\n') {
                break;
            }
        }

        // Create new buffer and copy relevant bytes from old buffer
        buf2 = malloc(len2);
        if (buf2 == NULL) {
            fprintf(stderr, "Could not allocate memory for file buffer\n");
            free(buf2);
            return false;
        }
        {
            int i, j;
            for (i = len - len2, j = 0; i < len; i++, j++) {
                buf2[j] = buf[i];
            }
        }
    }

    bool ret = parse_level(buf2, len2);
    free(buf2);
    return ret;
}

