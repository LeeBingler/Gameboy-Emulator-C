#include "gbemulator/common.h"
#include "gbemulator/ram.h"

typedef struct {
    u8 wram[0x2000];
    u8 hram[0x80];
} ram_context;

static ram_context ctx = {0};

u8 wram_read(u16 address) {
    address -= 0xC000;

    return ctx.wram[address];
}

u8 wram_write(u16 address, u8 value) {
    address -= 0xC000;

    return ctx.wram[address] = value;
}

u8 hram_read(u16 address) {
    address -= 0xFF80;

    return ctx.hram[address];
}

u8 hram_write(u16 address, u8 value) {
    address -= 0xFF80;

    return ctx.hram[address] = value;
}