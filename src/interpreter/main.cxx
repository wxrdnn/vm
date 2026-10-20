#include "../h/colors.h"
#include "../h/errorHandle.h"
#include "../h/interpreter/input.h"
#include "../h/interpreter/instTable.h"
#include "../h/interpreter/interpreter.h"
#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>

#define SHUTDOWN(__exitCode)                                                                                           \
    {                                                                                                                  \
        if (ctx.instTable->data)                                                                                       \
        {                                                                                                              \
            DestroyInstructionTable(ctx.instTable);                                                                    \
        }                                                                                                              \
        if (ctx.lableTable->data)                                                                                      \
        {                                                                                                              \
            DestroyLableTable(ctx.lableTable);                                                                         \
        }                                                                                                              \
        if (inputFileDescriptor)                                                                                       \
        {                                                                                                              \
            close(inputFileDescriptor);                                                                                \
        }                                                                                                              \
        if (ctx.outputFileDescriptor)                                                                                  \
        {                                                                                                              \
            close(ctx.outputFileDescriptor);                                                                           \
        }                                                                                                              \
        if (ctx.inputBuf)                                                                                              \
        {                                                                                                              \
            free(ctx.inputBuf);                                                                                        \
        }                                                                                                              \
                                                                                                                       \
        return __exitCode;                                                                                             \
    }

#define SHUTDOWN_IF_ERROR_FAIL(__error)                                                                                \
    {                                                                                                                  \
        ExitCode __code = (__error).exitCode;                                                                          \
        if (__code != ecSuccess)                                                                                       \
        {                                                                                                              \
            SHUTDOWN(__code);                                                                                          \
        }                                                                                                              \
    }

int main(int argc, char *argv[])
{
    // TODO 8 BYTE
    // TODO comments
    // TODO массив структур с кодом комманды, именем и указателем на функций
    Error error = CreateSuccess();

    InstructionTable instTable = {};
    LableTable lableTable = {};
    int inputFileDescriptor = open(argv[1], O_RDONLY);

    // int preProcFileDescriptor = creat("preproc", 0644);
    int preProcFileDescriptor = open("/dev/null", O_RDWR);
    int outputFileDescriptor = creat("output", 0644);

    AssemblyContext ctx = {
        .inputBuf = NULL,
        .inputBufSize = 0,
        .inputFileNameForDebug = argv[1],
        .instTable = &instTable,
        .lableTable = &lableTable,
        .outputFileDescriptor = 0,
    };

    SHUTDOWN_IF_ERROR_FAIL(HandleError(CreateInstructionTable(1, ctx.instTable)));

    SHUTDOWN_IF_ERROR_FAIL(HandleError(CreateLableTable(&lableTable, 64)));

    // fprintf(stderr,
    //         "DEBUG: main(): code of instruction <ADD>: %u\n",
    //         GetInstruction(&table, InstructionNameToHash("ADD"))->code);

    if (argc != 3)
    {
        printf(__RED "ERROR: Wrong input.\n" __BLUE "Usage: %s [INPUT] [REFERENCE]\n" __RESET, argv[0]);
        SHUTDOWN(ecWrongUsage);
    }

    SHUTDOWN_IF_ERROR_FAIL(HandleError(LoadInstrucitonTableFromFile(&instTable, argv[2])));

    // fprintf(stderr, "DEBUG: Input file descriptor: %d\n", inputFileDescriptor);
    if (inputFileDescriptor < 0)
    {
        error = CreateError(TranslateErrnoCode(errno), argv[1]);
        SHUTDOWN(HandleError(error).exitCode);
    }

    SHUTDOWN_IF_ERROR_FAIL(GetFileSize(argv[1], &ctx.inputBufSize));
    // printf("DEBUG: Input file size: %lu\n", inputFileSize);

    SHUTDOWN_IF_ERROR_FAIL(CreateTextBuf(ctx.inputBufSize, &ctx.inputBuf));

    if (preProcFileDescriptor < 0)
    {
        error = CreateError(TranslateErrnoCode(errno), "preproc");
        return HandleError(error).exitCode;
    }
    if (outputFileDescriptor < 0)
    {
        error = CreateError(TranslateErrnoCode(errno), "output");
        return HandleError(error).exitCode;
    }

    SHUTDOWN_IF_ERROR_FAIL(HandleError(LoadText(inputFileDescriptor, ctx.inputBuf, ctx.inputBufSize)));

    // Assembly in 2 iterations
    ctx.outputFileDescriptor = preProcFileDescriptor;
    SHUTDOWN_IF_ERROR_FAIL(HandleError(CompileAssembler(&ctx)));
    fprintf(stderr, "DEBUG: ---------------------------------------\n");
    ctx.outputFileDescriptor = outputFileDescriptor;
    SHUTDOWN_IF_ERROR_FAIL(HandleError(CompileAssembler(&ctx)));

    SHUTDOWN(ecSuccess);
}
