#ifndef PROC_INSTRUCTIONS_H

#define PROC_INSTRUCTIONS_H

enum ProcessorCommand
{
    pcHLT = 0x0,
    pcPUSH = 0x1,
    pcADD = 0x2,
    pcSUB = 0x3,
    pcMUL = 0x4,
    pcDIV = 0x5,
    pcOUT = 0x6,
    pcPUSHr = 0x7,
    pcPOPr = 0x8,
    pcJMP = 0x9,
    pcJA = 0xA,
    pcJAE = 0xB,
    pcJB = 0xC,
    pcJBE = 0xD,
    pcJE = 0xE,
    pcJNE = 0xF,
    pcCALL = 0x10,
    pcRET = 0x11,
    pcPOP = 0x12,
};

#endif
