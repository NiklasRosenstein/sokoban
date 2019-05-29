
#include "image.h"

static size_t b64_decoded_size(const char *b64);
static inline bool b64_is_valid_char(char c);

static int IMAGES_COUNT = 6;

static SDL_Surface **IMAGES_SDL[] = {
    &IMG_BOX,
    &IMG_BOXTARGET,
    &IMG_FREE,
    &IMG_PLAYER,
    &IMG_TARGET,
    &IMG_WALL
};

static char **IMAGES[] = {
    &b64image_box,
    &b64image_boxtarget,
    &b64image_free,
    &b64image_player,
    &b64image_target,
    &b64image_wall
};

static size_t *IMAGES_LENGTH[] = {
    &b64image_box_length,
    &b64image_boxtarget_length,
    &b64image_free_length,
    &b64image_player_length,
    &b64image_target_length,
    &b64image_wall_length
};

void deinitialize_images() {
    for (int i = 0; i < IMAGES_COUNT; i++) {
        free(*(IMAGES[i]));

        *(IMAGES_LENGTH[i]) = 0;
    }
}

bool initialize_images() {
    for (int i = 0; i < IMAGES_COUNT; i++) {
        // Skip already initialized images
        if (*(IMAGES_LENGTH[i]) != 0) {
            fprintf(stderr, "Skip decoding of image - image is already decoded\n");
            continue;
        }

        // Decode image from Base64
        char *bin;
        size_t len;
        if (!b64_decode(&bin, &len, *(IMAGES[i]))) {
            fprintf(stderr, "Error when decoding images\n");
            return false;
        }

        *(IMAGES[i]) = bin;
        *(IMAGES_LENGTH[i]) = len;


        // Create SDL Surface from binary
        loadIMG_from_buffer((IMAGES_SDL[i]), *(IMAGES[i]), *(IMAGES_LENGTH[i]));
    }

    return true;
}

bool b64_decode(char **out, size_t *out_len, const char* b64) {
    //TODO: Create this on the fly
    int b64invs[800] = { 62, -1, -1, -1, 63, 52, 53, 54, 55, 56, 57, 58,
                    59, 60, 61, -1, -1, -1, -1, -1, -1, -1,  0,  1,
                     2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13,
                    14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
                    -1, -1, -1, -1, -1, -1, 26, 27, 28, 29, 30, 31,
                    32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43,
                    44, 45, 46, 47, 48, 49, 50, 51 };

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

    // Check all Base64 characters
    for (size_t i = 0; i< b64_len; i++) {
        if (!b64_is_valid_char(b64[i])) {
            fprintf(stderr,
                    "Data does not seem to be valid Base64 string "
                    "(read invalid char '%c' at position %zu/%zu)\n",
                    b64[i], i, b64_len);
            *out = NULL;
            *out_len = 0;
            return false;
        }
    }

    // Allocate memory
    *out = malloc(*out_len + 1);    //TODO

    // Convert
    {
        size_t i, j;
        for (i = 0, j = 0; i < b64_len; i += 4, j += 3) {
            // Extract 4 Base64 Byte
            char b1 = b64[i+0];
            char b2 = b64[i+1];
            char b3 = b64[i+2];
            char b4 = b64[i+3];
            //printf("Extracted 0x%02x%02x%02x%02x\n", b1,b2,b3,b4);
            //printf("             %c %c %c %c\n", b1,b2,b3,b4);

            // Extract 4 Base64 Byte and decode each Byte to 6 Bit
            uint32_t v = 0;
            v =                                           b64invs[b1-43];
            v =                                (v << 6) | b64invs[b2-43];
            v = (b64[i+2] == '=') ? (v << 6) : (v << 6) | b64invs[b3-43];
            v = (b64[i+3] == '=') ? (v << 6) : (v << 6) | b64invs[b4-43];
            //printf("Decoded   0x%08x\n", v);

            (*out)[j] = (v >> 16) & 0xFF;
            if (b64[i+2] != '=') {
                (*out)[j+1] = (v >> 8) & 0xFF;
            }
            if (b64[i+3] != '=') {
                (*out)[j+2] = v & 0xFF;
            }
        }
    }

    return true;

}

// TODO: Remove this function
static inline bool b64_is_valid_char(char c) {
    if (c >= '0' && c <= '9')
        return true;
    if (c >= 'A' && c <= 'Z')
        return true;
    if (c >= 'a' && c <= 'z')
        return true;
    if (c == '+' || c == '/' || c == '=')
        return true;
    return false;
}

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

