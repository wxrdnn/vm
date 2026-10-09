#ifndef TYPES_H

#define TYPES_H

#include "constants.h"

typedef unsigned InstructionArg_t;

struct Instruction
{
    unsigned code;
    InstructionArg_t args[cMaxInstructionArgs];
    unsigned argsAmount;
};

#endif
