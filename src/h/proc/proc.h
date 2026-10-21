#ifndef PROC_PROC_H

#define PROC_PROC_H

#include "../stack.h"
#include "types.h"

Error InitializeProcessor(Processor *const proc, const size_t byteCodeSize, const size_t initialStackCap,
                          const size_t initialFuncStackCap);

void FreeProcessorData(Processor *const proc);

Error RunCommand(Processor *const proc, FILE *input, FILE *output, CommandTable *const table);

Error GetRegisterById(const byte_t id, Processor *const proc, Digit_t **result);

#endif
