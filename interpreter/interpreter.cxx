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

    fprintf(stderr, __YELLOW "DEBUG: GetCommandFromLine(): line = <%s>\n" __RESET, line);
    int itemsParsed = sscanf(line, "%4s %u", instName, &inst->args[0]); // FIXME HARDCODED!!!

    if (*instName == '\0')
    {
        return EOF;
    }
    InstructionNode *foundInstruction = GetInstruction(table, InstructionNameToHash(instName));
    if (!foundInstruction)
    {
        fprintf(stderr, __RED "ERROR: No \"%s\" instruction found in instruction table!\n" __RESET, instName);
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
        if (itemsParsed == EOF)
        {
            return error;
        }
        if (itemsParsed != inst.argsAmount + 1)
        {
            error.exitCode = ecParsingFailed;
            fprintf(stderr,
                    __RED "ERROR: Failed to parse \"%s\" at %s:%lu.\n" __RESET,
                    prevLinePos,
                    inputFileName,
                    lineCount);
            strncpy(error.context, prevLinePos, cMaxLine - 1);
            return error;
        }

        // fprintf(stderr, __YELLOW "DEBUG: prevLinePos = <%s>, first char = <%d>\n" __RESET, prevLinePos,
        // *prevLinePos);

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

    if (inst->argsAmount == 0)
    {
        sprintf(buf, "%u\n", inst->code); // FIXME HARDCODED!!!
    }
    else if (inst->argsAmount == 1)
    {
        sprintf(buf, "%u %u\n", inst->code, inst->args[0]); // FIXME HARDCODED!!!
    }

    fprintf(stderr, __YELLOW "DEBUG: WriteInstructionToFile(): buf = <%s>" __RESET, buf);
    if (write(fd, buf, sizeof(char) * strlen(buf)) == -1)
    {
        error = CreateError(TranslateErrnoCode(errno), "");
        return error;
    }

    return error;
}

// void DumpInstruction(const Instruction *const inst)
// {
//     ASSERT(inst);
//
//     // fprintf(stderr, "DEBUG: inst.code = %u\n", inst->code); // TODO Expand
//
//     return;
// }
