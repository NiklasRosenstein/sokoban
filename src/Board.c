
#include "Board.h"


char *BOARD = NULL;

uint8_t BOARD_WIDTH;
uint8_t BOARD_HEIGHT;

const char BOARD_FREE      = ' ';
const char BOARD_BOX       = '$';
const char BOARD_TARGET    = '.';
const char BOARD_BOXTARGET = '*';
const char BOARD_WALL      = '#';

/**
 * @brief Calculates how many targets are still left.
 *
 * Calculates how many targets are still left (that is targets
 * without a box on top of them).
 * 
 * @return The number of targets that are left.
 */
uint8_t board_targets_left() {
    // Count occurrences of targets left on board
    int counter = 0;

    int max = BOARD_HEIGHT * BOARD_WIDTH;
    for (int i = 0; i < max; i++) {
        if (BOARD[i] == BOARD_TARGET) {
            counter++;
        }
    }

    return counter;
}

/**
 * @brief Returns the value of the field at given (x, y) coordinates.
 *
 * @return The character at the coordinates.
 */
char board_get(uint8_t x, uint8_t y) {
    return BOARD[y * BOARD_WIDTH + x];
}

/**
 * @brief Sets a given field to a given value.
 *
 * @param x The x coordinate of the field to set.
 * @param y The y coordinate of the field to set.
 * @param b The value to set the field to.
 */
void board_set(uint8_t x, uint8_t y, char b) {
    BOARD[y * BOARD_WIDTH + x] = b;
}

static inline bool draw_field(const char field, int x, int y) {
    // Decide which image(s) to draw
    if (field == BOARD_BOX) {
        draw_field(BOARD_FREE, x, y);
        return render_static(IMG_BOX, x, y);
    } else if (field == BOARD_BOXTARGET) {
        draw_field(BOARD_TARGET, x, y);
        return render_animated(IMG_BOXTARGET, x, y, 200, 5, true);
    } else if (field == BOARD_FREE) {
        return render_permutated(IMG_FREE, x, y);
    } else if (field == BOARD_TARGET) {
        draw_field(BOARD_FREE, x, y);
        return render_animated(IMG_TARGET, x, y, 200, 5, false);
    } else if (field == BOARD_WALL) {
        return render_with_variation(IMG_WALL, x, y, 6, 2);
    } else {
        fprintf(stderr, "Found unknown character in board encoding"
                "'%c' (assuming as wall)\n", field);
        return draw_field(BOARD_WALL, x, y);
    }
}

/**
 * @brief Draws the complete gameboard.
 */
void draw_board() {
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            draw_field(board_get(x, y), x, y);
        }
    }
}

void deinitialize_board() {
    if (BOARD != NULL) {
        free(BOARD);
    }
    BOARD = NULL;
}

