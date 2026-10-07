#include "../h/interpreter/interpreter.h"
#include "../h/debug.h"
#include "../h/errorHandle.h"
#include "../h/interpreter/instTable.h"
#include "../h/utils.h"
#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <unistd.h>

int GetCommandFromLine(const char *const line, Instruction *const inst, const InstructionTable *const table)
{
    ASSERT(line);

    char instName[cMaxLine] = {};

    fprintf(stderr, "DEBUG: GetCommandFromLine(): line = <%s>\n", line);
    int itemsParsed = sscanf(line, "%4s %u", instName, &inst->args[0]); // FIXME HARDCODED!!!

    InstructionNode *foundInstruction = GetInstruction(table, InstructionNameToHash(instName));
    if (!foundInstruction)
    {
        fprintf(stderr, "ERROR: No \"%s\" instruction found in instruction table!\n", instName);
        return 0;
    }

    inst->argsAmount = (unsigned)(itemsParsed - 1);
    inst->code = foundInstruction->code;
    return itemsParsed;
}

unsigned InstructionNameToHash(const char *const instructionName)
{
    ASSERT(instructionName);
    const unsigned *p = (const unsigned *)instructionName;
    return *p;
}

Error CompileAssembler(const char *const inputBuf, const char *inputFileName, size_t inputBufSize,
                       const int outputFileDescriptor, const InstructionTable *const table)
{
    ASSERT(inputBuf);
    ASSERT(inputFileName);

    Error error = CreateSuccess();
    const char *prevLinePos = inputBuf;
    const char *nextLinePos = inputBuf;
    size_t lineCount = 1;
    Instruction inst = {};

    while ((unsigned)(nextLinePos - inputBuf) < inputBufSize)
    {
        nextLinePos = prevLinePos + strlen(prevLinePos);
        // fprintf(stderr,
        //         "DEBUG: lineCount: %lu, (nextLinePos - textBuf) = %ld, textBufSize = %lu.\n",
        //         indexBuf->lineCount,
        //         nextLinePos - textBuf,
        //         textBufSize);

        int itemsParsed = GetCommandFromLine(prevLinePos, &inst, table);
        if (itemsParsed > 2)
        {
            error.exitCode = ecParsingFailed;
            fprintf(stderr, "ERROR: Failed to parse \"%s\" at %s:%lu.\n", prevLinePos, inputFileName, lineCount);
            strncpy(error.context, prevLinePos, cMaxLine - 1);
            return error;
        }

        fprintf(stderr, "DEBUG: prevLinePos = <%s>, first char = <%d>\n", prevLinePos, *prevLinePos);

        if (*prevLinePos != '\0')
        {
            error = WriteInstructionToFile(&inst, outputFileDescriptor);
            if (IsFail(&error))
            {
                return error;
            }
        }

        prevLinePos = nextLinePos + 1;
        lineCount++;
        // fprintf(stderr, "DEBUG: string after prevLinePos: <%s>", prevLinePos);
    }

    return error;
}

Error WriteInstructionToFile(const Instruction *const inst, const int fd)
{
    ASSERT(inst);

    Error error = CreateSuccess();
    char buf[cMaxLine] = {};

    DumpInstruction(inst);

    if (inst->argsAmount == 0)
    {
        sprintf(buf, "%X\n", inst->code); // FIXME HARDCODED!!!
    }
    else if (inst->argsAmount == 1)
    {
        sprintf(buf, "%X %u\n", inst->code, inst->args[0]); // FIXME HARDCODED!!!
    }

    fprintf(stderr, "DEBUG: WriteInstructionToFile(): buf = <%s>", buf);
    if (write(fd, buf, sizeof(char) * strlen(buf)) == -1)
    {
        error = CreateError(TranslateErrnoCode(errno), "");
        return error;
    }

    return error;
}

void DumpInstruction(const Instruction *const inst)
{
    ASSERT(inst);

    fprintf(stderr, "DEBUG: inst.code = %u\n", inst->code); // TODO Expand

    return;
}

// unsigned InstructionHashToCode(const unsigned hash)
// {
//     switch (hash)
//     {
//     case InstructionNameToHash("PUSH"):
//     }
// }
