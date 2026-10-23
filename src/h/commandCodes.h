#ifndef COMMAND_CODES_H

#define COMMAND_CODES_H

enum ProcessorCommand
{
    pcHLT = 0x0,

    pcPUSHc = 0b00100001,
    pcPUSHr = 0b01000001,
    // pcPUSHm = 0b10000001, // Doesn't make sense
    pcPUSHrc = 0b01100001,
    pcPUSHmc = 0b10100001,
    pcPUSHmr = 0b11000001,
    pcPUSHmrc = 0b11100001,

    pcPOP = 0b00000010, // Just delete value from stack
    pcPOPr = 0b01000010,
    // pcPOPm = 0b10000010, // Doesn't make sense
    // pcPOPrc = 0b01100010, // Doesn't make sense
    pcPOPmc = 0b10100010,
    pcPOPmr = 0b11000010,
    pcPOPmrc = 0b11000010,

    pcADD = 0x2,
    pcSUB = 0x3,
    pcMUL = 0x4,
    pcDIV = 0x5,
    pcOUT = 0x6,
    pcJMP = 0x9,
    pcJA = 0xA,
    pcJAE = 0xB,
    pcJB = 0xC,
    pcJBE = 0xD,
    pcJE = 0xE,
    pcJNE = 0xF,
    pcCALL = 0x10,
    pcRET = 0x11,
};

#endif
