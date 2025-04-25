#pragma once

#include "./common.h"

typedef struct {
    u8 entry[4];
    u8 logo[0x30];

    char title[16];
    u16 new_licence_code;
    u8 sgb_flag;
    u8 cartridge_type;
    u8 rom_size;
    u8 ram_size;
    u8 dest_code;
    u8 old_licence_code;
    u8 rom_version;
    u8 checksum;
    u16 global_checksum;
} rom_header;

typedef struct {
    char filename[1024];
    u32 rom_size;
    u8 *rom_data;
    rom_header *header;
} cart_context;

bool load_cart(char *cart_name);
void free_cart(void);

u8 cart_read(u16 address);
u8 cart_write(u16 address, u16 value);