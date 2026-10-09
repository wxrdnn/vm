#ifndef PROC_INPUT_H

#define PROC_INPUT_H

#include "../errorHandle.h"

Error LoadByteCode(const int fileDescriptor, const size_t fileSize, unsigned *byteCode, const size_t byteCodeSize);

#endif
