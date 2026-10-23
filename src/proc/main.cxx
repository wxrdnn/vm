#include "../h/proc/commandTable.h"
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
        if (byteCodefd > 0)                                                                                            \
        {                                                                                                              \
            close(byteCodefd);                                                                                         \
        }                                                                                                              \
        if (table.data)                                                                                                \
        {                                                                                                              \
            FreeCommandTableData(&table);                                                                              \
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

    ProcInitData initData = {
        .byteCodeSize = 0,
        .stackCap = 32,
        .funcStackCap = 8,
        .ramSize = 1024,
        .inputfp = stdin,
        .outputfp = stdout,
    };

    CommandTable table = {};

    int byteCodefd = 0;

    if (argc != 2)
    {
        error = CreateError(ecWrongUsage, "");
        fprintf(stderr, "ERROR: Wrong usage. Usage: %s [INPUT]\n", argv[0]);
        return error.exitCode;
    }

    byteCodefd = open(argv[1], O_RDONLY);
    if (byteCodefd <= 0)
    {
        error = CreateError(TranslateErrnoCode(errno), argv[0]);
        return error.exitCode;
    }

    SHUTDOWN_IF_ERROR_FAIL(InitializeCommandTable(&table));

    LoadCommandTable(&table);

    SHUTDOWN_IF_ERROR_FAIL(GetFileSize(argv[1], &initData.byteCodeSize));

    SHUTDOWN_IF_ERROR_FAIL(InitializeProcessor(&proc, &initData));

    SHUTDOWN_IF_ERROR_FAIL(LoadByteCodeFromBinary(byteCodefd, initData.byteCodeSize, proc.byteCode, proc.byteCodeSize));

    DumpByte_tArrInHex(proc.byteCode, proc.byteCodeSize);

    while (!proc.halted && (proc.pc < proc.byteCodeSize))
    {
        // STACK_DUMP(proc.stack);
        // fprintf(stderr, "DEBUG: PC = %lu\n", proc.pc);
        SHUTDOWN_IF_ERROR_FAIL(RunCommand(&proc, &table));
    }

    SHUTDOWN(error.exitCode);
}
