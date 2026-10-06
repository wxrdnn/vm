#include "h/utils.h"
#include "string.h"
#include <cassert>
#include <cstddef>
#include <math.h>
#include <stdio.h>

const size_t cMaxLine = 1024;

void Swap(double *const a, double *const b)
{
    assert(a != NULL);
    assert(b != NULL);
    double t = *a;
    *a = *b;
    *b = t;
    return;
}

long CountLinesOfFile(FILE *const fp)
{
    assert(fp != NULL);
    long currentPos = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char buffer[cMaxLine] = {};
    long count = 0;
    while (fgets(buffer, cMaxLine, fp) != NULL)
    {
        ++count;
    }

    fseek(fp, currentPos, SEEK_SET);
    return count;
}

bool ReplaceNewLineCharWithNullTerminator(char *const s)
{
    assert(s != NULL);
    char *p = strchr(s, '\n');
    if (p)
    {
        *p = '\0';
        return true;
    }
    return false;
}

void ReplaceAllNewLineCharWithNullTerminator(char *s)
{
    while (ReplaceNewLineCharWithNullTerminator(s))
        s += strlen(s) + 1;
    return;
}

void ClearBuffer()
{
    int c = 0;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
    return;
}

bool FileIsEmpty(FILE *const fp)
{
    assert(fp != NULL);

    int c = 0; // default value
    if ((c = getc(fp)) == EOF)
    {
        return true;
    }

    ungetc(c, fp);
    return false;
}
