
#include "Soko.h"

/**
 * @brief Types for moves of Sokoban
 */
enum SokoMoveType {
    /**
     * Move is not possible.
     */
    MOVE_TYPE_IMPOSSIBLE,

    /**
     * Move is a normal possible move.
     */
    MOVE_TYPE_ALONE,

    /**
     * Move is possible and is a move which also moves a Box.
     */
    MOVE_TYPE_WITHBOX
};

/**
 * @brief Direction Sokoban is currently facing
 */
static enum Direction SOKO_FACING = FACING_LEFT;

/**
 * @brief Current x position of Sokoban
 */
uint8_t SOKO_X;

/**
 * @brief Current y position of Sokoban
 */
uint8_t SOKO_Y;


/**
 * @brief Checks if and how Sokoban can move in a given direction.
 *
 * Gets a direction and checks if Sokoban can move in the given direction.
 * If there is a box in the given direction it is checked whether this
 * box can move.
 *
 * @param dx The value to move Sokoban in x direction
 * @param dy The value to move Sokoban in y direction
 *
 * @return The type of move that Sokoban can (can't) do in the
 *         given direction.
 */
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

/**
 * @brief Moves a box in a given direction.
 *
 * Moves the box in a given direction.
 *
 * @param dx The value to move a Box in x direction
 * @param dy The value to move a Box in y direction
 */
static void move_box(int dx, int dy) {
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

/**
 * @brief Moves Sokoban in a given direction if possible.
 *
 * Moves Sokoban in a given direction if this move is possible.
 * If this move requires the move of a box this box is moved too.
 *
 * @param dx The value to Sokoban in x direction
 * @param dy The value to Sokoban in y direction
 */
void move_soko(int dx, int dy) {
    // Save direction Sokoban is facing
    if (dx > 0) {
        SOKO_FACING = FACING_RIGHT;
    } else if (dx < 0) {
        SOKO_FACING = FACING_LEFT;
    } else if (dy > 0) {
        // Dont modify when moving down
        SOKO_FACING = FACING_DOWN;
    } else if (dy < 0) {
        // Dont modify when moving up
        SOKO_FACING = FACING_UP;
    }

    // Check which type of move to do
    enum SokoMoveType type = check_move(dx, dy);

    // Check if move is possible at all
    if (type == MOVE_TYPE_IMPOSSIBLE) {
        return;
    }

    // Move the box first if necessary
    if (type == MOVE_TYPE_WITHBOX) {
        move_box(dx, dy);
    }

    // Set new sokoban coordinates
    SOKO_X += dx;
    SOKO_Y += dy;
}

/**
 * @brief Shows Sokoban at the current position.
 *
 * Draws Sokoban at the current position onto the screen.
**/
bool draw_soko() {
    return render_with_direction(IMG_PLAYER, SOKO_X, SOKO_Y, SOKO_FACING);
}

