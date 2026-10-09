#ifndef PROC_H

#define PROC_H

#include "../stack.h"

typedef int Digit_t;
struct CPU
{
    unsigned *byteCode;
    unsigned long procCount;

    Stack_t *stack;

    Digit_t regA;
    Digit_t regB;
    Digit_t regC;
    Digit_t regD;
};

#endif
