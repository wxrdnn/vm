#ifndef UTILS_H

#define UTILS_H

#include <stdio.h>

//----------------------------------------------------------
//! Checks if two double numbers are equal with error rate
//----------------------------------------------------------

void Swap(double *const a, double *const b);

long CountLinesOfFile(FILE *const fp);

bool ReplaceNewLineCharWithNullTerminator(char *const s);

void ReplaceAllNewLineCharWithNullTerminator(char *s);

void ClearBuffer();

bool FileIsEmpty(FILE *const fp);

void DumpIntArr(const int *const arr, const size_t size);

void DumpUnsignedIntArr(const unsigned *const arr, const size_t size);

#define ARG_NAME_TO_STR(__x) #__x

#endif
