
#include "Main.h"

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
        return 1;
    }

    // If a file path was given on the command line load the requested
    // level - otherwise load the default level.
    if (filepath != NULL) {
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

    // Start the game
    game_loop();

    // End
    deinitialize();
    return 0;
}

