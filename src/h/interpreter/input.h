#ifndef INPUT_H

#define INPUT_H

#include "../errorHandle.h"
#include "../types.h"
#include "instTable.h"
#include <cstddef>
#include <cstdio>

Error GetFileSize(const char *const path, size_t *size);

Error LoadText(const int fd, char *const textBuf, const size_t fileSize); // needs textBuf size of <fileSize + 1>

Error CreateTextBuf(const size_t size, char **textBuf);

char *ReadLine(char *buf, size_t bufSize, FILE *inputFile);

Error LoadInstrucitonTableFromFile(InstructionTable *const table, const char *const filePath);

#endif
