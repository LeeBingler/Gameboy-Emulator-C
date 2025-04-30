#pragma once

#include "gbemulator/common.h"

u8 wram_read(u16 address);
u8 wram_write(u16 address, u8 value);

u8 hram_read(u16 address);
u8 hram_write(u16 address, u8 value);