#ifndef INTERPRETER_H

#define INTERPRETER_H

#include "../errorHandle.h"
#include "../types.h"
#include <cstdio>

int GetCommandFromLine(const char *const line, Instruction *const inst);

unsigned InstructionNameToHash(const char *const instructionName);

Error CompileAssembler(const char *const inputBuf, const char *inputFileName, size_t inputBufSize,
                       const int outputFileDescriptor);

Error WriteInstructionToFile(const Instruction *const inst, const int fd);

void DumpInstruction(const Instruction *const inst);

#endif
