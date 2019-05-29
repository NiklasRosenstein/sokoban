
#include "soko.h"

int SOKO_X = 1;
int SOKO_Y = 1;

enum SokoMoveType {
    MOVE_TYPE_IMPOSSIBLE,
    MOVE_TYPE_ALONE,
    MOVE_TYPE_WITHBOX
};

static enum SokoMoveType check_move(int dx, int dy) {
    char field = board_get(SOKO_X + dx, SOKO_Y + dy);

    // If target field is a wall sokoban can not move
    if (field == BOARD_WALL) {
        return MOVE_TYPE_IMPOSSIBLE;
    }

    // If target field is free or a target sokoban can move
    if (field == BOARD_TARGET || field == BOARD_FREE) {
        return MOVE_TYPE_ALONE;
    }

    // If target field is a box or a box on a target
    // we need to check if the box can move
    if (field == BOARD_BOX || field == BOARD_BOXTARGET) {
        enum SokoMoveType box_field = check_move(2 * dx, 2 * dy);

        if (box_field == MOVE_TYPE_ALONE) {
            return MOVE_TYPE_WITHBOX;
        } else {
            return MOVE_TYPE_IMPOSSIBLE;
        }
    }

    // This should not happen (unknown field)
    return MOVE_TYPE_IMPOSSIBLE;
}

void move_box(int dx, int dy) {
    // Sokoban moves from (x,y) to (x+dx,y+dy)
    // -> Box moves from (x+dx,y+dy) to (x+dx*2,y+dy*2)
    int x = SOKO_X + dx;
    int y = SOKO_Y + dy;

    int target_x = x + dx;
    int target_y = y + dy;

    // Set old field
    if (board_get(x, y) == BOARD_BOXTARGET) {
        board_set(x, y, BOARD_TARGET);
    } else {
        board_set(x, y, BOARD_FREE);
    }

    // Set new field
    if (board_get(target_x, target_y) == BOARD_TARGET) {
        board_set(target_x, target_y, BOARD_BOXTARGET);
    } else {
        board_set(target_x, target_y, BOARD_BOX);
    }
}

void move_soko(int dx, int dy) {
    // Check which type of move to do
    enum SokoMoveType type = check_move(dx, dy);

    if (type == MOVE_TYPE_IMPOSSIBLE) {
        return;
    }

    // Move the box first if necessary
    if (type == MOVE_TYPE_WITHBOX) {
        move_box(dx, dy);
    }

    // Clear the old field
    show_field(SOKO_X, SOKO_Y);

    // Show sokoban on the new field
    SOKO_X += dx;
    SOKO_Y += dy;
    show_soko();
}

void show_soko() {
    // Draw
    SDL_Rect rect;
    rect.x = SOKO_X * 32;
    rect.y = SOKO_Y * 32;
    SDL_BlitSurface(IMG_PLAYER, NULL, WINSURFACE, &rect);

    // Update the surface
    SDL_UpdateWindowSurface(WINDOW);
}

