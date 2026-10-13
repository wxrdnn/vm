#ifndef TYPES_H

#define TYPES_H

#include "constants.h"

typedef unsigned char byte_t;
typedef unsigned InstructionArg_t;

struct Instruction
{
    byte_t code;
    InstructionArg_t args[cMaxInstructionArgs];
    unsigned argsAmount;
};

#endif
