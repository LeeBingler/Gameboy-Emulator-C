#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gbemulator/common.h"
#include "gbemulator/cart.h"

static void print_helper(void) {
    printf("Usage: %s <rom_file>\n", NAME_EXEC);
}

static bool parser_args(int argc, char **argv) {
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

int main(int argc, char **argv) {
    if (!parser_args(argc, argv)) return EXIT_FAILURE;

    if(!load_cart(argv[1])) return EXIT_FAILURE;

    free_cart();
    return EXIT_SUCCESS;
}
