
#include "View.h"

//Screen dimension constants
static unsigned int SCREEN_WIDTH  = 660;
static unsigned int SCREEN_HEIGHT = 660;

static const uint8_t GRID_SIZE = 32;

static bool WINDOW_MINIMIZED = false;

static float SCALE = 2;

static unsigned int ORIGIN_X;
static unsigned int ORIGIN_Y;

/**
 * @brief The main window.
 */
SDL_Window *WINDOW;

/**
 * @brief Renderer of the main window.
 */
SDL_Renderer *WINRENDERER;

/**
 * @brief Initialize everything.
 *
 * Initialize SDL and create all necessary images.
 */
bool initialize() {
    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL could not be initialized! SDL_Error: '%s'\n", SDL_GetError());
        return false;
    }

    // Initialize SDL_Image (only png part)
    {
        int init = IMG_Init(IMG_INIT_PNG);
        if ((init & IMG_INIT_PNG) != IMG_INIT_PNG) {
            fprintf(stderr, "Could not initialize SDL_Image with PNG support\n");
            SDL_Quit();
            return false;
        }
    }

    // Create window
    WINDOW = NULL;
    WINDOW = SDL_CreateWindow("Sokoban",
                            SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED,
                            SCREEN_WIDTH, SCREEN_HEIGHT,
                            SDL_WINDOW_SHOWN |
                            SDL_WINDOW_RESIZABLE);
    if (WINDOW == NULL) {
        fprintf(stderr, "Could not create Window!\n"
                "SDL_Error: '%s'\n", SDL_GetError());
        IMG_Quit();
        SDL_Quit();
        return false;
    }

    // Create renderer for window
    WINRENDERER = SDL_CreateRenderer(
                    WINDOW, -1,
                    SDL_RENDERER_ACCELERATED |
                    SDL_RENDERER_PRESENTVSYNC);
    if (WINRENDERER == NULL) {
        fprintf(stderr, "Could not create renderer for window!\n"
                "SDL_Error: '%s'\n", SDL_GetError());
        SDL_DestroyWindow(WINDOW);
        IMG_Quit();
        SDL_Quit();
        return false;
    }


    // Initialize and decode images
    if(!initialize_images()) {
        SDL_DestroyRenderer(WINRENDERER);
        SDL_DestroyWindow(WINDOW);
        IMG_Quit();
        SDL_Quit();
        return false;
    }

    return true;
}

void initialize_scaler() {
    // Set window minimized state
    if (SDL_GetWindowFlags(WINDOW) & SDL_WINDOW_MINIMIZED) {
        WINDOW_MINIMIZED = true;
    } else {
        WINDOW_MINIMIZED = false;
    }

    // Set correct window size
    SDL_GetWindowSize(WINDOW, (int *)&SCREEN_WIDTH, (int *)&SCREEN_HEIGHT);

    // Calculate scale size
    {
        // Board width and height have both to fit after scaling
        float scale1 = SCREEN_WIDTH  / (BOARD_WIDTH  * (float)GRID_SIZE);
        float scale2 = SCREEN_HEIGHT / (BOARD_HEIGHT * (float)GRID_SIZE);
        SCALE = (scale1 > scale2) ? scale2 : scale1;

        // Only adjust scale in steps of 0.5 so distortion does not get too bad
        int r = 0;
        while (SCALE >= 0.5) {
            SCALE -= 0.5;
            r++;
        }
        SCALE = 0.5f * r;

        // Dont scale to zero
        if (SCALE < 0.5) {
            SCALE = 0.5;
        }
    }

    // Calculate origin
    ORIGIN_X = SCREEN_WIDTH  - (BOARD_WIDTH  * GRID_SIZE * SCALE);
    ORIGIN_X /= 2;
    ORIGIN_Y = SCREEN_HEIGHT - (BOARD_HEIGHT * GRID_SIZE * SCALE);
    ORIGIN_Y /= 2;
}

/**
 * @brief Deinitializes everything
 *
 * Initializes SDL and decodes images.
 */
void deinitialize() {
    deinitialize_board();
    deinitialize_images();

    SDL_DestroyRenderer(WINRENDERER);
    SDL_DestroyWindow(WINDOW);

    // Clean up dynamically loaded SDL_Image libraries
    // Deinitialize SDL_Image
    IMG_Quit();

    // Deinitialize SDL
    SDL_Quit();
}

/**
 * @brief Render the current game status.
 *
 * This calls other functions in order to fill the render buffer
 * with the correct textures and then flips the back buffer.
 *
 * @return True on success. False otherwise.
 */
bool render() {
    // Dont render when the window is minimized
    if (WINDOW_MINIMIZED) {
        return true;
    }

    // Initialize buffer
    SDL_SetRenderDrawColor(WINRENDERER, 0x00, 0x00, 0x00, 0xFF);
    if (SDL_RenderClear(WINRENDERER) != 0) {
        fprintf(stderr, "Could not clear window!\n"
                "SDL_Error: '%s'\n", SDL_GetError());
        return false;
    }

    if (MENU_SHOWN) {
        // Render menu if requested
        show_menu();
    } else {
        // Render game

        // Draw the board
        draw_board();

        // Draw sokoban
        draw_soko();
    }

    // Flip buffers
    SDL_RenderPresent(WINRENDERER);

    return true;
}

bool render_animated(const SDL_Texture *text, int x, int y, uint16_t frame_time, uint8_t frames, bool overlay) {
    int frame = (GAME_TIME / frame_time) % frames;

    // If this is an overlay animation first draw the base frame
    // then draw animation on top
    if (overlay && frame != 0) {
        render_static(text, x, y);
    }

    // Select correct animation frame
    SDL_Rect clip;
    clip.x = frame * GRID_SIZE;
    clip.y = 0;
    clip.w = GRID_SIZE;
    clip.h = GRID_SIZE;

    // Set viewport to field
    SDL_Rect view;
    view.x = ORIGIN_X + (x * GRID_SIZE * SCALE);
    view.y = ORIGIN_Y + (y * GRID_SIZE * SCALE);
    view.w = GRID_SIZE * SCALE;
    view.h = GRID_SIZE * SCALE;

    if (SDL_RenderSetViewport(WINRENDERER, &view) != 0) {
        fprintf(stderr, "Could not set viewport for rendering!\n"
                "SDL_Error: '%s'\n", SDL_GetError());
        return false;
    }

    int render_success = SDL_RenderCopy(WINRENDERER, (SDL_Texture *)text,
                                        &clip, NULL);
    if (render_success != 0) {
        fprintf(stderr, "Could not render texture!\n"
                "SDL_Error: '%s'\n", SDL_GetError());
        return false;
    }

    return true;
}

bool render_with_direction(const SDL_Texture *text, int x, int y, enum Direction direction) {
    // Dont clip image by default
    SDL_Rect clip;
    clip.x = 0;
    clip.y = 0;
    clip.w = GRID_SIZE;
    clip.h = GRID_SIZE;

    // Dont flip image by default
    SDL_RendererFlip flip = SDL_FLIP_NONE;

    // Depending on direction use different frame or flip image
    switch (direction) {
        case FACING_RIGHT:
            break;
        case FACING_LEFT:
            flip = SDL_FLIP_HORIZONTAL;
            break;
        case FACING_DOWN:
            clip.y = GRID_SIZE;
            break;
        case FACING_UP:
            clip.y = 2 * GRID_SIZE;
            break;
    }

    // Set viewport to field
    SDL_Rect view;
    view.x = ORIGIN_X + (x * GRID_SIZE * SCALE);
    view.y = ORIGIN_Y + (y * GRID_SIZE * SCALE);
    view.w = GRID_SIZE * SCALE;
    view.h = GRID_SIZE * SCALE;

    if (SDL_RenderSetViewport(WINRENDERER, &view) != 0) {
        fprintf(stderr, "Could not set viewport for rendering!\n"
                "SDL_Error: '%s'\n", SDL_GetError());
        return false;
    }

    // Render
    int render_success; // = SDL_RenderCopyEx(WINRENDERER, (SDL_Texture *)text,
                         //                 &clip, NULL, angle, NULL, flip);
    if (flip == SDL_FLIP_NONE) {
        render_success = SDL_RenderCopy(WINRENDERER, (SDL_Texture *)text,
                                        &clip, NULL);
    } else {
        render_success = SDL_RenderCopyEx(WINRENDERER, (SDL_Texture *)text,
                                          &clip, NULL,
                                          0, NULL, flip);
    }
    if (render_success != 0) {
        fprintf(stderr, "Could not render texture!\n"
                "SDL_Error: '%s'\n", SDL_GetError());
        return false;
    }

    return true;

}

bool render_with_variation(const SDL_Texture *text, int x, int y, int variations, int commonness) {
    unsigned int i = y * BOARD_WIDTH + x;
    uint8_t frame = 0;

    // Check if texture should be varified
    if (i % commonness == 0) {
        // Select the correct frame
        unsigned int j = i / commonness;
        frame = j % variations;

        // First render the base frame, then render variation on top
        render_static(text, x, y);
    }

    // Select correct frame
    SDL_Rect clip;
    clip.x = frame * GRID_SIZE;
    clip.y = 0;
    clip.w = GRID_SIZE;
    clip.h = GRID_SIZE;

    // Set viewport to field
    SDL_Rect view;
    view.x = ORIGIN_X + (x * GRID_SIZE * SCALE);
    view.y = ORIGIN_Y + (y * GRID_SIZE * SCALE);
    view.w = GRID_SIZE * SCALE;
    view.h = GRID_SIZE * SCALE;

    if (SDL_RenderSetViewport(WINRENDERER, &view) != 0) {
        fprintf(stderr, "Could not set viewport for rendering!\n"
                "SDL_Error: '%s'\n", SDL_GetError());
        return false;
    }

    int render_success = SDL_RenderCopy(WINRENDERER, (SDL_Texture *)text,
                                        &clip, NULL);
    if (render_success != 0) {
        fprintf(stderr, "Could not render texture!\n"
                "SDL_Error: '%s'\n", SDL_GetError());
        return false;
    }

    return true;
}

bool render_permutated(const SDL_Texture *text, int x, int y) {
    // Dont clip image
    SDL_Rect clip;
    clip.x = 0;
    clip.y = 0;
    clip.w = GRID_SIZE;
    clip.h = GRID_SIZE;

    // Set viewport to field
    SDL_Rect view;
    view.x = ORIGIN_X + (x * GRID_SIZE * SCALE);
    view.y = ORIGIN_Y + (y * GRID_SIZE * SCALE);
    view.w = GRID_SIZE * SCALE;
    view.h = GRID_SIZE * SCALE;

    double angle;
    SDL_RendererFlip flip;

    // Calculate rotation and flipping of field depending on coordinate
    // (We have 8 different states of texture)
    int i = (y * BOARD_WIDTH + x) % 8;
    switch (i) {
        // Normal
        case 0:
            angle = 0.f;
            flip = SDL_FLIP_NONE;
            break;
        // Rotate 90
        case 1:
            angle = 90;
            flip = SDL_FLIP_NONE;
            break;
        // Rotate 180
        case 2:
            angle = 180;
            flip = SDL_FLIP_NONE;
            break;
        // Rotate 270
        case 3:
            angle = 270;
            flip = SDL_FLIP_NONE;
            break;
        // Flip horizontally
        case 4:
            angle = 0.f;
            flip = SDL_FLIP_HORIZONTAL;
            break;
        // Flip vertically
        case 5:
            angle = 0.f;
            flip = SDL_FLIP_VERTICAL;
            break;
        // Flip horizontally and rotate 90
        case 6:
            flip = SDL_FLIP_HORIZONTAL;
            angle = 90;
            break;
        // Flip vertically and rotate 90
        case 7:
            flip = SDL_FLIP_VERTICAL;
            angle = 90;
            break;
        default:
            // this should never happen
            angle = 0.f;
            flip = SDL_FLIP_NONE;
            break;
    }

    // Render
    if (SDL_RenderSetViewport(WINRENDERER, &view) != 0) {
        fprintf(stderr, "Could not set viewport for rendering!\n"
                "SDL_Error: '%s'\n", SDL_GetError());
        return false;
    }
    int render_success = SDL_RenderCopyEx(WINRENDERER, (SDL_Texture *)text,
                                          &clip, NULL, angle, NULL, flip);
    if (render_success != 0) {
        fprintf(stderr, "Could not render texture!\n"
                "SDL_Error: '%s'\n", SDL_GetError());
        return false;
    }

    return true;
}

bool render_static(const SDL_Texture *text, int x, int y) {
    // Dont clip image
    SDL_Rect clip;
    clip.x = 0;
    clip.y = 0;
    clip.w = GRID_SIZE;
    clip.h = GRID_SIZE;

    // Set viewport to field
    SDL_Rect view;
    view.x = ORIGIN_X + (x * GRID_SIZE * SCALE);
    view.y = ORIGIN_Y + (y * GRID_SIZE * SCALE);
    view.w = GRID_SIZE * SCALE;
    view.h = GRID_SIZE * SCALE;

    if (SDL_RenderSetViewport(WINRENDERER, &view) != 0) {
        fprintf(stderr, "Could not set viewport for rendering!\n"
                "SDL_Error: '%s'\n", SDL_GetError());
        return false;
    }

    int render_success = SDL_RenderCopy(WINRENDERER, (SDL_Texture *)text,
                                        &clip, NULL);
    if (render_success != 0) {
        fprintf(stderr, "Could not render texture!\n"
                "SDL_Error: '%s'\n", SDL_GetError());
        return false;
    }

    return true;
}

