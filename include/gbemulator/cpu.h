#pragma once

#include "gbemulator/common.h"
#include "gbemulator/instruction.h"

typedef struct {
    u8 a;
    u8 b;
    u8 c;
    u8 d;
    u8 e;
    u8 f;
    u8 h;
    u8 l;
    u16 sp;
    u16 pc;
} cpu_registers;

typedef struct {
    cpu_registers regs;
    instruction *cur_inst;
} cpu_context;