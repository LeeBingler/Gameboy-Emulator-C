#pragma once

#include "gbemulator/common.h"

void stack_push(u8 data);
void stack_push16(u16 data);

u8 stack_pop(void);
u16 stack_pop16(void);