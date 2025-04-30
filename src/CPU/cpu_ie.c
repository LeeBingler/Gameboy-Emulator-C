#include "gbemulator/common.h"
#include "gbemulator/cpu.h"

extern cpu_context ctx;

u8 cpu_get_ie_register() {
    return ctx.ie_register;
}

u8 cpu_set_ie_register(u8 val) {
    return ctx.ie_register = val;
}