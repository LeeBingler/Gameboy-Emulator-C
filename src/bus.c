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
