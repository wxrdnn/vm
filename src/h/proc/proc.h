#ifndef PROC_PROC_H

#define PROC_PROC_H

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

Error InitializeProcessor(Processor *const proc, const size_t byteCodeSize, const size_t initialStackCap,
                          const size_t initialFuncStackCap);

void FreeProcessorData(Processor *const proc);

Error RunCommand(Processor *const proc, FILE *input, FILE *output);

Error GetRegisterById(const byte_t id, Processor *const proc, Digit_t **result);

#endif
