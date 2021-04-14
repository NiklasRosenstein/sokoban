
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

const char b64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int main(int argc, char *argv[]);
void help(char *name);
void filepath_to_filename(char *str);
void filename_to_hguard(char *str);
bool read_file(const char *fname, unsigned char **buffer, long *size);
bool write_file(const char *fname, char *content);
size_t b64_encoded_size(size_t binary_size);
char *b64_encode(const unsigned char *in, const size_t len);
bool generate_h(char *variable, char *out_file);
bool generate_c(char *b64, char *variable, char *out_file);

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

    // Check if .h file name is valid (must end with .h)
    if (hfile != NULL) {
        char ext = hfile[strlen(hfile) - 1];
        if (ext != 'h') {
            fprintf(stderr, "Given .h file '%s' must end with .h file extension\n", hfile);
            return 1;
        }
    }

    // Check if .c file name is valid (must end with .c)
    if (cfile != NULL) {
        char ext = cfile[strlen(cfile) - 1];
        if (ext != 'c') {
            fprintf(stderr, "Given .c file '%s' must end with .c file extension\n", cfile);
            return 1;
        }
    }

    // Generate .h file
    if (hfile != NULL) {
        if (!generate_h(variable, hfile)) {
            printf("Could not generate .h file content\n");
            return 1;
        }
        printf("Generated %s\n", hfile);
    }

    // Generate .c file
    if (cfile != NULL) {
        // Read binary file
        unsigned char *binary = NULL;
        long binary_size = 0;
        if (!read_file(in_file, &binary, &binary_size)) {
            return 1;
        }

        // Encode to Base64
        char *b64_buffer = b64_encode(binary, binary_size);
        free(binary);

        // Generate .c file
        if (!generate_c(b64_buffer, variable, cfile)) {
            printf("Could not generate .c file content\n");
            free(b64_buffer);
            return 1;
        }
        free(b64_buffer);
        printf("Generated %s\n", cfile);
    }


    return 0;
}

void help(char *name) {
    // TODO: Print help
    printf("Usage: %s <input> -o <output>\n", name);
    printf("\n");
    printf("This script takes an input...\n");
}

void filepath_to_filename(char *str) {
    int len = strlen(str) + 1;

    // Find start index of filename
    int start = 0;
    for (int i = 0; i < len; i++) {
        if (str[i] == '/' || str[i] == '\\') {
            start = i + 1;
        }
    }

    // Remove characters before '/'
    if (start == 0) {
        return;
    }
    else {
        int i;
        int j;
        for (i = 0, j = start; j < len; i++, j++) {
            str[i] = str[j];
        }
    }

    return;
}

void filename_to_hguard(char *str) {
    size_t len = strlen(str);

    size_t i;
    size_t j;
    for (i = 0, j = 0; i < len; i++, j++) {
        char c = str[i];

        // .h becomes _H
        if (c == '.') {
            str[j] = '_';
            continue;
        }

        // Directory separator will be '_'
        if (c == '/' || c == '\\') {
            str[j] = '_';
            continue;
        }

        // '_' is allowed
        if (c == '_') {
            str[j] = c;
            continue;
        }

        // A-Z characters are allowed
        if (c > 0x40 && c < 0x5B) {
            str[j] = c;
            continue;
        }

        // a-z characters are converted to uppercase A-Z
        if (c > 0x60 && c < 0x7B) {
            str[j] = c - 0x20;
            continue;
        }

        // Skip all other characters
        j--;
        continue;
    }
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

bool generate_h(char *variable, char *out_file) {
    // Create string to append content to
    int out_len = 20;
    char *out = malloc(out_len * sizeof(char));
    if (out == NULL) {
        printf("Could not allocate memory\n");
        free(out);
        return false;
    }
    out[0] = '\0';


    // Add warning
    append_string(&out, &out_len, "\n// This file is generated automatically"
                                 " - all changes will be overwritten!\n");

    // Add header guard
    {
        char *hguard = malloc(strlen(out_file) + 1);
        strcpy(hguard, out_file);

        // Convert file path to basename
        filepath_to_filename(hguard);

        // Generate header guard
        filename_to_hguard(hguard);

        append_string(&out, &out_len, "\n#ifndef ");
        append_string(&out, &out_len, hguard);
        append_string(&out, &out_len, "\n#define ");
        append_string(&out, &out_len, hguard);
        append_string(&out, &out_len, "\n");

        free(hguard);
    }

    // Include headers
    append_string(&out, &out_len, "\n");
    append_string(&out, &out_len, "#include <stdlib.h>\n\n");

    // Add variable declaration
    append_string(&out, &out_len, "\nextern size_t b64image_");
    append_string(&out, &out_len, variable);
    append_string(&out, &out_len, "_length;\n");

    append_string(&out, &out_len, "\nextern const char *b64image_");
    append_string(&out, &out_len, variable);
    append_string(&out, &out_len, ";\n");

    // End header guard
    append_string(&out, &out_len, "\n#endif\n");


    // Resize buffer length to minimum required
    out_len = strlen(out) + 1;
    out = realloc(out, out_len);
    if (out == NULL) {
        printf("Could not allocate memory\n");
        free(out);
        return false;
    }
    out[out_len - 1] = '\0';


    // Write to .h file
    if (!write_file(out_file, out)) {
        printf("Could not write .h file '%s'\n", out_file);
        free(out);
        return false;
    }

    free(out);
    return true;
}

bool generate_c(char *b64, char *variable, char *out_file) {
    // Create string to append content to
    int out_len = 20;
    char *out = malloc(out_len * sizeof(char));
    if (out == NULL) {
        printf("Could not allocate memory\n");
        free(out);
        return false;
    }
    out[0] = '\0';

    // Add warning
    append_string(&out, &out_len, "\n// This file is generated automatically"
                                 " - all changes will be overwritten!\n");

    // Include header
    append_string(&out, &out_len, "\n");
    append_string(&out, &out_len, "#include \"");
    {
        // Create .h file name from .c file name
        char *header = malloc(strlen(out_file) + 1);
        strcpy(header, out_file);
        header[strlen(out_file) - 1] = 'h';

        // Get basename of path
        filepath_to_filename(header);

        append_string(&out, &out_len, header);
        free(header);
    }
    append_string(&out, &out_len, "\"\n\n");

    // Add variable definition
    append_string(&out, &out_len, "\nsize_t b64image_");
    append_string(&out, &out_len, variable);
    append_string(&out, &out_len, "_length = 0;\n");

    append_string(&out, &out_len, "\nconst char *b64image_");
    append_string(&out, &out_len, variable);
    append_string(&out, &out_len, " = \n");

    // Add Base64 string char by char
    const int CHARS_PER_LINE = 60;
    size_t len = strlen(b64);
    for (size_t i = 0; i < len; i++) {
        append_string(&out, &out_len, "        \"");
        for (int k = 0; k < CHARS_PER_LINE; k++) {
            if (i + k < len) {
                //TODO: Add more chars at once
                char foo[] = { b64[i + k], '\0' };
                append_string(&out, &out_len, foo);
            } else {
                break;
            }
        }
        i += CHARS_PER_LINE - 1;

        append_string(&out, &out_len, "\"\n");
    }
    append_string(&out, &out_len, "        ;\n\n");


    // Resize buffer length to minimum required
    out_len = strlen(out) + 1;
    out = realloc(out, out_len);
    if (out == NULL) {
        printf("Could not allocate memory\n");
        free(out);
        return false;
    }
    out[out_len - 1] = '\0';


    // Write generated content to file
    if (!write_file(out_file, out)) {
        fprintf(stderr, "Could not write .c file '%s'\n", out_file);
        free(out);
        return false;
    }

    free(out);
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

