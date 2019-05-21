#include "main.h"

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;


bool initialize(SDL_Window** window) {
    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    }


    *window = NULL;
    *window = SDL_CreateWindow("SDL Tutorial",
                            SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED,
                            SCREEN_WIDTH, SCREEN_HEIGHT,
                            SDL_WINDOW_SHOWN);
    if (*window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

bool loadIMG(SDL_Surface** image) {
    const char* filename = "resources/wall.png";

    *image = NULL;
    //*image = SDL_LoadBMP("image.bmp");
    //*image = IMG_Load(filename);
    if (*image == NULL) {
        printf("Could not load image %s, SDL_Error: %s\n", filename, SDL_GetError());
        return false;
    }
    
    return true;
}

bool loadIMG2(SDL_Surface** image) {
    const char* filename = "resources/wall.png";

    char* buffer = 0;
    long length;
    FILE* f = fopen(filename, "rb");

    // Read file into buffer
    if (!f) {
        printf("Could not load file\n");
        return false;
    }
    fseek(f, 0, SEEK_END);
    length = ftell(f);
    fseek(f, 0, SEEK_SET);
    buffer = malloc(length);
    if (!buffer) {
        printf("Could not allocate memory\n");
        fclose(f);
        free(buffer);
        return false;
    }
    fread(buffer, 1, length, f);
    fclose (f);


    // Copy buffer into SDL_Surface and free original buffer
    SDL_RWops *rw = SDL_RWFromMem(buffer, length);
    SDL_Surface *temp = IMG_Load_RW(rw, 1);
    free(buffer);
    if (temp == NULL) {
        printf("IMG_Load_RW: %s\n", IMG_GetError());
        return 1;
    }

    *image = temp;
    
    // Convert the image to optimal display format
    //SDL_Surface *myimage;
    //image = SDL_DisplayFormat(temp);
    
    //Free the temporary surface
    //SDL_FreeSurface(temp);
    
    return true;
}

int main(int argc, char* argv[]) {
    // TODO: This prevents Wunused-parameter
    if (argc == 0 || argv == 0) {
        
    }

    SDL_Window* window = NULL;

    // Initialize SDL and get window
    if (initialize(&window) == false) {
        return false;
    }
 
    // Get window surface
    SDL_Surface* windowSurface = SDL_GetWindowSurface(window);
    
    // Fill the surface white
    SDL_FillRect(windowSurface, NULL, SDL_MapRGB(windowSurface->format, 0xFF, 0xFF, 0xFF));
    SDL_UpdateWindowSurface(window);

    SDL_Surface* image = NULL;

    // Load image
    loadIMG2(&image);
    if (image == NULL) {
        printf("error!\n");
    }

    // Paint image to surface
    SDL_BlitSurface(image, NULL, windowSurface, NULL);
    SDL_UpdateWindowSurface(window);
    
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
    
    //Destroy window
    SDL_FreeSurface(image);
    SDL_FreeSurface(windowSurface);
    SDL_DestroyWindow(window);
    
    //Quit SDL subsystems
    SDL_Quit();
    
    return 0;
}

