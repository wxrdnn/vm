#ifndef PROC_PROC_H

#define PROC_PROC_H

#include "../stack.h"

struct Processor
{
    bool halted;
    byte_t *byteCode;
    size_t byteCodeSize;
    unsigned long pc;

    Stack_t *stack;

    Digit_t regA;
    Digit_t regB;
    Digit_t regC;
    Digit_t regD;
};

Error InitializeProcessor(Processor *const proc, const size_t byteCodeSize, const size_t initialStackCap);

void FreeProcessorData(Processor *const proc);

Error RunCommand(Processor *const proc, FILE *input, FILE *output);

#endif
