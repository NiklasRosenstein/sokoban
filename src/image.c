
#include "image.h"

char* b64_to_binary(const char** b64_buffer) {
    size_t len = strlen(*b64_buffer);
    char *binary = malloc(len + 1);
    binary[len] = '\0';
    return binary;
}
