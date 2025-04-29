#include "gbemulator/common.h"
#include "gbemulator/parser_args.h"
#include "gbemulator/emu.h"

#include "gbemulator/cpu.h"
#include "gbemulator/cart.h"

static emu_context ctx = {0};

emu_context *emu_get_context(void) {
    return &ctx;
}

void delay(u32 ms) {
    return;
}
void emu_cycles(int cpu_cycles) {
    return;
}

int main(int argc, char **argv) {
    if (!parser_args(argc, argv)) return EXIT_FAILURE;

    if(!load_cart(argv[1])) return EXIT_FAILURE;

    if(!cpu_init()) return EXIT_FAILURE;

    ctx.running = true;
    ctx.paused = false;
    ctx.ticks = 0;

    while(ctx.running) {
        if (ctx.paused) {
            delay(10);
            continue;
        }

        if (!cpu_step()) {
            printf("CPU Stopped\n");
            return -3;
        }

        ctx.ticks++;
    }

    free_cart();
    return EXIT_SUCCESS;
}

