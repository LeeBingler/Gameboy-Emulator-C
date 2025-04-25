#include "gbemulator/cart.h"

u8 bus_read(u16 address) {
    if (address < 0x7fff)
        return cart_read(address);
}

u8 bus_write(u16 address, u16 value) {
    if (address < 0x7fff)
        return cart_write(address, value);
}
