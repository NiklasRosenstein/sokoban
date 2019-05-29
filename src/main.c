
#include "main.h"

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//enum KeyPress {
//    KEY_PRESS_DEFAULT,
//    KEY_PRESS_UP,
//    KEY_PRESS_DOWN,
//    KEY_PRESS_LEFT,
//    KEY_PRESS_RIGHT,
//    KEY_PRESS_TOTAL,
//};

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
        fprintf(stderr, "Window could not be created! SDL_Error: '%s'\n", SDL_GetError());
        return false;
    }

    // Get window surface
    WINSURFACE = SDL_GetWindowSurface(WINDOW);
    if (WINSURFACE == NULL) {
        fprintf(stderr, "Could not get surface of window! SDL_Error: '%s'\n", SDL_GetError());
        return false;
    }

    // Fill the surface with color
    SDL_FillRect(WINSURFACE, NULL, SDL_MapRGB(WINSURFACE->format, 0x00, 0xFF, 0x00));
    SDL_UpdateWindowSurface(WINDOW);

    // Initialize and decode images
    initialize_images();

    return true;
}

void deinitialize() {
    SDL_FreeSurface(WINSURFACE);
    SDL_DestroyWindow(WINDOW);

    // Quit SDL subsystems
    SDL_Quit();

    deinitialize_images();

    // Clean up dynamicall loaded SDL_Image libraries
    //IMG_Quit();
}

void do_command(SDL_Keycode key) {
    int x = 0;
    int y = 0;

    switch (key) {
        case SDLK_UP:
            y--;
            break;
        case SDLK_DOWN:
            y++;
            break;
        case SDLK_LEFT:
            x--;
            break;
        case SDLK_RIGHT:
            x++;
            break;
        default:
            return;
            break;
    }

    move_soko(x, y);
}

int main(int argc, char* argv[]) {
    // TODO: This prevents Wunused-parameter
    if (argc == 0 || argv == 0) {
        //
    }

    // Initialize SDL and other basic functionality
    if (initialize() == false) {
        return false;
    }


    // Paint image to surface
    //SDL_BlitSurface(IMG_PLAYER, NULL, WINSURFACE, NULL);
    //SDL_UpdateWindowSurface(WINDOW);

    show_board(WINDOW, WINSURFACE);
    show_soko();


    //Wait two seconds
    //SDL_Delay(200);


    bool quit = false;
    SDL_Event e;
    while (!quit) {
        // Process event queue
        while(SDL_PollEvent(&e) != 0) {
            // Check which type of event happened
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        quit = true;
                        break;
                    default:
                        do_command(e.key.keysym.sym);

                        if (board_targets_left() == 0) {
                            printf("Game cleared\n");
                            quit = true;
                        }
                        break;
                }

            }
        }

    }

    // Destroy window
    //SDL_FreeSurface(img_box);
    //SDL_FreeSurface(img_free);
    //SDL_FreeSurface(img_player);
    //SDL_FreeSurface(img_target);
    //SDL_FreeSurface(img_wall);

    deinitialize();


    return 0;
}

