#ifndef INSTTABLE_H

#define INSTTABLE_H

#include "../constants.h"
#include "../errorHandle.h"

struct InstructionNode
{
    InstructionNode *next;
    unsigned hash;
    unsigned code;
};

struct InstructionTable
{
    InstructionNode **data;
    size_t size;
};

Error CreateInstructionTable(const size_t size, InstructionTable *const tablePtr);

InstructionNode *GetInstruction(const InstructionTable *const table, const unsigned hash);

Error AddInstruction(InstructionTable *const tablePtr, const unsigned hash, const unsigned code);

void DestroyInstructionTable(InstructionTable *const tablePtr);

InstructionNode *FindTailNode(const InstructionNode *const start);

InstructionNode *FindFirstBeforeTailNode(const InstructionNode *const start);

#endif
