#include "../h/interpreter/input.h"
#include "../h/debug.h"
#include "../h/errorHandle.h"
#include "../h/utils.h"
#include <assert.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

Error GetFileSize(const char *const path, size_t *size)
{
    ASSERT(size);

    struct stat buf = {};
    int errnoCode = stat(path, &buf);
    Error error = CreateError(TranslateErrnoCode(errnoCode), path);
    *size = (size_t)buf.st_size;
    return error;
}

Error LoadText(const int fd, char *const textBuf, const size_t fileSize) // needs textBuf size of <fileSize + 1>
{
    ASSERT(textBuf);

    long int numberRead = read(fd, textBuf, fileSize);
    Error error = CreateSuccess();
    if (numberRead != (long int)fileSize)
    {
        error.exitCode = ecCantReadFile;
        return error;
    }

    fprintf(stderr, "DEBUG: fileSize = %lu\n", fileSize);
    textBuf[fileSize - 1] = '\0';
    ReplaceAllNewLineCharWithNullTerminator(textBuf);

    return error;
}

Error CreateTextBuf(const size_t size, char **textBuf)
{
    ASSERT(textBuf);

    *textBuf = (char *)calloc(size, sizeof(char));
    Error error = CreateError(ecSuccess, "");
    if (!*textBuf)
    {
        error = CreateError(ecCantAllocateMemory, "text buffer");
        return error;
    }
    return error;
}

char *ReadLine(char *buf, const size_t bufSize, FILE *inputFile)
{
    ASSERT(buf);
    memset(buf, 0, bufSize);
    return fgets(buf, (int)(bufSize - 1), inputFile);
}
