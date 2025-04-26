#include "gbemulator/common.h"
#include "gbemulator/instruction.h"

instruction instrictions[0x100] = {
    [0x00] = {IN_NOP, AM_IMP},
    [0x01] = {IN_LD, AM_R_D16, RT_BC}
    // To be continued...
};