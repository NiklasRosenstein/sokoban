
#include "Board.h"

//char BOARD[] = "WWWWWWWWWW"
//               "WFFFFFFFFW"
//               "WFFWWWWFFW"
//               "WFFWTFFFFW"
//               "WFBWFFFFFW"
//               "WFFWFBFFFW"
//               "WFFWWTWFFW"
//               "WFFFFFWFFW"
//               "WFFFFFFFFW"
//               "WWWWWWWWWW";

//char BOARD[] = "##########"
//               "#        #"
//               "#  ####  #"
//               "#  #.    #"
//               "# B#     #"
//               "#  # B   #"
//               "#  ##.#  #"
//               "#     #  #"
//               "#        #"
//               "##########";

char *BOARD = NULL;

uint8_t BOARD_HEIGHT = 10;
uint8_t BOARD_WIDTH = 10;

const char BOARD_FREE = ' ';
const char BOARD_BOX = 'B';
const char BOARD_TARGET = '.';
const char BOARD_BOXTARGET = '*';
const char BOARD_WALL = '#';

/**
 * @brief SDL_Surface of the main window.
 */
SDL_Surface *WINSURFACE;

/**
 * @brief The main window.
 */
SDL_Window *WINDOW;


/**
 * @brief Calculates how many targets are still left.
 *
 * Calculates how many targets are still left (that is targets
 * without a box on top of them).
 * 
 * @return The number of targets that are left.
 */
int board_targets_left() {
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
char board_get(int x, int y) {
    return BOARD[y * BOARD_WIDTH + x];
}

/**
 * @brief Sets a given field to a given value.
 *
 * @param x The x coordinate of the field to set.
 * @param y The y coordinate of the field to set.
 * @param b The value to set the field to.
 */
void board_set(int x, int y, char b) {
    BOARD[y * BOARD_WIDTH + x] = b;
    show_field(x, y);
}

/**
 * @brief Takes coordinates array and draws its fields.
 *
 * Takes an array of (x, y) coordinates and draws all fields at the given
 * coordinates.
 *
 * @param xy Array of (x, y) coordinates whose fields will be drawn.
 * @param length Length of the given array.
 */
void show_fields(const int xy[][2], const int length) {
    for (int i = 0; i < length; i++) {
        int x = xy[i][0];
        int y = xy[i][1];

        // Get correct field
        char field = board_get(x, y);

        // Select correct image to draw
        SDL_Surface *img;
        if (field == BOARD_BOX) {
            img = IMG_BOX;
        } else if (field == BOARD_BOXTARGET) {
            img = IMG_BOXTARGET;
        } else if (field == BOARD_FREE) {
            img = IMG_FREE;
        } else if (field == BOARD_TARGET) {
            img = IMG_TARGET;
        } else if (field == BOARD_WALL) {
            img = IMG_WALL;
        } else {
            fprintf(stderr, "Found unknown character in board encoding"
                    "'%c' (assuming as wall)\n", field);
            img = IMG_WALL;
        }

        // Draw
        SDL_Rect dst;
        dst.x = x * 32;
        dst.y = y * 32;
        dst.w = 0;          // Not used
        dst.h = 0;          // Not used
        SDL_BlitSurface(img, NULL, WINSURFACE, &dst);
    }

    // Update the surface after all bitmaps have been drawn
    SDL_UpdateWindowSurface(WINDOW);
}

/**
 * @brief Draws the field at position (x,y)
 */
void show_field(const int x, const int y) {
    const int xy[1][2] = {{x, y}};
    show_fields(xy, 1);
}

/**
 * @brief Draws the complete gameboard.
 */
void show_board() {
    int size = BOARD_HEIGHT * BOARD_WIDTH;

    // Create array of coordinates
    int xy[size][2];
    for (int i = 0; i< size; i++) {
        xy[i][0] = i % BOARD_WIDTH; // x
        xy[i][1] = i / BOARD_WIDTH; // y
    }

    show_fields(xy, size);
}

void deinitialize_board() {
    if (BOARD != NULL) {
        free(BOARD);
    }
    BOARD = NULL;
}

