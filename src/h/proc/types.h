#ifndef PROC_TYPES_H

#define PROC_TYPES_H

#include "../stack.h"

struct Processor
{
    bool halted;
    byte_t *byteCode;
    size_t byteCodeSize;
    size_t pc;

    Stack_t *stack;

    Digit_t regA;
    Digit_t regB;
    Digit_t regC;
    Digit_t regD;

    Stack_t *funcStack;
};

typedef Error (*CommandFunc)(Processor *);

struct CommandTable
{
    CommandFunc *data;
    size_t size;
};

#endif
