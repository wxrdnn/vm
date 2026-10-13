#include "../h/proc/proc.h"
#include "../h/proc/instructions.h"
#include <cstdlib>

Error InitializeProcessor(Processor *const proc, const size_t byteCodeSize, const size_t initialStackCap)
{
    ASSERT(proc);
    Error error = CreateSuccess();

    proc->byteCode = (byte_t *)calloc(byteCodeSize, sizeof(byte_t));
    if (!proc->byteCode)
    {
        error = CreateError(ecCantAllocateMemory, "proc.byteCode");
        return error;
    }

    proc->byteCodeSize = byteCodeSize * sizeof(byte_t);
    proc->pc = 0;

    proc->stack = (Stack_t *)calloc(1, sizeof(Stack_t));
    if (!proc->stack)
    {
        error = CreateError(ecCantAllocateMemory, "proc.stack");
        return error;
    }

    StackDebugData *stkDebugData = (StackDebugData *)calloc(1, sizeof(StackDebugData));
    if (!stkDebugData)
    {
        error = CreateError(ecCantAllocateMemory, "stkDebugData");
        return error;
    }
    stkDebugData->creationLine = __LINE__ - 13;
    stkDebugData->creationFile = __FILE__;
    stkDebugData->creationFunction = __FUNCTION__;
    stkDebugData->name = "proc.stack";

    RETURN_ERROR_IF_FAIL(StackInit(proc->stack, initialStackCap, stkDebugData));

    proc->regA = 0;
    proc->regB = 0;
    proc->regC = 0;
    proc->regD = 0;

    return error;
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

    return;
}

Error RunCommand(Processor *const proc)
{
    ProcessorCommand commandCode = (ProcessorCommand)proc->byteCode[proc->pc]; // cast 1 byte to int

    switch (commandCode) {}
}
