
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

const char b64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int main(int argc, char *argv[]);
void help(char *name);
bool filename_to_hguard(const char *fname, char **hguard);
bool read_file(const char *fname, unsigned char **buffer, long *size);
bool write_file(const char *fname, char *content);
size_t b64_encoded_size(size_t binary_size);
char *b64_encode(const unsigned char *in, const size_t len);
bool generate_h(char **out, char *hguard, char *variable);
bool generate_c(char **out, char *b64, char *variable);

int main(int argc, char *argv[]) {
    // Read parameters
    char *in_file = NULL;
    char *cfile = NULL;
    char *hfile = NULL;
    char *variable = NULL;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-c") == 0) {
            if (i + 1 < argc) {
                cfile = argv[i + 1];
                i++;
            } else {
                printf("Expected file name after '-c' option\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-h") == 0) {
            if (i + 1 < argc) {
                hfile = argv[i + 1];
                i++;
            } else {
                printf("Expected file name after '-h' option\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-n") == 0) {
            if (i + 1 < argc) {
                variable = argv[i + 1];
                i++;
            } else {
                printf("Expected variable name after '-n' option\n");
                return 1;
            }
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            help(argv[0]);
            return 0;
        } else {
            if (in_file == NULL) {
                in_file = argv[i];
            } else {
                printf("Dont know what to do with extra parameter '%s'\n", argv[i]);
            }
        }
    }
    // Need variable name for .c file and .h file
    if (variable == NULL) {
        help(argv[0]);
        return 1;
    }

    // Need binary file to generate .c file
    if (cfile != NULL && in_file == NULL) {
        help(argv[0]);
        return 1;
    }

    // Must either generate .h file or .c file
    if (cfile == NULL && hfile == NULL) {
        help(argv[0]);
        return 1;
    }

    // Generate .h file
    if (hfile != NULL) {
        // Generate header guard
        char *hguard = NULL;
        if (!filename_to_hguard(hfile, &hguard)) {
            printf("Could not generate header guard\n");
            return 1;
        }

        // Generate .h content
        char *fcontent = NULL;
        if (!generate_h(&fcontent, hguard, variable)) {
            printf("Could not generate .h file content\n");
            free(hguard);
            return 1;
        }
        free(hguard);

        // Write to .h file
        if (!write_file(hfile, fcontent)) {
            printf("Could not write .h file '%s'\n", hfile);
            free(fcontent);
            return 1;
        }
        free(fcontent);
    }

    // Generate .c file
    if (cfile != NULL) {
        // Read binary file
        unsigned char *binary = NULL;
        long binary_size = 0;
        if (!read_file(in_file, &binary, &binary_size)) {
            // free(binary); //TODO ?
            return 1;
        }

        // Encode to Base64
        char *b64_buffer = b64_encode(binary, binary_size);
        free(binary);

        // Generate .c content
        char *file_content = NULL;
        if (!generate_c(&file_content, b64_buffer, variable)) {
            printf("Could not generate .c file content\n");
            free(b64_buffer);
            return 1;
        }
        free(b64_buffer);

        // Write content to .c file
        if (!write_file(cfile, file_content)) {
            printf("Could not write .c file '%s'\n", cfile);
            free(file_content);
            return 1;
        }
        free(file_content);
    }


    return 0;
}

void help(char *name) {
    // TODO
    printf("Usage: %s <input> -o <output>\n", name);
    printf("\n");
    printf("This script takes an input...\n");
}

bool filename_to_hguard(const char *fname, char **hguard) {
    size_t flen = strlen(fname);
    size_t hlen = flen;

    // Allocate header guard
    *hguard = malloc(flen + 1);
    if (!*hguard) {
        printf("Could not allocate memory for header guard\n");
        return false;
    }
    (*hguard)[flen] = '\0';

    size_t i;
    size_t j;
    for (i = 0, j = 0; i < flen; i++, j++) {
        char c = fname[i];

        // .h becomes _H
        if (c == '.') {
            (*hguard)[j] = '_';
            continue;
        }

        // Directory separator will be '_'
        if (c == '/' || c == '\\') {
            (*hguard)[j] = '_';
            continue;
        }

        // '_' is allowed
        if (c == '_') {
            (*hguard)[j] = c;
            continue;
        }

        // A-Z characters are allowed
        if (c > 0x40 && c < 0x5B) {
            (*hguard)[j] = c;
            continue;
        }
        
        // a-z characters are converted to uppercase A-Z
        if (c > 0x60 && c < 0x7B) {
            (*hguard)[j] = c - 0x20;
            continue;
        }


        // Skip all other characters
        j--;
        hlen--;
        continue;
    }

    // Resize buffer if characters have been skipped
    if (hlen != flen) {
        (*hguard) = realloc(*hguard, hlen + 1);
        (*hguard)[hlen] = '\0';
    }

    return true;
}

bool read_file(const char *fname, unsigned char **buffer, long *size) {
    FILE *f = fopen(fname, "rb");
    if (!f) {
        printf("Could not open file '%s'\n", fname);
        return false;
    }

    // Read file into buffer
    fseek(f, 0, SEEK_END);
    *size = ftell(f);
    fseek(f, 0, SEEK_SET);
    *buffer = malloc(*size);
    if (!*buffer) {
        printf("Could not allocate memory for file buffer\n");
        fclose(f);
        free(*buffer);
        return false;
    }
    fread(*buffer, 1, *size, f);
    fclose(f);

    return true;
}

bool write_file(const char *fname, char *content) {
    FILE *f = fopen(fname, "w");
    if (!f) {
        printf("Could not open file '%s'\n", fname);
        return false;
    }

    //printf("Attempting to write to '%s' the following content:\n", fname);
    //printf("%s\n", string);
    fputs(content, f);

    fclose(f);
    return true;
}

bool append_string(char **buf, int* buf_len, const char* str) {
    // Always grow buffer by at least this amount
    const int auto_growth = 20;

    // Grow buffer if needed
    int free = *buf_len - strlen(*buf) - 1;
    int needed = strlen(str);
    if (needed > free) {
        int diff = needed - free;

        *buf_len += (diff > auto_growth) ? diff + auto_growth : auto_growth;
        *buf = realloc(*buf, *buf_len);
        if (*buf == NULL) {
            printf("Could not allocate memory\n");
            return false;
        }
    }
    strcat(*buf, str);
    return true;
}

bool generate_h(char **out, char *hguard, char *variable) {
    // Create string to append content to
    int out_len = 20;
    *out = malloc(out_len * sizeof(char));
    if (*out == NULL) {
        printf("Could not allocate memory\n");
        free(*out);
        return false;
    }
    *out[0] = '\0';


    // Add header guard
    append_string(out, &out_len, "\n#ifndef ");
    append_string(out, &out_len, hguard);
    append_string(out, &out_len, "\n#define ");
    append_string(out, &out_len, hguard);
    append_string(out, &out_len, "\n");

    // Add warning
    append_string(out, &out_len, "\n// This file is generated automatically"
                                 " - all changes will be overwritten!\n");

    // Add variable declaration
    append_string(out, &out_len, "\nunsigned long b64image_");
    append_string(out, &out_len, variable);
    append_string(out, &out_len, "_length;\n");

  //append_string(out, &out_len, "\nconst char *b64image_");
    append_string(out, &out_len, "\nchar *b64image_");
    append_string(out, &out_len, variable);
    append_string(out, &out_len, ";\n");

    // End header guard
    append_string(out, &out_len, "\n#endif\n");

    // Resize buffer length to minimum required
    out_len = strlen(*out) + 1;
    *out = realloc(*out, out_len);
    if (*out == NULL) {
        printf("Could not allocate memory\n");
        free(*out);
        return false;
    }
    (*out)[out_len - 1] = '\0';

    return true;
}

bool generate_c(char **out, char *b64, char *variable) {
    // Create string to append content to
    int out_len = 20;
    *out = malloc(out_len * sizeof(char));
    if (*out == NULL) {
        printf("Could not allocate memory\n");
        free(*out);
        return false;
    }
    *out[0] = '\0';

    // Add warning
    append_string(out, &out_len, "\n// This file is generated automatically"
                                 " - all changes will be overwritten!\n");

    // Add variable definition
    append_string(out, &out_len, "\nunsigned long b64image_");
    append_string(out, &out_len, variable);
    append_string(out, &out_len, "_length = 0;\n");

  //append_string(out, &out_len, "\nconst char *b64image_");
    append_string(out, &out_len, "\nchar *b64image_");
    append_string(out, &out_len, variable);
    append_string(out, &out_len, " = \n");

    // Add Base64 string char by char
    const int CHARS_PER_LINE = 60;
    size_t len = strlen(b64);
    for (size_t i = 0; i < len; i++) {
        append_string(out, &out_len, "        \"");
        for (int k = 0; k < CHARS_PER_LINE; k++) {
            if (i + k < len) {
                //append_string(out, &out_len, &b64[i + k]);
                //TODO: Fixme
                char *foo = malloc(2);
                foo[0] = b64[i + k];
                foo[1] = '\0';
                append_string(out, &out_len, foo);
                free(foo);
            } else {
                break;
            }
        }
        i += CHARS_PER_LINE - 1;

        append_string(out, &out_len, "\"\n");
    }
    append_string(out, &out_len, "        ;\n\n");


    // Resize buffer length to minimum required
    out_len = strlen(*out) + 1;
    *out = realloc(*out, out_len);
    if (*out == NULL) {
        printf("Could not allocate memory\n");
        free(*out);
        return false;
    }
    (*out)[out_len - 1] = '\0';

    return true;
}

char *b64_encode(const unsigned char *in, const size_t len) {
    // Check input
    if (in == NULL || len == 0) {
        printf("Error - given binary data is NULL");
        exit(1);
        return NULL;
    }
    
    size_t encoded_length = b64_encoded_size(len);

    char *out = malloc(encoded_length + 1);
    out[encoded_length] = '\0';

    size_t i, j;
    for (i = 0, j = 0; i < len; i += 3, j += 4) {
        // Read 3 Byte of binary
        uint32_t v = in[i];
        v = (i + 1 < len) ? v << 8 | in[i + 1] : v << 8;
        v = (i + 2 < len) ? v << 8 | in[i + 2] : v << 8;

        // Take 6 Bit and encode it with 1 Base64 Byte
        out[j]                     = b64chars[(v >> 18) & 0x3F];
        out[j + 1]                 = b64chars[(v >> 12) & 0x3F];
        out[j + 2] = (i + 1 < len) ? b64chars[(v >>  6) & 0x3F] : '=';
        out[j + 3] = (i + 2 < len) ? b64chars[ v        & 0x3F] : '=';
    }
    
    return out;
}

size_t b64_encoded_size(size_t binary_size) {
    size_t size = binary_size;

    // Binary size needs to be divisable by 3
    if (binary_size % 3 != 0) {
    	size += 3 - (binary_size % 3);
    }

    // 3 binary Byte are represented by 4 Base64 chars
    size /= 3;
    size *= 4;
    
    return size;
}

