#ifndef PROC_INSTRUCTIONS_H

#define PROC_INSTRUCTIONS_H

enum ProcessorCommand
{
    pcHLT = 0x0,
    pcPUSH = 0x1,
    pcADD = 0x2,
    pcSUB = 0x3,
    pcOUT = 0x5,
};

#endif
