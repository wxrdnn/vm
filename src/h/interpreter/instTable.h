#ifndef INTERPRETER_INSTTABLE_H

#define INTERPRETER_INSTTABLE_H

#include "../constants.h"
#include "../errorHandle.h"
#include "../types.h"
#include <cstdint>

struct InstructionNode
{
    InstructionNode *next;
    uint64_t hash;
    byte_t code;
};

struct InstructionTable
{
    InstructionNode **data;
    size_t size;
};

Error CreateInstructionTable(const size_t size, InstructionTable *const tablePtr);

InstructionNode *GetInstruction(const InstructionTable *const table, const uint64_t hash);

Error AddInstruction(InstructionTable *const tablePtr, const uint64_t hash, const byte_t code);

void DestroyInstructionTable(InstructionTable *const tablePtr);

InstructionNode *FindTailNode(InstructionNode *const start);

InstructionNode *FindFirstBeforeTailNode(InstructionNode *const start);

#endif
