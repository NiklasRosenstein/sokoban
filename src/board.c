
#include "board.h"

const char *board = "WWWWWWWWWW"
                    "WFFFFFFFFW"
                    "WFFWWWWFFW"
                    "WFFWTFFFFW"
                    "WFBWFFFFFW"
                    "WFFWFBFFFW"
                    "WFFWWTWFFW"
                    "WFFFFFWFFW"
                    "WFFFFFFFFW"
                    "WWWWWWWWWW";

const uint8_t BOARD_HEIGHT = 10;
const uint8_t BOARD_WIDTH = 10;

const char BOARD_FREE = 'F';
const char BOARD_BOX = 'B';
const char BOARD_TARGET = 'T';
const char BOARD_BOXTARGET = 'Z';
const char BOARD_WALL = 'W';

SDL_Surface *WINSURFACE;
SDL_Window *WINDOW;


static inline char board_get(int x, int y);


int board_targets_left() {
    // Count occurrences of targets left on board
    int counter = 0;
    for (int i = 0; i < BOARD_HEIGHT * BOARD_WIDTH; i++) {
        if (board[i] == BOARD_TARGET) {
            counter++;
        }
    }

    return counter;
}

/**
 * Returns the field character at given (x,y) coordinates.
 * @return The character at the coordinates.
 */
static inline char board_get(int x, int y) {
    return board[y * BOARD_WIDTH + x];
}

/**
 * Takes an array of (x,y) coordinates and draws all fields
 * at the given coordinates.
 */
void draw_fields(const int xy[][2], const int length) {
    printf("draw_fields() - Drawing %d fields ...", length);
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
        SDL_Rect rect;
        rect.x = x * 32;
        rect.y = y * 32;
        SDL_BlitSurface(img, NULL, WINSURFACE, &rect);
    }
    printf("done\n");

    // Update the surface after all bitmaps have been drawn
    SDL_UpdateWindowSurface(WINDOW);
}

/**
 * Draws the field at position (x,y)
 */
void draw_field(const int x, const int y) {
    const int xy[1][2] = {{x, y}};
    draw_fields(xy, 1);
}

/**
 * Draws the complete gameboard.
 */
void draw_board() {
    int size = BOARD_HEIGHT * BOARD_WIDTH;
    printf("draw_board() - drawing board with size %d\n", size);

    // Create array of coordinates
    int xy[size][2];
    for (int i = 0; i< size; i++) {
        xy[i][0] = i % BOARD_WIDTH; // x
        xy[i][1] = i / BOARD_WIDTH; // y
    }

    draw_fields(xy, size);
}

bool load_level() {
    // Level Example:
    //   ####
    // ###  ####
    // #     $ #
    // # #  #$ #
    // # . .#@ #
    // #########
    //
    // Level Example:
    // ####
    // # .#
    // #  ###
    // #*@  #
    // #  $ #
    // #  ###
    // ####

    // TODO:
    // Load Level
    //
    // - Calculate width and height
    // - Fill gaps (?)
    // - Replace Player (@) with free field ( )
    // - Set Player position

}
