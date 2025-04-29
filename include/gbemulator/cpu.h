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

    //current fetch...
    u16 fetched_data;
    u16 mem_dest;
    bool dest_is_mem;
    u8 cur_opcode;
    instruction *cur_inst;

    bool halted;
    bool stepping;

    bool int_master_enabled;

} cpu_context;


bool cpu_init(void);
bool cpu_step(void);

// UTILS
u16 cpu_read_reg(reg_type rt);
u16 reverse(u16 n);