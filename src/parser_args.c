#include "gbemulator/common.h"
#include <string.h>

static void print_helper(void) {
    printf("Usage: %s <rom_file>\n", NAME_EXEC);
}

bool parser_args(int argc, char **argv) {
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            print_helper();
            return false;
        }
    }

    if (argc != 2) {
        fprintf(stderr, "Missing argument: missing <rom_file>\n");
        fprintf(stderr, "Try '%s --help' for more information.\n", NAME_EXEC);
        return false;
    }

    return true;
}
