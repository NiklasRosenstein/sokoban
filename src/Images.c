
#include "Images.h"

/**
 * @brief Number of different images in this game.
 */
static int IMAGES_COUNT = 6;

/**
 * @brief Internal array of pointers of SDL_Surfaces of images
 */
static SDL_Texture **IMAGES_SDL[] = {
    &IMG_BOX,
    &IMG_BOXTARGET,
    &IMG_FREE,
    &IMG_PLAYER,
    &IMG_TARGET,
    &IMG_WALL
};

/**
 * @brief Internal array of pointers to b64 encoded images
 */
static const char **IMAGES[] = {
    &b64image_box,
    &b64image_boxtarget,
    &b64image_free,
    &b64image_player,
    &b64image_target,
    &b64image_wall
};

static size_t b64_decoded_size(const char *b64) {
    if (b64 == NULL) {
        return 0;
    }
    // Every Base64 string has to be divisible by 4
    if (strlen(b64) % 4 != 0) {
        fprintf(stderr, "String does not seem to be valid Base64 data\n");
        return 0;
    }

    // Base64 encodes 3 binary Byte as 4 Base64 Byte
    size_t b64_len = strlen(b64);
    size_t bin_len = b64_len / 4 * 3;

    // Remove '=' padded Byte
    for (size_t i = b64_len - 1; i > 0; i--) {
        if (b64[i] == '=') {
            bin_len--;
        } else {
            break;
        }
    }

    return bin_len;
}

/**
 * @brief Decodes a Base64 encoded image to raw binary bytes.
 *
 * @param out Pointer to a location where output data will be stored.
 *        This function will malloc the required size. This will need
 *        to be freed eventually.
 * @param out_len Pointer to a location where the length of the output
 *        data will be stored.
 * @param b64 The Base64 encoded input image for this function.
 *
 * @return True if decoding was successful, false otherwise.
 */
static bool b64_decode(char **out, size_t *out_len, const char *b64) {
    // Invert Base64 encryption matrix
    const char b64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    char b64inv[80];

    // Fill decoding matrix with -1 because not all elements will be set.
    // This is not really necessary but hopefully makes bugs reproducable.
    memset(b64inv, -1, sizeof(b64inv));

    for (unsigned long i = 0; i < sizeof(b64chars) - 1; i++) {
        b64inv[b64chars[i] - 43] = i;
    }

    if (b64 == NULL) {
        fprintf(stderr, "Data does not seem to be valid Base64 string ");
        fprintf(stderr, "(data is null)\n");
        *out = NULL;
        *out_len = 0;
        return false;
    }

    // Calculate length of Base64 string and output binary
    size_t b64_len = strlen(b64);
    *out_len = b64_decoded_size(b64);

    // Every Base64 string has to be divisible by 4
    if (b64_len % 4 != 0) {
        fprintf(stderr, "Data does not seem to be valid Base64 string ");
        fprintf(stderr, "(data not divisable by 4)\n");
        *out = NULL;
        *out_len = 0;
        return false;
    }

    // Check if all Base64 characters
    for (size_t i = 0; i< b64_len; i++) {
        unsigned char c = b64[i];

        if (c >= '0' && c <= '9') {
            continue;
        } else if (c >= 'A' && c <= 'Z') {
            continue;
        } else if (c >= 'a' && c <= 'z') {
            continue;
        } else if (c == '+' || c == '/' || c == '=') {
            continue;
        }

        // Character is not a valid Base64 character
        fprintf(stderr,
                "Data does not seem to be valid Base64 string "
                "(read invalid char '%c' at position %zu/%zu)\n",
                b64[i], i, b64_len);
        *out = NULL;
        *out_len = 0;
        return false;
    }

    // Allocate memory
    *out = malloc(*out_len + 1);

    // Decode Base64 string
    {
        size_t i, j;
        for (i = 0, j = 0; i < b64_len; i += 4, j += 3) {
            // Extract 4 Base64 Byte
            const char b1 = b64[i];
            const char b2 = b64[i + 1];
            const char b3 = b64[i + 2];
            const char b4 = b64[i + 3];

            // Decode each Byte to 6 Bit
            uint32_t d = 0;
            d =                                             b64inv[b1 - 43];
            d =                                  (d << 6) | b64inv[b2 - 43];
            d = (b64[i + 2] == '=') ? (d << 6) : (d << 6) | b64inv[b3 - 43];
            d = (b64[i + 3] == '=') ? (d << 6) : (d << 6) | b64inv[b4 - 43];

            // Save 3 decoded Byte (except if they are padding)
            (*out)[j] = (d >> 16) & 0xFF;
            if (b3 != '=') {
                (*out)[j + 1] = (d >> 8) & 0xFF;
            }
            if (b4 != '=') {
                (*out)[j + 2] = d & 0xFF;
            }
        }
    }

    return true;
}

/**
 * @brief Creates an SDL_Surface from raw binary data.
 *
 * @param image The memory location where the SDL_Surface will be created.
 * @param buffer The raw binary image data from which the SDL_Surface will
 *        be created.
 * @param length The length of the raw binary data.
 */
static bool loadIMG_from_buffer(SDL_Surface **image, const char *buffer, const int length) {
    // Create SDL_RWops from buffer (this does not copy memory!)
    SDL_RWops *rw = SDL_RWFromConstMem(buffer, length);

    // Copy buffer into SDL_Surface and free SDL_RWops
    SDL_Surface *temp = IMG_Load_RW(rw, 0);
    SDL_RWclose(rw);
    if (temp == NULL) {
        fprintf(stderr, "Could not load image from buffer\n");
        fprintf(stderr, "IMG_Load_RW: '%s'\n", IMG_GetError());
        return false;
    }

    *image = temp;

    return true;
}

/**static bool loadIMG_from_file(SDL_Surface **image, const char *filename) {
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
}**/

/**
 * @brief Deinitialize all images
 */
void deinitialize_images() {
    // Free all textures
    for (int i = 0; i < IMAGES_COUNT; i++) {
        // Free texture if it wasnt already freed
        if (*(IMAGES_SDL[i]) != NULL) {
            SDL_DestroyTexture(*(IMAGES_SDL[i]));
            *(IMAGES_SDL[i]) = NULL;
        }
    }
}

/**
 * @brief Initialize all images
 *
 * Decodes all Base64 encoded images and creates SDL_Sufaces of them.
 */
bool initialize_images() {
    // Decode all Base64 encoded images, save decoded
    // output and create SDL_Surface from image.
    for (int i = 0; i < IMAGES_COUNT; i++) {
        // Decode image from Base64
        char *bin;
        size_t len;
        if (!b64_decode(&bin, &len, *(IMAGES[i]))) {
            fprintf(stderr, "Error when decoding images\n");
            deinitialize_images();
            return false;
        }

        // Create SDL Surface from binary and free original binary
        SDL_Surface *surf;
        if (!loadIMG_from_buffer(&surf, bin, len)) {
            fprintf(stderr, "Could not create SDL_Surface from image\n");
            free(bin);
            deinitialize_images();
            return false;
        }
        free(bin);

        // Create texture from surface and free surface
        SDL_Texture *text = SDL_CreateTextureFromSurface(WINRENDERER, surf);
        if (text == NULL) {
            fprintf(stderr, "Could not create texture from loaded SDL_Surface\n"
                    "SDL_Error: '%s'\n", SDL_GetError());
            SDL_FreeSurface(surf);
            deinitialize_images();
            return false;
        }
        SDL_FreeSurface(surf);

        // Save reference to texture
        *(IMAGES_SDL[i]) = text;
    }

    return true;
}

