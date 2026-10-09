#include "../h/proc/input.h"
#include "../h/debug.h"
#include "../h/textio.h"
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>

Error LoadByteCode(const int fileDescriptor, const size_t fileSize, unsigned *byteCode, const size_t byteCodeSize)
{
    ASSERT(byteCode);

    Error error = CreateSuccess();
    //     if (fileDescriptor < 0)
    //     {
    //     }

    char *textBuf = (char *)calloc(fileSize, sizeof(char));
    if (!textBuf)
    {
        error = CreateError(ecCantAllocateMemory, "textBuf");
        return error;
    }

    error = LoadText(fileDescriptor, textBuf, fileSize);
    RETURN_ERROR_IF_FAIL(HandleError(error));

    int itemsParsed = 0;
    char *currentStr = textBuf;
    unsigned *currentByteCodePos = byteCode;

    unsigned buf1 = 0;
    unsigned buf2 = 0;
    while ((unsigned)(currentStr - textBuf) < fileSize &&
           (itemsParsed = sscanf(currentStr, "%u %u", &buf1, &buf2)) > 0 && itemsParsed < 3)
    {
        ASSERT(currentByteCodePos - byteCode < byteCodeSize);

        fprintf(stderr, "DEBUG: currentStr - textBuf = %ld, itemsParsed = %d\n", currentStr - textBuf, itemsParsed);

        *currentByteCodePos = buf1;
        ++currentByteCodePos;
        if (itemsParsed == 2)
        {
            ASSERT(currentByteCodePos - byteCode < byteCodeSize);
            *currentByteCodePos = buf2;
            ++currentByteCodePos;
        }

        currentStr += strlen(currentStr) + 1;
    }

    fprintf(stderr, "DEBUG: currentStr - textBuf = %ld, itemsParsed = %d\n", currentStr - textBuf, itemsParsed);

    return error;
}
