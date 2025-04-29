#include "gbemulator/cpu.h"
#include "gbemulator/bus.h"
#include "gbemulator/emu.h"
#include "gbemulator/instruction.h"

cpu_context ctx = {0};

bool cpu_init(void) {
    ctx.regs.pc = 0x100;
    ctx.regs.a = 0x01;

#ifdef DEBUG
    printf("CPU init\n");
#endif // DEBUG

    return true;
}

static bool fetch_instruction(void) {
    ctx.cur_opcode = bus_read(ctx.regs.pc++);
    ctx.cur_inst = instruction_by_opcode(ctx.cur_opcode);
    return false;
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
        if(cpu_fetch_data()) return false;

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