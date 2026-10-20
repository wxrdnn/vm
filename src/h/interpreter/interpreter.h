#ifndef INTERPRETER_INTERPRETER_H

#define INTERPRETER_INTERPRETER_H

#include "../errorHandle.h"
#include "../types.h"
#include "instTable.h"
#include <cstdint>
#include <cstdio>

struct LableTable
{
    size_t *data;
    size_t size;
};

struct AssemblyContext
{
    char *inputBuf;
    size_t inputBufSize;
    char *inputFileNameForDebug;

    InstructionTable *instTable;
    LableTable *lableTable;

    int outputFileDescriptor;
};

Error CreateLableTable(LableTable *const lableTable, const size_t initSize);

int GetCommandFromLine(const char *const line, Instruction *const inst, const InstructionTable *const table,
                       const LableTable *const lableTable, size_t *const processCount);

Error CompileAssembler(AssemblyContext *const ctx);

Error WriteInstructionToFile(const Instruction *const inst, const int fd);

Error DumpInstructionToFile(const Instruction *const inst, const int fd);

void DestroyLableTable(LableTable *const lableTable);

Error AddLableToTable(const char *const line, LableTable *const lableTable, const size_t processCount);

Error ProcessLineWithLables(char *const buf, const size_t bufSize, const char *prevLinePos, const size_t prevLinePosLen,
                            const LableTable *const lableTable);

// void DumpInstruction(const Instruction *const inst);

#endif
