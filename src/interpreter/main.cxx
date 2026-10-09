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
        if (table.data)                                                                                                \
        {                                                                                                              \
            DestroyInstructionTable(&table);                                                                           \
        }                                                                                                              \
        if (inputFileDescriptor)                                                                                       \
        {                                                                                                              \
            close(inputFileDescriptor);                                                                                \
        }                                                                                                              \
        if (outputFileDescriptor)                                                                                      \
        {                                                                                                              \
            close(outputFileDescriptor);                                                                               \
        }                                                                                                              \
        if (textBuf)                                                                                                   \
        {                                                                                                              \
            free(textBuf);                                                                                             \
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
    Error error = CreateSuccess();

    InstructionTable table = {};
    int inputFileDescriptor = open(argv[1], O_RDONLY);
    size_t inputFileSize = 0;
    int outputFileDescriptor = creat("output", 0644);
    char *textBuf = NULL;

    SHUTDOWN_IF_ERROR_FAIL(error = HandleError(CreateInstructionTable(1, &table)));

    // fprintf(stderr,
    //         "DEBUG: main(): code of instruction <ADD>: %u\n",
    //         GetInstruction(&table, InstructionNameToHash("ADD"))->code);

    if (argc != 3)
    {
        printf(__RED "ERROR: Wrong input.\n" __BLUE "Usage: %s [INPUT] [REFERENCE]\n" __RESET, argv[0]);
        SHUTDOWN(ecWrongUsage);
    }

    SHUTDOWN_IF_ERROR_FAIL(error = HandleError(LoadInstrucitonTableFromFile(&table, argv[2])));

    // fprintf(stderr, "DEBUG: Input file descriptor: %d\n", inputFileDescriptor);
    if (inputFileDescriptor < 0)
    {
        error = CreateError(TranslateErrnoCode(errno), argv[1]);
        SHUTDOWN(HandleError(error).exitCode);
    }

    SHUTDOWN_IF_ERROR_FAIL(error = GetFileSize(argv[1], &inputFileSize));
    // printf("DEBUG: Input file size: %lu\n", inputFileSize);

    SHUTDOWN_IF_ERROR_FAIL(error = CreateTextBuf(inputFileSize, &textBuf));

    if (outputFileDescriptor < 0)
    {
        error = CreateError(TranslateErrnoCode(errno), "output");
        return HandleError(error).exitCode;
    }

    SHUTDOWN_IF_ERROR_FAIL(HandleError(error = LoadText(inputFileDescriptor, textBuf, inputFileSize)));
    SHUTDOWN_IF_ERROR_FAIL(
        HandleError(error = CompileAssembler(textBuf, argv[1], inputFileSize, outputFileDescriptor, &table)));

    SHUTDOWN(ecSuccess);
}
