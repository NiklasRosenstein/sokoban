
#include "main.h"

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;


bool initialize(SDL_Window** window) {
    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: '%s'\n", SDL_GetError());
        return false;
    }


    *window = NULL;
    *window = SDL_CreateWindow("Sokoban",
                            SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED,
                            SCREEN_WIDTH, SCREEN_HEIGHT,
                            SDL_WINDOW_SHOWN);
    if (*window == NULL) {
        fprintf(stderr, "Window could not be created! SDL_Error: '%s'\n", SDL_GetError());
        return false;
    }

    return true;
}

bool loadIMG_from_file(SDL_Surface **image, const char *filename) {
    // Cut off the file ending (last 3 characters)
    const char *ending = "";
    if (strlen(filename) > 3) {
        ending = &(filename[strlen(filename) - 3]);
    }

    // Decide depending on file ending which method to use
    *image = NULL;
    if (strcmp(ending, "bmp") == 0) {
        *image = SDL_LoadBMP(filename);
    } else {
        *image = IMG_Load(filename);
    }
    if (*image == NULL) {
        fprintf(stderr, "Could not load image '%s', SDL_Error: '%s'\n", filename, SDL_GetError());
        return false;
    }
    
    return true;
}

bool loadIMG_from_buffer(SDL_Surface **image, const char *buffer, const int length) {
    // Create SDL_RWops from buffer (this does not copy memory!)
    SDL_RWops *rw = SDL_RWFromConstMem(buffer, length);

    // Copy buffer into SDL_Surface and free SDL_RWops
    SDL_Surface *temp = IMG_Load_RW(rw, 0);
    //SDL_RWclose(rw);
    if (temp == NULL) {
        fprintf(stderr, "Could not load image from buffer\n");
        fprintf(stderr, "IMG_Load_RW: '%s'\n", IMG_GetError());
        return false;
    }

    *image = temp;
    
    return true;
}

int main(int argc, char* argv[]) {
    // TODO: This prevents Wunused-parameter
    if (argc == 0 || argv == 0) {
        
    }

    // Decode Base64 images
    initialize_images();

    // Initialize SDL and other basic functionality
    SDL_Window *win = NULL;
    if (initialize(&win) == false) {
        return false;
    }
    WINDOW = win;
 
    // Get window surface
    SDL_Surface* surface = SDL_GetWindowSurface(win);
    WINSURFACE = surface;
    
    // Fill the surface with background color
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0x00, 0xFF, 0x00));
    SDL_UpdateWindowSurface(win);

    // Load images
    //SDL_Surface *IMG_FREE = NULL;
    loadIMG_from_buffer(&IMG_FREE, b64image_free, b64image_free_length);
    if (IMG_FREE == NULL) {
        fprintf(stderr, "Could not load image\n");
    }
    //SDL_Surface *img_box = NULL;
    loadIMG_from_buffer(&IMG_BOX, b64image_box, b64image_box_length);
    if (IMG_BOX == NULL) {
        fprintf(stderr, "Could not load image\n");
    }
    //SDL_Surface *img_player = NULL;
    loadIMG_from_buffer(&IMG_PLAYER, b64image_player, b64image_player_length);
    if (IMG_PLAYER == NULL) {
        fprintf(stderr, "Could not load image\n");
    }
    //SDL_Surface *img_target = NULL;
    loadIMG_from_buffer(&IMG_TARGET, b64image_target, b64image_target_length);
    if (IMG_TARGET == NULL) {
        fprintf(stderr, "Could not load image\n");
    }
    //SDL_Surface *img_wall = NULL;
    loadIMG_from_buffer(&IMG_WALL, b64image_wall, b64image_wall_length);
    if (IMG_WALL == NULL) {
        fprintf(stderr, "Could not load image\n");
    }

    // Paint image to surface
    SDL_BlitSurface(IMG_PLAYER, NULL, surface, NULL);
    SDL_UpdateWindowSurface(win);

    draw_board(win, surface);


    //Wait two seconds
    //SDL_Delay(200);


    bool quit = false;
    SDL_Event e;
    while (!quit) {
        // Process event queue
        while(SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

    }
    
    // Destroy window
    //SDL_FreeSurface(img_box);
    //SDL_FreeSurface(img_free);
    //SDL_FreeSurface(img_player);
    //SDL_FreeSurface(img_target);
    //SDL_FreeSurface(img_wall);

    SDL_FreeSurface(surface);
    SDL_DestroyWindow(win);
    
    // Clean up dynamicall loaded SDL_Image libraries
    //IMG_Quit();

    // Quit SDL subsystems
    SDL_Quit();

    deinitialize_images();
    
    return 0;
}

