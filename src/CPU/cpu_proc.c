#include "gbemulator/cpu.h"
#include "gbemulator/common.h"
#include "gbemulator/emu.h"


static bool proc_none(cpu_context *ctx) {
    fprintf(stderr, "proc_none(): INVALID INSTRUCTION! %02X\n", ctx->cur_opcode);
    return true;
}

static bool proc_nop(cpu_context *ctx) {
    return false;
}

static bool proc_ld(cpu_context *ctx) {
    return false;
}

static bool check_cond(cpu_context *ctx) {
    bool z = CPU_FLAG_Z;
    bool c = CPU_FLAG_C;

    switch (ctx->cur_inst->cond) {
        case CT_NONE: return true;
        case CT_C: return c;
        case CT_NC: return !c;
        case CT_Z: return z;
        case CT_NZ: return !z;
    }

    return false;
}

static bool proc_jp(cpu_context *ctx) {
    if(check_cond(ctx)) {
        ctx->regs.pc = ctx->fetched_data;
        emu_cycles(1);
    }
    return false;
}

static bool proc_xor(cpu_context *ctx) {
    ctx->regs.a ^= ctx->fetched_data & 0xFF;
    cpu_set_flags(ctx, ctx->regs.a == 0, 0, 0, 0);
    return false;
}

static bool proc_di(cpu_context *ctx) {
    ctx->int_master_enabled = false;
    return false;
}

static IN_PROC processors[] = {
    [IN_NONE] = proc_none,
    [IN_NOP] = proc_nop,
    [IN_LD] = proc_ld,
    [IN_JP] = proc_jp,
    [IN_XOR] = proc_xor,
    [IN_DI] = proc_di,
};

IN_PROC inst_get_processor(in_type type) {
    return processors[type];
}