#include "h/textio.h"
#include "h/debug.h"
#include "h/utils.h"
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

Error LoadText(const int fd, char *const textBuf, const size_t textBufSize)
{
    ASSERT(textBuf);

    long int numberRead = read(fd, textBuf, textBufSize - 1);
    Error error = CreateSuccess();
    if (numberRead != (long int)(textBufSize - 1))
    {
        error.exitCode = ecCantReadFile;
        return error;
    }

    // fprintf(stderr, "DEBUG: fileSize = %lu\n", fileSize);
    textBuf[textBufSize - 1] = '\0';
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
