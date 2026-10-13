#ifndef PROC_INPUT_H

#define PROC_INPUT_H

#include "../errorHandle.h"
#include "../types.h"

Error LoadByteCodeFromBinary(const int fileDescriptor, const size_t fileSize, byte_t *byteCode,
                             const size_t byteCodeSize);

#endif
