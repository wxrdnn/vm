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

#endif
