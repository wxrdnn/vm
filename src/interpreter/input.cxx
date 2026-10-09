#include "../h/interpreter/input.h"
#include "../h/debug.h"
#include "../h/errorHandle.h"
#include "../h/interpreter/instTable.h"
#include "../h/interpreter/interpreter.h"
#include "../h/utils.h"
#include <assert.h>
#include <cerrno>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

char *ReadLine(char *buf, const size_t bufSize, FILE *inputFile)
{
    ASSERT(buf);
    memset(buf, 0, bufSize);
    return fgets(buf, (int)(bufSize - 1), inputFile);
}

Error LoadInstrucitonTableFromFile(InstructionTable *const table, const char *const filePath)
{
    ASSERT(table);
    ASSERT(table->data);
    ASSERT(filePath);

    Error error = CreateSuccess();

    FILE *fp = fopen(filePath, "r");
    if (!fp)
    {
        error = CreateError(TranslateErrnoCode(errno), filePath);
    }

    char buf[cMaxLine] = {};
    unsigned lineCount = 1;
    while (fgets(buf, cMaxLine - 1, fp))
    {
        char nameRead[cMaxInstructionNameLength + 1] = {};
        unsigned codeRead = 0;

        int itemsRead = sscanf(buf, "%4s %u", nameRead, &codeRead); // FIXME HARDOCDED
        if (itemsRead != 2 && *buf != '\0')
        {
            fprintf(stderr,
                    __RED "ERROR: Failed to parse instruction \"%s\" from %s:%u. Items read = %d, expected: "
                          "<INSTRUCTION> <CODE>\n" __RESET,
                    buf,
                    filePath,
                    lineCount,
                    itemsRead);
            error = CreateError(ecParsingFailed, buf);
            return error;
        }

        error = AddInstruction(table, InstructionNameToHash(nameRead), codeRead);
        if (IsFail(&error))
        {
            return error;
        }

        lineCount++;
    }

    fclose(fp);
    return error;
}
