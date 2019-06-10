
#include "Main.h"

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

/**
 * @brief Initialize everything.
 *
 * Initialize SDL and create all necessary images.
 */
bool initialize() {
    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: '%s'\n", SDL_GetError());
        return false;
    }

    // Create window
    WINDOW = NULL;
    WINDOW = SDL_CreateWindow("Sokoban",
                            SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED,
                            SCREEN_WIDTH, SCREEN_HEIGHT,
                            SDL_WINDOW_SHOWN);
    if (WINDOW == NULL) {
        fprintf(stderr, "Could not create Window!\n"
                "SDL_Error: '%s'\n", SDL_GetError());
        return false;
    }

    // Get window surface
    WINSURFACE = SDL_GetWindowSurface(WINDOW);
    if (WINSURFACE == NULL) {
        fprintf(stderr, "Could not get surface of window!\n"
                "SDL_Error: '%s'\n", SDL_GetError());
        return false;
    }

    // Fill the surface with color
    uint32_t color = SDL_MapRGB(WINSURFACE->format, 0x00, 0x00, 0x00);
    int ret1 = SDL_FillRect(WINSURFACE, NULL, color);
    if (ret1 != 0) {
        fprintf(stderr, "Could not fill window with color!\n"
                "SDL_Error: '%s'\n", SDL_GetError());
        return false;
    }
    int ret2 = SDL_UpdateWindowSurface(WINDOW);
    if (ret2 != 0) {
        fprintf(stderr, "Could not update sdl window surface!\n"
                "SDL_Error: '%s'\n", SDL_GetError());
    }

    // Initialize and decode images
    initialize_images();

    return true;
}

/**
 * @brief Deinitializes everything
 *
 * Initializes SDL and decodes images.
 */
void deinitialize() {
    deinitialize_board();
    deinitialize_images();

    SDL_FreeSurface(WINSURFACE);
    SDL_DestroyWindow(WINDOW);

    // Quit SDL subsystems
    SDL_Quit();

    // Clean up dynamicall loaded SDL_Image libraries
    IMG_Quit();
}

/**
 * @brief Handles key press.
 *
 * Gets a key press and tries to move Sokoban in the
 * corresponding direction.
 *
 * @param key The key to handle
 */
void do_command(SDL_Keycode key) {
    int x = 0;
    int y = 0;

    switch (key) {
        case SDLK_k:
        case SDLK_UP:
            y--;
            break;
        case SDLK_j:
        case SDLK_DOWN:
            y++;
            break;
        case SDLK_h:
        case SDLK_LEFT:
            x--;
            break;
        case SDLK_l:
        case SDLK_RIGHT:
            x++;
            break;
        default:
            return;
            break;
    }

    move_soko(x, y);
}

/**
 * @brief Prints helping information to terminal.
 *
 * @param argv0 Value in argv[0], which is the name with
 *        which this application was started.
 *
 * @param error Whether help output should be printed as an error
 *        (true -> stderr) or as normal output (false -> stdout)
 */
static void help(char *argv0, bool error) {
    FILE *out = error ? stderr : stdout;

    fprintf(out, "Usage: %s [OPTIONS]\n", argv0);
    fprintf(out, "\n");
    fprintf(out, "OPTIONS\n");
    fprintf(out, "  -f, --file FILE\n");
    fprintf(out, "      Level file to load directly. This has to be a valid .sok file\n");
    fprintf(out, "\n");
    fprintf(out, "  -h, --help\n");
    fprintf(out, "      Print this help\n");
    fprintf(out, "\n");
}

/**
 * @brief Parses arguments given to application on command line
 *
 * @param argc Number of arguments to parse
 * @param argv String array of arguments to parse
 * @param filepath Memory location to store a pointer to a filepath
 *        if one was set on the command line. Otherwise this will
 *        not be touched.
 *
 * @return False if application should not keep running, true otherwise
 */
static bool parse_arguments(int argc, char **argv, char **filepath) {
    //TODO: Implement this function!

    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];

        if (strcmp(arg, "-h")     == 0 ||
            strcmp(arg, "--help") == 0) {
            help(argv[0], false);
            return false;
        }

        if (strcmp(arg, "-f")     == 0 ||
            strcmp(arg, "--file") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Expected filename after %s option\n", argv[i]);
                return false;
            }

            *filepath = argv[i + 1];
            i++;
            continue;
        }

        // Unknown command line option
        fprintf(stderr, "Unknown option '%s'\n", arg);
        help(argv[0], true);
        return false;
    }

    return true;
}

/**
 * @brief main() function
 */
int main(int argc, char **argv) {
    // Parse command line options
    char *filepath = NULL;
    if (!parse_arguments(argc, argv, &filepath)) {
        return 0;
    }

    // Initialize SDL and other basic functionality
    if (initialize() == false) {
        return false;
    }

    // If a file path was given on the command line load the requested
    // level - otherwise load the default level.
    if (filepath != NULL) {
        //load_level_from_file("level2.sok");
        if (!load_level_from_file(filepath)) {
            fprintf(stderr, "Could not load level\n");
            return 1;
        }
    } else {
        if(!load_level_from_buffer(DEFAULT_LEVEL, strlen(DEFAULT_LEVEL))) {
            fprintf(stderr, "Could not load level\n");
            return 1;
        }
    }

    // Draw the board and Sokoban for the first time
    show_board(WINDOW, WINSURFACE);
    show_soko();


    bool quit = false;
    SDL_Event e;
    while (!quit) {
        // Process event queue (block for at most 100ms)
        if (SDL_WaitEventTimeout(&e, 100) != 0) {
            // Check which type of event happened
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_q:
                    case SDLK_ESCAPE:
                        quit = true;
                        break;
                    default:
                        do_command(e.key.keysym.sym);

                        // Check if there are still unfilled targets left
                        if (board_targets_left() == 0) {
                            printf("Game cleared\n");
                            quit = true;
                        }
                        break;
                }
            }
        }

        // TODO: Implement running game timer
        // TODO: Implement step counter
    }

    deinitialize();

    return 0;
}

