#include "gbemulator/cart.h"
#include "gbemulator/instruction.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static cart_context ctx = { 0 };

static const char *ROM_TYPES[35] = {
    "ROM ONLY",
    "MBC1",
    "MBC1+RAM",
    "MBC1+RAM+BATTERY",
    "0x04 ???",
    "MBC2",
    "MBC2+BATTERY",
    "0x07 ???",
    "ROM+RAM 1",
    "ROM+RAM+BATTERY 1",
    "0x0A ???",
    "MMM01",
    "MMM01+RAM",
    "MMM01+RAM+BATTERY",
    "0x0E ???",
    "MBC3+TIMER+BATTERY",
    "MBC3+TIMER+RAM+BATTERY 2",
    "MBC3",
    "MBC3+RAM 2",
    "MBC3+RAM+BATTERY 2",
    "0x14 ???",
    "0x15 ???",
    "0x16 ???",
    "0x17 ???",
    "0x18 ???",
    "MBC5",
    "MBC5+RAM",
    "MBC5+RAM+BATTERY",
    "MBC5+RUMBLE",
    "MBC5+RUMBLE+RAM",
    "MBC5+RUMBLE+RAM+BATTERY",
    "0x1F ???",
    "MBC6",
    "0x21 ???",
    "MBC7+SENSOR+RUMBLE+RAM+BATTERY",
};

static const char *LIC_CODE[0xA5] = {
    [0x00] = "None",
    [0x01] = "Nintendo R&D1",
    [0x08] = "Capcom",
    [0x13] = "Electronic Arts",
    [0x18] = "Hudson Soft",
    [0x19] = "b-ai",
    [0x20] = "kss",
    [0x22] = "pow",
    [0x24] = "PCM Complete",
    [0x25] = "san-x",
    [0x28] = "Kemco Japan",
    [0x29] = "seta",
    [0x30] = "Viacom",
    [0x31] = "Nintendo",
    [0x32] = "Bandai",
    [0x33] = "Ocean/Acclaim",
    [0x34] = "Konami",
    [0x35] = "Hector",
    [0x37] = "Taito",
    [0x38] = "Hudson",
    [0x39] = "Banpresto",
    [0x41] = "Ubi Soft",
    [0x42] = "Atlus",
    [0x44] = "Malibu",
    [0x46] = "angel",
    [0x47] = "Bullet-Proof",
    [0x49] = "irem",
    [0x50] = "Absolute",
    [0x51] = "Acclaim",
    [0x52] = "Activision",
    [0x53] = "American sammy",
    [0x54] = "Konami",
    [0x55] = "Hi tech entertainment",
    [0x56] = "LJN",
    [0x57] = "Matchbox",
    [0x58] = "Mattel",
    [0x59] = "Milton Bradley",
    [0x60] = "Titus",
    [0x61] = "Virgin",
    [0x64] = "LucasArts",
    [0x67] = "Ocean",
    [0x69] = "Electronic Arts",
    [0x70] = "Infogrames",
    [0x71] = "Interplay",
    [0x72] = "Broderbund",
    [0x73] = "sculptured",
    [0x75] = "sci",
    [0x78] = "THQ",
    [0x79] = "Accolade",
    [0x80] = "misawa",
    [0x83] = "lozc",
    [0x86] = "Tokuma Shoten Intermedia",
    [0x87] = "Tsukuda Original",
    [0x91] = "Chunsoft",
    [0x92] = "Video system",
    [0x93] = "Ocean/Acclaim",
    [0x95] = "Varie",
    [0x96] = "Yonezawa/s’pal",
    [0x97] = "Kaneko",
    [0x99] = "Pack in soft",
    [0xA4] = "Konami (Yu-Gi-Oh!)"
};


u8 cart_read(u16 address) {
    return ctx.rom_data[address];
}

u8 cart_write(u16 address, u16 value) {
    return value;
}

void free_cart(void) {
    free(ctx.rom_data);
}

const char *cart_lic_name(void) {
    if (ctx.header->new_licence_code < 0xA5) {
        return LIC_CODE[ctx.header->old_licence_code];
    }

    return "UNKNOWN";
}

const char *cart_romtype_name(void) {
    if(ctx.header->cartridge_type < 34)
        return ROM_TYPES[ctx.header->cartridge_type];

    return "UNKNOWN";
}

bool load_cart(char *cart_name) {
    FILE *fd = fopen(cart_name, "rb");

    if (!fd) {
        fprintf(stderr, "Failed to open %s", cart_name);
        return false;
    }

    strncpy(ctx.filename, cart_name, sizeof(ctx.filename) - 1);
    ctx.filename[sizeof(ctx.filename) - 1] = '\0';

    fseek(fd, 0, SEEK_END);
    ctx.rom_size = ftell(fd);
    rewind(fd);

    if (!(ctx.rom_data = calloc(1, ctx.rom_size))){
        fclose(fd);
        return false;
    }

    if (fread(ctx.rom_data, ctx.rom_size, 1, fd) != 1) {
        fprintf(stderr, "Failed to read ROM file\n");
        free(ctx.rom_data);
        fclose(fd);
        return false;
    }

    fclose(fd);

    // Entry point (https://gbdev.io/pandocs/The_Cartridge_Header.html#0100-0103--entry-point)
    ctx.header = (rom_header *)(ctx.rom_data + 0x100);
    ctx.header->title[15] = '\0';

#ifdef DEBUG
    printf("Cartridge Loaded:\n");
    printf("\t Title    : %s\n", ctx.header->title);
    printf("\t Type     : %2.2X (%s)\n", ctx.header->cartridge_type, cart_romtype_name());
    printf("\t ROM Size : %d KB\n", 32 << ctx.header->rom_size);
    printf("\t RAM Size : %2.2X\n", ctx.header->ram_size);
    printf("\t LIC Code : %2.2X (%s)\n", ctx.header->old_licence_code, cart_lic_name());
    printf("\t ROM Vers : %2.2X\n", ctx.header->rom_version);
#endif

    // Checksum test (https://gbdev.io/pandocs/The_Cartridge_Header.html#014d--header-checksum)
    u16 checksum = 0;
    for (u16 address = 0x0134; address <= 0x014c; address++) {
        checksum = checksum - ctx.rom_data[address] - 1;
    }

#ifdef DEBUG
    printf("\t Checksum : %2.2X (%s)\n", ctx.header->checksum, (checksum & 0xFF) ? "PASSED" : "FAILED");
#endif

    return true;
}
