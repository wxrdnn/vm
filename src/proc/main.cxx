#include "../h/proc/input.h"
#include "../h/proc/output.h"
#include "../h/proc/proc.h"
#include "../h/textio.h"
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    // TODO SHUTDOWN macro

    Error error = CreateSuccess();

    Processor proc = {};

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
    RETURN_EXITCODE_IF_FAIL(GetFileSize(argv[1], &inputFileSize));

    RETURN_EXITCODE_IF_FAIL(InitializeProcessor(&proc, inputFileSize, 64));

    RETURN_EXITCODE_IF_FAIL(LoadByteCodeFromBinary(inputfd, inputFileSize, proc.byteCode, inputFileSize));

    DumpByte_tArrInHex(proc.byteCode, proc.byteCodeSize);

    FreeProcessorData(&proc);
    close(inputfd);
    return error.exitCode;
}
