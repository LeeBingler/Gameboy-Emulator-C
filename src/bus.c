#include "gbemulator/common.h"
#include "gbemulator/cart.h"
#include "gbemulator/ram.h"

#define NOT_IMPL {printf("NOT implemented: accessing address %04X\n", address); return 0;}

u8 bus_read(u16 address) {
    if (address < 0x8000) {
        // ROM data
        return cart_read(address);
    } else if (address < 0xA000) {
        // video RAM
        NOT_IMPL
    } else if (address < 0xC000) {
        // save / external RAM
        return cart_read(address);
    } else if (address < 0xE000) {
        // work RAM
        return wram_read(address);
    } else if (address < 0xFE00) {
        // echo RAM
        return 0;
    } else if (address < 0xFEA0) {
        // OAM (sprite attribute table)
        NOT_IMPL
    } else if (address < 0xFF00) {
        // forbidden
        return 0;
    } else if (address < 0xFF80) {
        // I/O register
        NOT_IMPL
    } else if (address < 0xFFFF) {
        // High RAM
        return hram_read(address);
    } else if (address == 0xFFFF) {
        // Interrupt switch
        NOT_IMPL
    }

    printf("Unknown bus_read(%04X)\n", address);
    return 0;
}

u8 bus_write(u16 address, u16 value) {
    if (address < 0x8000) {
        // ROM data
        return cart_write(address, value);
    } else if (address < 0xA000) {
        // video RAM
        NOT_IMPL
    } else if (address < 0xC000) {
        // save / external RAM
        return cart_write(address, value);
    } else if (address < 0xE000) {
        // work RAM
        return wram_write(address, value);
    } else if (address < 0xFE00) {
        // echo RAM
        return 0;
    } else if (address < 0xFEA0) {
        // OAM (sprite attribute table)
        NOT_IMPL
    } else if (address < 0xFF00) {
        // forbidden
        return 0;
    } else if (address < 0xFF80) {
        // I/O register
        NOT_IMPL
    } else if (address < 0xFFFF) {
        // High RAM
        return hram_write(address, value);
    } else if (address == 0xFFFF) {
        // Interrupt switch
        NOT_IMPL
    }

    printf("Unknown bus_write(%04X, %04X)\n", address, value);
    return 0;
}

u16 bus_read16(u16 address) {
    u16 lo = bus_read(address);
    u16 hi = bus_read(address + 1);

    return lo | (hi << 8);
}

u16 bus_write16(u16 address, u16 value) {
    u16 ret = 0;
    ret = bus_write(address + 1, (value >> 8) & 0xFF);
    ret += (bus_write(address, value & 0xFF) >> 8);
    return ret;
}