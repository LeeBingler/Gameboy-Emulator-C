#include "gbemulator/cpu.h"
#include "gbemulator/common.h"
#include "gbemulator/emu.h"
#include "gbemulator/bus.h"
#include "gbemulator/stack.h"


static bool proc_none(cpu_context *ctx) {
    fprintf(stderr, "proc_none(): INVALID INSTRUCTION! %02X\n", ctx->cur_opcode);
    return true;
}

static bool proc_nop(cpu_context *ctx) {
    (void) ctx;
    return false;
}

static bool proc_ld(cpu_context *ctx) {
    if (ctx->dest_is_mem) {
        if (ctx->cur_inst->reg_2 >= RT_AF) {
            bus_write16(ctx->mem_dest, ctx->fetched_data);
            emu_cycles(1);
        } else {
            bus_write(ctx->mem_dest, ctx->fetched_data);
        }

        emu_cycles(1);
        return false;
    }

    if (ctx->cur_inst->mode == AM_HL_SPR) { // 0xF8 instruction
        u8 hflag = (cpu_read_reg(ctx->cur_inst->reg_2) & 0xF) + (ctx->fetched_data & 0x0F) >= 0x10;
        u8 cflag = (cpu_read_reg(ctx->cur_inst->reg_2) & 0xFF) + (ctx->fetched_data & 0x0F) >= 0x100;

        cpu_set_flags(ctx, 0, 0, hflag, cflag);
        cpu_set_reg(ctx->cur_inst->reg_1, cpu_read_reg(ctx->cur_inst->reg_2) + (char) ctx->fetched_data);
        return false;
    }

    cpu_set_reg(ctx->cur_inst->reg_1, ctx->fetched_data);
    return false;
}

static bool proc_ldh(cpu_context *ctx) {
    if (ctx->cur_inst->reg_1 == RT_A) {
        cpu_set_reg(ctx->cur_inst->reg_1, bus_read(0xFF00 | ctx->fetched_data));
    } else {
        bus_write(0xFF00 | ctx->fetched_data, ctx->regs.a);
    }

    emu_cycles(1);
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

static bool goto_addr(cpu_context *ctx, u16 addr, bool pushpc) {
    if(check_cond(ctx)) {
        if (pushpc) {
            stack_push16(ctx->regs.pc);
            emu_cycles(2);
        }

        ctx->regs.pc = addr;
        emu_cycles(1);
    }
    return false;
}

static bool proc_jp(cpu_context *ctx) {
    return goto_addr(ctx, ctx->fetched_data, false);
}

static bool proc_jr(cpu_context *ctx) {
    char rel = (char) (ctx->fetched_data & 0xFF);
    u16 addr = ctx->regs.pc + rel;

    return goto_addr(ctx, addr, false);
}

static bool proc_call(cpu_context *ctx) {
    return goto_addr(ctx, ctx->fetched_data, true);
}

static bool proc_ret(cpu_context *ctx) {
    if (ctx->cur_inst->cond != CT_NONE) {
        emu_cycles(1);
    }

    if(check_cond(ctx)) {
        u16 lo = stack_pop();
        emu_cycles(1);

        u16 hi = stack_pop();
        emu_cycles(1);

        u16 n = (hi << 8) | lo;
        ctx->regs.pc = n;
    }
    return false;
}

static bool proc_reti(cpu_context *ctx) {
    ctx->int_master_enabled = true;
    return proc_ret(ctx);
}

static bool proc_pop(cpu_context *ctx) {
    u16 lo = stack_pop();
    emu_cycles(1);

    u16 hi = stack_pop();
    emu_cycles(1);

    u16 n = (hi << 8) | lo;

    if (ctx->cur_inst->reg_1 == RT_AF) {
        cpu_set_reg(ctx->cur_inst->reg_1, n & 0xFFF0);
    } else {
        cpu_set_reg(ctx->cur_inst->reg_1, n);
    }

    return false;
}

static bool proc_push(cpu_context *ctx) {
    u16 hi = (cpu_read_reg(ctx->cur_inst->reg_1) >> 8) & 0xFF;
    emu_cycles(1);
    stack_push(hi);

    u16 lo = cpu_read_reg(ctx->cur_inst->reg_1) & 0xFF;
    emu_cycles(1);
    stack_push(lo);

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
    [IN_LDH] = proc_ldh,
    [IN_JP] = proc_jp,
    [IN_JR] = proc_jr,
    [IN_POP] = proc_pop,
    [IN_PUSH] = proc_push,
    [IN_CALL] = proc_call,
    [IN_RET] = proc_ret,
    [IN_RETI] = proc_reti,
    [IN_XOR] = proc_xor,
    [IN_DI] = proc_di,
};

IN_PROC inst_get_processor(in_type type) {
    return processors[type];
}