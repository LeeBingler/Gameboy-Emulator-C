#include "gbemulator/cpu.h"
#include "gbemulator/bus.h"
#include "gbemulator/emu.h"
#include "gbemulator/instruction.h"

cpu_context ctx = {0};

bool cpu_init(void) {
    ctx.regs.pc = 0x100;
    ctx.regs.a = 0x01;
    return true;
}

static bool fetch_instruction(void) {
    ctx.cur_opcode = bus_read(ctx.regs.pc++);
    ctx.cur_inst = instruction_by_opcode(ctx.cur_opcode);
    return false;
}

static bool fetch_data(void) {
    ctx.mem_dest = 0;
    ctx.dest_is_mem = false;

    if (!(ctx.cur_inst)) {
        fprintf(stderr, "Unknown Addressing Mode! %02X\n", ctx.cur_opcode);
        return true;
    }

    switch (ctx.cur_inst->mode) {
        case AM_IMP: return false;

        case AM_R:
            ctx.fetched_data = cpu_read_reg(ctx.cur_inst->reg_1);
            return false;

        case AM_R_D8:
            ctx.fetched_data = bus_read(ctx.regs.pc);
            emu_cycles(1);
            ctx.regs.pc++;
            return false;

        case AM_D16: {
            u16 lo = bus_read(ctx.regs.pc);
            emu_cycles(1);

            u16 hi = bus_read(ctx.regs.pc + 1);
            emu_cycles(1);

            ctx.fetched_data = lo | (hi << 8);

            ctx.regs.pc += 2;

            return false;
        }

        default:
            fprintf(stderr, "Unknown Addressing Mode! %d (%02X)\n", ctx.cur_inst->mode, ctx.cur_opcode);
            return true;
    }
}

static bool execute(void) {
    IN_PROC proc = inst_get_processor(ctx.cur_inst->type);

    if (!proc) {
        fprintf(stderr, "execute failed\n");
        return true;
    }

    return proc(&ctx);
}

bool cpu_step(void) {
    if (!ctx.halted) {
        u16 pc = ctx.regs.pc;

        if(fetch_instruction()) return false;
        if(fetch_data()) return false;

#ifdef DEBUG
    printf("%04X: %-7s (%02X %02X %02X) A: %02X B: %02X C: %02X\n",
            pc, inst_name(ctx.cur_inst->type), ctx.cur_opcode,
            bus_read(pc + 1), bus_read(pc + 2), ctx.regs.a, ctx.regs.b, ctx.regs.c);
#endif

        if (ctx.cur_inst == NULL) {
            fprintf(stderr, "Unknown Instruction %02X\n", ctx.cur_opcode);
            return false;
        }

        if(execute()) return false;
    }
    return true;
}