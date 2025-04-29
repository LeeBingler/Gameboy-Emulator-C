#include "gbemulator/common.h"
#include "gbemulator/cpu.h"
#include "gbemulator/emu.h"
#include "gbemulator/bus.h"

extern cpu_context ctx;

bool cpu_fetch_data(void) {
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
            break;

        case AM_R_R:
            ctx.fetched_data = cpu_read_reg(ctx.cur_inst->reg_2);
            break;

        case AM_R_D8:
            ctx.fetched_data = bus_read(ctx.regs.pc);
            emu_cycles(1);
            ctx.regs.pc++;
            break;

        case AM_R_D16:
        case AM_D16: {
            u16 lo = bus_read(ctx.regs.pc);
            emu_cycles(1);

            u16 hi = bus_read(ctx.regs.pc + 1);
            emu_cycles(1);

            ctx.fetched_data = lo | (hi << 8);

            ctx.regs.pc += 2;
        } break;

        case AM_MR_R:
            ctx.fetched_data = cpu_read_reg(ctx.cur_inst->reg_2);
            ctx.mem_dest = cpu_read_reg(ctx.cur_inst->reg_1);
            ctx.dest_is_mem = true;

            if (ctx.cur_inst->reg_1 == RT_C) { // instruction LDH (C), A   0xF2
                ctx.mem_dest |= 0xFF00;
            }

            break;

        case AM_R_MR: {
            u16 address = cpu_read_reg(ctx.cur_inst->reg_2);

            if (ctx.cur_inst->reg_1 == RT_C) { // instruction LDH (C), A   0xF2
                ctx.mem_dest |= 0xFF00;
            }

            ctx.fetched_data = bus_read(address);
            emu_cycles(1);

        } break;

        case AM_R_HLI:
            ctx.fetched_data = bus_read(cpu_read_reg(ctx.cur_inst->reg_2));
            emu_cycles(1);

            cpu_set_reg(RT_HL, cpu_read_reg(RT_HL) + 1);
            break;

        case AM_R_HLD:
            ctx.fetched_data = bus_read(cpu_read_reg(ctx.cur_inst->reg_2));
            emu_cycles(1);

            cpu_set_reg(RT_HL, cpu_read_reg(RT_HL) - 1);
            break;

        case AM_HLI_R:
            ctx.fetched_data = cpu_read_reg(ctx.cur_inst->reg_2);
            ctx.mem_dest = cpu_read_reg(ctx.cur_inst->reg_1);
            ctx.dest_is_mem = true;
            cpu_set_reg(RT_HL, cpu_read_reg(RT_HL) + 1);
            break;

        case AM_HLD_R:
            ctx.fetched_data = cpu_read_reg(ctx.cur_inst->reg_2);
            ctx.mem_dest = cpu_read_reg(ctx.cur_inst->reg_1);
            ctx.dest_is_mem = true;
            cpu_set_reg(RT_HL, cpu_read_reg(RT_HL) - 1);
            break;

        case AM_R_A8:
            ctx.fetched_data = bus_read(ctx.regs.pc);
            emu_cycles(1);
            ctx.regs.pc++;
            break;

        case AM_A8_R:
            ctx.fetched_data = cpu_read_reg(ctx.cur_inst->reg_2);
            ctx.mem_dest = bus_read(ctx.regs.pc) | 0xFF00;
            ctx.dest_is_mem = true;
            emu_cycles(1);
            ctx.regs.pc++;
            break;

        case AM_D8:
        case AM_HL_SPR:
            ctx.fetched_data = bus_read(ctx.regs.pc);
            emu_cycles(1);
            ctx.regs.pc++;
            break;

        case AM_A16_R:
        case AM_D16_R: {
            ctx.fetched_data = cpu_read_reg(ctx.cur_inst->reg_2);
            u16 lo = bus_read(ctx.regs.pc);
            emu_cycles(1);

            u16 hi = bus_read(ctx.regs.pc + 1);
            emu_cycles(1);

            ctx.mem_dest = lo | (hi << 8);
            ctx.dest_is_mem = true;

            ctx.regs.pc += 2;
        } break;

        case AM_MR_D8:
            ctx.fetched_data = bus_read(ctx.regs.pc);
            emu_cycles(1);
            ctx.regs.pc++;

            ctx.mem_dest = cpu_read_reg(ctx.cur_inst->reg_1);
            ctx.dest_is_mem = true;
            break;

        case AM_MR:
            ctx.mem_dest = cpu_read_reg(ctx.cur_inst->reg_1);
            ctx.dest_is_mem = true;
            ctx.fetched_data = bus_read(cpu_read_reg(ctx.cur_inst->reg_1));
            emu_cycles(1);
            break;

        case AM_R_A16: {
            u16 lo = bus_read(ctx.regs.pc);
            emu_cycles(1);

            u16 hi = bus_read(ctx.regs.pc + 1);
            emu_cycles(1);
            ctx.regs.pc += 2;

            u16 addr = lo | (hi << 8);

            ctx.fetched_data = bus_read(addr);
            emu_cycles(1);

        } break;

        default:
            fprintf(stderr, "Unknown Addressing Mode! %d (%02X)\n", ctx.cur_inst->mode, ctx.cur_opcode);
            return true;
    }

    return false;
}
