#ifndef PROC_TYPES_H

#define PROC_TYPES_H

#include "../stack.h"
#include <cstddef>

struct Ram
{
    Digit_t *data;
    size_t size;
};

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

    Ram *ram;

    FILE *inputfp;
    FILE *outputfp;
};

typedef Error (*CommandFunc)(Processor *);

struct CommandTable
{
    CommandFunc *data;
    size_t size;
};

struct ProcInitData
{
    size_t byteCodeSize;
    size_t stackCap;
    size_t funcStackCap;
    size_t ramSize;

    FILE *inputfp;
    FILE *outputfp;
};

#endif
