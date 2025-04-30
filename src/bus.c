#include "gbemulator/cart.h"

u8 bus_read(u16 address) {
    if (address < 0x8000)
        return cart_read(address);

    return 0;
}

u8 bus_write(u16 address, u16 value) {
    if (address < 0x8000)
        return cart_write(address, value);

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
    ret = ret + (bus_write(address, value & 0xFF) >> 8);
    return ret;
}