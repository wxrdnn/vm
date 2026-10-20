#include "../h/proc/input.h"
#include "../h/proc/output.h"
#include "../h/proc/proc.h"
#include "../h/textio.h"
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>

#define SHUTDOWN(__exitCode)                                                                                           \
    {                                                                                                                  \
        if (inputfd > 0)                                                                                               \
        {                                                                                                              \
            close(inputfd);                                                                                            \
        }                                                                                                              \
                                                                                                                       \
        FreeProcessorData(&proc);                                                                                      \
                                                                                                                       \
        return __exitCode;                                                                                             \
    }

#define SHUTDOWN_IF_ERROR_FAIL(__error)                                                                                \
    {                                                                                                                  \
        Error __err = __error;                                                                                         \
        if (__err.exitCode != ecSuccess)                                                                               \
        {                                                                                                              \
            HandleError(__err);                                                                                        \
            SHUTDOWN(__err.exitCode);                                                                                  \
        }                                                                                                              \
    }

int main(int argc, char *argv[])
{
    Error error = CreateSuccess();

    Processor proc = {};
    int inputfd = 0;
    size_t inputFileSize = 0;

    if (argc != 2)
    {
        error = CreateError(ecWrongUsage, "");
        fprintf(stderr, "ERROR: Wrong usage. Usage: %s [INPUT]\n", argv[0]);
        return error.exitCode;
    }

    inputfd = open(argv[1], O_RDONLY);
    if (inputfd <= 0)
    {
        error = CreateError(TranslateErrnoCode(errno), argv[0]);
        return error.exitCode;
    }

    SHUTDOWN_IF_ERROR_FAIL(GetFileSize(argv[1], &inputFileSize));

    SHUTDOWN_IF_ERROR_FAIL(InitializeProcessor(&proc, inputFileSize, 16, 8));

    SHUTDOWN_IF_ERROR_FAIL(LoadByteCodeFromBinary(inputfd, inputFileSize, proc.byteCode, inputFileSize));

    DumpByte_tArrInHex(proc.byteCode, proc.byteCodeSize);

    while (!proc.halted && (proc.pc < proc.byteCodeSize))
    {
        // STACK_DUMP(proc.stack);
        // fprintf(stderr, "DEBUG: PC = %lu\n", proc.pc);
        SHUTDOWN_IF_ERROR_FAIL(RunCommand(&proc, stdin, stdout));
    }

    SHUTDOWN(error.exitCode);
}
