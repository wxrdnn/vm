#include "../h/proc/input.h"
#include "../h/textio.h"
#include "../h/utils.h"
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    Error error = CreateSuccess();

    if (argc != 2)
    {
        error = CreateError(ecWrongUsage, "");
        fprintf(stderr, "ERROR: Wrong usage. Usage: %s [INPUT]\n", argv[0]);
        return error.exitCode;
    }

    int inputfd = open(argv[1], O_RDONLY);
    if (inputfd <= 0)
    {
        error = CreateError(TranslateErrnoCode(errno), argv[0]);
        return error.exitCode;
    }

    size_t inputFileSize = 0;
    error = GetFileSize(argv[1], &inputFileSize);
    RETURN_EXITCODE_IF_FAIL(error);

    unsigned *byteCode = (unsigned *)calloc(inputFileSize, sizeof(unsigned));

    error = LoadByteCode(inputfd, inputFileSize, byteCode, inputFileSize);
    RETURN_EXITCODE_IF_FAIL(error);

    DumpUnsignedIntArr(byteCode, inputFileSize);

    return error.exitCode;
}
