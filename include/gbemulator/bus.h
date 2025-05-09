#pragma once

#include "gbemulator/common.h"


u8 bus_read(u16 address);
u8 bus_write(u16 address, u16 value);
u16 bus_read16(u16 address);
u16 bus_write16(u16 address, u16 value);