#include "gbemulator/common.h"
#include "gbemulator/stack.h"
#include "gbemulator/cpu.h"
#include "gbemulator/bus.h"

void stack_push(u8 data) {
    cpu_get_regs()->sp--;
    bus_write(cpu_get_regs()->sp, data);
}

void stack_push16(u16 data) {
    stack_push((data >> 8) & 0xFF);
    stack_push(data & 0xFF);
}

u8 stack_pop(void) {
    return bus_read(cpu_get_regs()->sp++);
}

u16 stack_pop16(void) {
    u16 lo = stack_pop();
    u16 hi = stack_pop();

    return (hi << 8) | lo;
}