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

int main(int argc, char *argv[])
{
    Error error = CreateSuccess();

    InstructionTable table = {};
    RETURN_EXITCODE_IF_FAIL(error = HandleError(CreateInstructionTable(1, &table)));
    RETURN_EXITCODE_IF_FAIL(error = HandleError(LoadInstrucitonTableFromFile(&table, "reference")));
    // fprintf(stderr,
    //         "DEBUG: main(): code of instruction <ADD>: %u\n",
    //         GetInstruction(&table, InstructionNameToHash("ADD"))->code);

    size_t inputFileSize = 0;
    if (argc != 2)
    {
        printf(__RED "ERROR: Wrong input.\n" __BLUE "Usage: %s [INPUT]\n" __RESET, argv[0]);
        return ecWrongUsage;
    }

    int inputFileDescriptor = open(argv[1], O_RDONLY);
    // fprintf(stderr, "DEBUG: Input file descriptor: %d\n", inputFileDescriptor);
    if (inputFileDescriptor < 0)
    {
        error = CreateError(TranslateErrnoCode(errno), argv[1]);
        return HandleError(error).exitCode;
    }

    RETURN_EXITCODE_IF_FAIL(error = GetFileSize(argv[1], &inputFileSize));
    // printf("DEBUG: Input file size: %lu\n", inputFileSize);

    char *textBuf = NULL;
    RETURN_EXITCODE_IF_FAIL(error = CreateTextBuf(inputFileSize + 1, &textBuf)); // FIXME MAGIC NUMBER

    int outputFileDescriptor = creat("output", 0644);
    if (outputFileDescriptor < 0)
    {
        error = CreateError(TranslateErrnoCode(errno), "output");
        return HandleError(error).exitCode;
    }

    RETURN_EXITCODE_IF_FAIL(HandleError(error = LoadText(inputFileDescriptor, textBuf, inputFileSize)));
    RETURN_EXITCODE_IF_FAIL(
        HandleError(error = CompileAssembler(textBuf, argv[1], inputFileSize, outputFileDescriptor, &table)));

    free(textBuf);
    close(inputFileDescriptor);
    close(outputFileDescriptor);
    DestroyInstructionTable(&table);
    return ecSuccess;
}
