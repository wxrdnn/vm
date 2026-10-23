#include "../h/proc/proc.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

Error _InitProcStack(Processor *const proc, const size_t initStackCap);
Error _InitProcFuncStack(Processor *const proc, const size_t initFuncStackCap);
Error _InitProcRam(Processor *const proc, const size_t initRamSize);

Error InitializeProcessor(Processor *const proc, const ProcInitData *const initData)
{
    ASSERT(proc);
    proc->halted = false;

    proc->byteCode = (byte_t *)calloc(initData->byteCodeSize, sizeof(byte_t));
    if (!proc->byteCode)
    {
        return CreateError(ecCantAllocateMemory, "proc->byteCode");
    }

    proc->byteCodeSize = initData->byteCodeSize * sizeof(byte_t);
    proc->pc = 0;

    RETURN_ERROR_IF_FAIL(_InitProcStack(proc, initData->stackCap));

    proc->regA = 0;
    proc->regB = 0;
    proc->regC = 0;
    proc->regD = 0;

    RETURN_ERROR_IF_FAIL(_InitProcFuncStack(proc, initData->funcStackCap));

    RETURN_ERROR_IF_FAIL(_InitProcRam(proc, initData->ramSize));

    proc->inputfp = initData->inputfp;
    proc->outputfp = initData->outputfp;

    return CreateSuccess();
}

Error _InitProcStack(Processor *const proc, const size_t initStackCap)
{
    ASSERT(proc);
    proc->stack = (Stack_t *)calloc(1, sizeof(Stack_t));
    if (!proc->stack)
    {
        return CreateError(ecCantAllocateMemory, "proc->stack");
    }

    StackDebugData *stkDebugData = (StackDebugData *)calloc(1, sizeof(StackDebugData));
    if (!stkDebugData)
    {
        return CreateError(ecCantAllocateMemory, "proc->stack->debugData");
    }
    stkDebugData->creationLine = __LINE__ - 13;
    stkDebugData->creationFile = __FILE__;
    stkDebugData->creationFunction = __FUNCTION__;
    stkDebugData->name = "proc->stack";

    RETURN_ERROR_IF_FAIL(StackInit(proc->stack, initStackCap, stkDebugData));

    return CreateSuccess();
}

Error _InitProcFuncStack(Processor *const proc, const size_t initFuncStackCap)
{
    ASSERT(proc);

    proc->funcStack = (Stack_t *)calloc(1, sizeof(Stack_t));
    if (!proc->funcStack)
    {
        return CreateError(ecCantAllocateMemory, "proc->funcStack");
    }

    StackDebugData *funcStkDebugData = (StackDebugData *)calloc(1, sizeof(StackDebugData));
    if (!funcStkDebugData)
    {
        return CreateError(ecCantAllocateMemory, "proc->funcStack->debugData");
    }
    funcStkDebugData->creationLine = __LINE__ - 13;
    funcStkDebugData->creationFile = __FILE__;
    funcStkDebugData->creationFunction = __FUNCTION__;
    funcStkDebugData->name = "proc->funcStack";

    RETURN_ERROR_IF_FAIL(StackInit(proc->funcStack, initFuncStackCap, funcStkDebugData));

    return CreateSuccess();
}

Error _InitProcRam(Processor *const proc, const size_t initRamSize)
{
    ASSERT(proc);

    proc->ram = (Ram *)calloc(1, sizeof(Ram));
    if (!proc->ram)
    {
        return CreateError(ecCantAllocateMemory, "proc->ram");
    }

    proc->ram->size = initRamSize;
    proc->ram->data = (Digit_t *)calloc(initRamSize, sizeof(Digit_t));
    if (!proc->ram->data)
    {
        return CreateError(ecCantAllocateMemory, "proc->ram->data");
    }

    return CreateSuccess();
}

void FreeProcessorData(Processor *const proc)
{
    ASSERT(proc);

    ASSERT(proc->byteCode);
    free(proc->byteCode);

    ASSERT(proc->stack);
    ASSERT(proc->stack->data);
    ASSERT(proc->stack->debugData);
    StackFreeData(proc->stack);
    free(proc->stack->debugData);
    free(proc->stack);

    ASSERT(proc->funcStack);
    ASSERT(proc->funcStack->data);
    ASSERT(proc->funcStack->debugData);
    StackFreeData(proc->funcStack);
    free(proc->funcStack->debugData);
    free(proc->funcStack);

    ASSERT(proc->ram);
    ASSERT(proc->ram->data);
    free(proc->ram->data);
    free(proc->ram);

    return;
}

Error RunCommand(Processor *const proc, CommandTable *const table)
{
    ASSERT(proc);

    byte_t commandCode = proc->byteCode[proc->pc];
    fprintf(stderr, "DEBUG: Executing %X\n", commandCode);

    ASSERT(commandCode < table->size);
    proc->pc++;

    CommandFunc func = table->data[commandCode];
    if (func == NULL)
    {
        char ctx[cMaxLine] = {};
        snprintf(ctx, cMaxLine - 1, "%d", commandCode); // FIXME HARDCODED
        return CreateError(ecUnknownProcessorInstruction, ctx);
    }

    Error error = func(proc); // Call certain command function

    //     case pcPUSHr:
    //         RETURN_ERROR_IF_FAIL(GetRegisterById(proc->byteCode[proc->pc], proc, &tmpReg));
    //         ASSERT(tmpReg);
    //         RETURN_ERROR_IF_FAIL(StackPush(proc->stack, *tmpReg));
    //         proc->pc += sizeof(Digit_t); // TODO PUSHr arg can fit in 1 byte
    //         break;

    return error;
}

Error GetRegisterById(const byte_t id, Processor *const proc, Digit_t **result)
{
    ASSERT(proc);
    Error error = CreateSuccess();

    switch (id)
    {
    case 0x0:
        *result = &proc->regA;
        break;

    case 0x1:
        *result = &proc->regB;
        break;

    case 0x2:
        *result = &proc->regC;
        break;

    case 0x3:
        *result = &proc->regD;
        break;

    default:
        char buf[3] = {};
        snprintf(buf, 3, "%02X", id);
        error = CreateError(ecUnknownRegisterId, buf);
        return error;
    }
    return error;
}
