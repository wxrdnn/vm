#include "../h/interpreter/interpreter.h"
#include "../h/debug.h"
#include "../h/errorHandle.h"
#include "../h/interpreter/instTable.h"
#include "../h/utils.h"
#include <cassert>
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

Error CreateLableTable(LableTable *const lableTable, const size_t initSize)
{
    ASSERT(lableTable);
    Error error = CreateSuccess();

    lableTable->data = (size_t *)calloc(initSize, sizeof(size_t));
    if (!lableTable)
    {
        error = CreateError(ecCantAllocateMemory, "labelTable");
        return error;
    }

    lableTable->size = initSize;

    return error;
}

int GetCommandFromLine(const char *const line, Instruction *const inst, const InstructionTable *const table,
                       const LableTable *const lableTable, size_t *const processCount)
{
    ASSERT(line);

    char instName[cMaxInstructionNameLength + 1] = {};

    fprintf(stderr, __YELLOW "DEBUG: GetCommandFromLine(): Got line <%s>\n" __RESET, line);
    int itemsParsed =
        sscanf(line, "%8s %X %X %X", instName, &inst->args[0], &inst->args[1], &inst->args[2]); // FIXME HARDCODED!!!

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
    *processCount += sizeof(byte_t) + sizeof(unsigned) * inst->argsAmount;
    return itemsParsed;
}

uint64_t InstructionNameToHash(const char *const instructionName)
{
    ASSERT(instructionName);

    const uint64_t *p = (const uint64_t *)instructionName;
    return *p;
}

Error CompileAssembler(AssemblyContext *const ctx)
{
    ASSERT(ctx);
    ASSERT(ctx->inputBuf);
    ASSERT(ctx->inputFileNameForDebug);

    Error error = CreateSuccess();
    const char *prevLinePos = ctx->inputBuf;
    const char *nextLinePos = ctx->inputBuf;
    size_t lineCount = 1;
    Instruction inst = {};
    size_t processCount = 0;

    const char *bufToReadFrom = NULL;
    char *buf = NULL;
    bool freeBuf = false;

    while ((unsigned)(nextLinePos - ctx->inputBuf) < ctx->inputBufSize - 1)
    {
        size_t prevLinePosLen = strlen(prevLinePos);
        nextLinePos = prevLinePos + prevLinePosLen;

        // fprintf(stderr,
        //         "DEBUG: lineCount: %lu, (nextLinePos - textBuf) = %ld, textBufSize = %lu.\n",
        //         indexBuf->lineCount,
        //         nextLinePos - textBuf,
        //         textBufSize);

        if (*prevLinePos == '\0')
        {
            prevLinePos = nextLinePos + 1;
            lineCount++;
            continue;
        }

        if (*prevLinePos == ':')
        {
            AddLableToTable(prevLinePos, ctx->lableTable, processCount);
            prevLinePos = nextLinePos + 1;
            lineCount++;
            continue;
        }
        else if (strchr(prevLinePos, ':'))
        {
            size_t bufSize = prevLinePosLen + 1 + cMaxInstructionArgs * sizeof(size_t) * 2;
            buf = (char *)calloc(bufSize, sizeof(char));
            ProcessLineWithLables(buf, bufSize, prevLinePos, prevLinePosLen, ctx->lableTable);
            fprintf(stderr, "DEBUB: line <%s> after processing lable: <%s>\n", prevLinePos, buf);
            bufToReadFrom = buf;
        }
        else
        {
            bufToReadFrom = prevLinePos;
        }

        int itemsParsed = GetCommandFromLine(bufToReadFrom, &inst, ctx->instTable, ctx->lableTable, &processCount);
        if (itemsParsed == EOF)
        {
            return error;
        }
        if ((unsigned)itemsParsed != inst.argsAmount + 1)
        {
            error.exitCode = ecParsingFailed;
            fprintf(stderr,
                    __RED "ERROR: Failed to parse \"%s\" at %s:%lu.\n" __RESET,
                    prevLinePos,
                    ctx->inputFileNameForDebug,
                    lineCount);
            strncpy(error.context, bufToReadFrom, cMaxLine - 1);
            return error;
        }

        // fprintf(stderr, __YELLOW "DEBUG: prevLinePos = <%s>, first char = <%d>\n" __RESET, prevLinePos,
        // *prevLinePos);

        if (*bufToReadFrom != '\0')
        {
            error = WriteInstructionToFile(&inst, ctx->outputFileDescriptor);
            if (IsFail(&error))
            {
                return error;
            }
        }

        prevLinePos = nextLinePos + 1;
        lineCount++;
        // fprintf(stderr, "DEBUG: string after prevLinePos: <%s>", prevLinePos);
    }

    if (freeBuf)
    {
        free(buf);
    }
    return error;
}

Error WriteInstructionToFile(const Instruction *const inst, const int fd)
{
    ASSERT(inst);

    Error error = CreateSuccess();

    if (write(fd, &inst->code, sizeof(inst->code)) == -1)
    {
        error = CreateError(TranslateErrnoCode(errno), "binary code file");
        return error;
    }
    // fprintf(stderr, "DEBUG: Write <%b> to output with fd %d\n", inst->code, fd);

    if (write(fd, inst->args, sizeof(*inst->args) * inst->argsAmount) == -1)
    {
        error = CreateError(TranslateErrnoCode(errno), "binary code file");
        return error;
    }

    return error;
}

Error DumpInstructionToFile(const Instruction *const inst, const int fd)
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

    char *debugBuf = strdup(buf);
    ReplaceNewLineCharWithNullTerminator(debugBuf);
    fprintf(stderr, __YELLOW "DEBUG: WriteInstructionToFile(): Writing <%s> to output file\n" __RESET, debugBuf);
    free(debugBuf);

    if (write(fd, buf, sizeof(char) * strlen(buf)) == -1)
    {
        error = CreateError(TranslateErrnoCode(errno), "");
        return error;
    }

    return error;
}

void DestroyLableTable(LableTable *const lableTable)
{
    ASSERT(lableTable);

    free(lableTable->data);
    return;
}

Error AddLableToTable(const char *const line, LableTable *const lableTable, const size_t processCount)
{
    ASSERT(line);
    ASSERT(lableTable);

    Error error = CreateSuccess();
    unsigned labelIndex = 0;

    int itemsParsed = sscanf(line, ":%u", &labelIndex);
    if (itemsParsed == 1)
    {
        if (labelIndex > lableTable->size)
        {
            error.exitCode = ecWrongLable;
        }
        lableTable->data[labelIndex] = processCount;
    }
    else
    {
        error.exitCode = ecParsingFailed;
        strncpy(error.context, line, cMaxLine - 1);
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

Error ProcessLineWithLables(char *const buf, const size_t bufSize, const char *prevLinePos, const size_t prevLinePosLen,
                            const LableTable *const lableTable)
{
    ASSERT(buf);
    ASSERT(prevLinePos);

    Error error = CreateSuccess();

    if (!buf)
    {
        error = CreateError(ecCantAllocateMemory, "buffer");
        return error;
    }

    const char *fastP = strchr(prevLinePos, ':');
    const char *slowP = prevLinePos;
    char *bufP = buf;
    while (fastP != NULL && fastP < prevLinePos + prevLinePosLen && slowP < prevLinePos + prevLinePosLen)
    {
        ASSERT(fastP > slowP);
        ASSERT(bufP < buf + bufSize);
        memcpy(bufP, slowP, (fastP - slowP) * sizeof(char));
        bufP += fastP - slowP;
        // fprintf(
        //     stderr, "DEBUG: In ProcessLineWithLables: copied first %ld bytes of <%s> to bufP\n", fastP - slowP,
        //     slowP);

        unsigned index = 0;
        int itemsParsed = sscanf(fastP, ":%u", &index);
        if (itemsParsed != 1)
        {
            error.exitCode = ecParsingFailed;
            strncpy(error.context, prevLinePos, cMaxLine - 1);
            return error;
        }

        sprintf(bufP, "%lX", lableTable->data[index]);
        bufP += strlen(bufP);

        slowP = fastP;
        while (slowP < prevLinePos + prevLinePosLen && *slowP != ' ' && *slowP != '\0')
        {
            slowP++;
        }

        fastP = strchr(slowP, ':');
    }

    strncpy(bufP, slowP, bufSize - (bufP - buf));
    return error;
}
