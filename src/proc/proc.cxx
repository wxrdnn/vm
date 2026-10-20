#include "../h/proc/proc.h"
#include "../h/commandCodes.h"
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

void _Jump(Processor *const proc);

Error InitializeProcessor(Processor *const proc, const size_t byteCodeSize, const size_t initialStackCap,
                          const size_t initialFuncStackCap)
{
    ASSERT(proc);
    Error error = CreateSuccess();

    proc->halted = false;

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

    proc->funcStack = (Stack_t *)calloc(1, sizeof(Stack_t));
    if (!proc->funcStack)
    {
        error = CreateError(ecCantAllocateMemory, "proc.funcStack");
        return error;
    }

    StackDebugData *funcStkDebugData = (StackDebugData *)calloc(1, sizeof(StackDebugData));
    if (!funcStkDebugData)
    {
        error = CreateError(ecCantAllocateMemory, "proc.funcStkDebugData");
        return error;
    }
    funcStkDebugData->creationLine = __LINE__ - 13;
    funcStkDebugData->creationFile = __FILE__;
    funcStkDebugData->creationFunction = __FUNCTION__;
    funcStkDebugData->name = "proc.funcStack";

    RETURN_ERROR_IF_FAIL(StackInit(proc->funcStack, initialFuncStackCap, funcStkDebugData));

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

    ASSERT(proc->funcStack);
    ASSERT(proc->funcStack->data);
    ASSERT(proc->funcStack->debugData);
    StackFreeData(proc->funcStack);
    free(proc->funcStack->debugData);
    free(proc->funcStack);

    return;
}

Error RunCommand(Processor *const proc, FILE *input, FILE *output)
{
    ASSERT(proc);

    Error error = CreateSuccess();
    ProcessorCommand commandCode = (ProcessorCommand)proc->byteCode[proc->pc]; // cast 1 byte to int

    Digit_t tmpVar1 = 0;
    Digit_t tmpVar2 = 0;
    Digit_t *tmpReg = NULL;
    unsigned tmpAddress = 0;

    fprintf(stderr, "DEBUG: Executing %X\n", *(proc->byteCode + proc->pc));

    proc->pc++;
    switch (commandCode)
    {

    case pcADD:
        break;

    case pcSUB:
        RETURN_ERROR_IF_FAIL(StackPop(proc->stack, &tmpVar1));
        RETURN_ERROR_IF_FAIL(StackPop(proc->stack, &tmpVar2));
        RETURN_ERROR_IF_FAIL(StackPush(proc->stack, tmpVar2 - tmpVar1));
        break;

    case pcMUL:
        RETURN_ERROR_IF_FAIL(StackPop(proc->stack, &tmpVar1));
        RETURN_ERROR_IF_FAIL(StackPop(proc->stack, &tmpVar2));
        RETURN_ERROR_IF_FAIL(StackPush(proc->stack, tmpVar1 * tmpVar2));
        break;

    case pcDIV:
        RETURN_ERROR_IF_FAIL(StackPop(proc->stack, &tmpVar1));
        RETURN_ERROR_IF_FAIL(StackPop(proc->stack, &tmpVar2));
        RETURN_ERROR_IF_FAIL(StackPush(proc->stack, tmpVar1 / tmpVar2));
        break;

    case pcOUT:
        RETURN_ERROR_IF_FAIL(StackPop(proc->stack, &tmpVar1));
        if (fprintf(output, "%d\n", tmpVar1) < 0) // FIXME hardcoded
        {
            error = CreateError(TranslateErrnoCode(errno), "output file");
            return error;
        }
        break;

    case pcPUSHr:
        RETURN_ERROR_IF_FAIL(GetRegisterById(proc->byteCode[proc->pc], proc, &tmpReg));
        ASSERT(tmpReg);
        RETURN_ERROR_IF_FAIL(StackPush(proc->stack, *tmpReg));
        proc->pc += sizeof(Digit_t); // TODO PUSHr arg can fit in 1 byte
        break;

    case pcPOPr:
        RETURN_ERROR_IF_FAIL(GetRegisterById(proc->byteCode[proc->pc], proc, &tmpReg));
        ASSERT(tmpReg);
        RETURN_ERROR_IF_FAIL(StackPop(proc->stack, tmpReg));
        proc->pc += sizeof(Digit_t);
        break;

    case pcJMP:
        _Jump(proc);
        break;

    case pcJA:
        ASSERT(proc->byteCodeSize > proc->pc + 2 * sizeof(Digit_t) + sizeof(unsigned int));

        memcpy(&tmpVar1, proc->byteCode + proc->pc, sizeof(Digit_t));
        proc->pc += sizeof(Digit_t);
        memcpy(&tmpVar2, proc->byteCode + proc->pc, sizeof(Digit_t));
        proc->pc += sizeof(Digit_t);

        if (tmpVar1 > tmpVar2)
        {
            _Jump(proc);
        }
        break;

    case pcJAE:
        ASSERT(proc->byteCodeSize > proc->pc + 2 * sizeof(Digit_t) + sizeof(unsigned int));

        memcpy(&tmpVar1, proc->byteCode + proc->pc, sizeof(Digit_t));
        proc->pc += sizeof(Digit_t);
        memcpy(&tmpVar2, proc->byteCode + proc->pc, sizeof(Digit_t));
        proc->pc += sizeof(Digit_t);

        if (tmpVar1 >= tmpVar2)
        {
            _Jump(proc);
        }
        break;

    case pcJB:
        ASSERT(proc->byteCodeSize > proc->pc + 2 * sizeof(Digit_t) + sizeof(unsigned int));

        memcpy(&tmpVar1, proc->byteCode + proc->pc, sizeof(Digit_t));
        proc->pc += sizeof(Digit_t);
        memcpy(&tmpVar2, proc->byteCode + proc->pc, sizeof(Digit_t));
        proc->pc += sizeof(Digit_t);

        if (tmpVar1 < tmpVar2)
        {
            _Jump(proc);
        }
        break;

    case pcJBE:
        ASSERT(proc->byteCodeSize > proc->pc + 2 * sizeof(Digit_t) + sizeof(unsigned int));

        memcpy(&tmpVar1, proc->byteCode + proc->pc, sizeof(Digit_t));
        proc->pc += sizeof(Digit_t);
        memcpy(&tmpVar2, proc->byteCode + proc->pc, sizeof(Digit_t));
        proc->pc += sizeof(Digit_t);

        if (tmpVar1 <= tmpVar2)
        {
            _Jump(proc);
        }
        break;

    case pcJE:
        ASSERT(proc->byteCodeSize > proc->pc + 2 * sizeof(Digit_t) + sizeof(unsigned int));

        memcpy(&tmpVar1, proc->byteCode + proc->pc, sizeof(Digit_t));
        proc->pc += sizeof(Digit_t);
        memcpy(&tmpVar2, proc->byteCode + proc->pc, sizeof(Digit_t));
        proc->pc += sizeof(Digit_t);

        if (tmpVar1 == tmpVar2)
        {
            _Jump(proc);
        }
        break;

    case pcJNE:
        ASSERT(proc->byteCodeSize > proc->pc + 2 * sizeof(Digit_t) + sizeof(unsigned int));

        memcpy(&tmpVar1, proc->byteCode + proc->pc, sizeof(Digit_t));
        proc->pc += sizeof(Digit_t);
        memcpy(&tmpVar2, proc->byteCode + proc->pc, sizeof(Digit_t));
        proc->pc += sizeof(Digit_t);

        if (tmpVar1 != tmpVar2)
        {
            _Jump(proc);
        }
        break;

    case pcCALL: // FIXME CANT HANDLE addresses larger than 4 bytes !!!!
        ASSERT(proc->byteCodeSize > proc->pc + sizeof(unsigned int));

        // memcpy(&tmpAddress, proc->byteCode + proc->pc, sizeof(unsigned int));
        // fprintf(stderr, "DEBUG: pcCALL: Put address %u in function stack.\n", tmpAddress);
        RETURN_ERROR_IF_FAIL(StackPush(proc->funcStack, proc->pc + sizeof(unsigned int)));
        _Jump(proc);
        break;

    case pcRET:
        RETURN_ERROR_IF_FAIL(StackPop(proc->funcStack, &tmpVar1));
        proc->pc = (unsigned int)tmpVar1;
        break;

    case pcPOP:
        RETURN_ERROR_IF_FAIL(StackPop(proc->stack, &tmpVar1));
        break;

    default:
        char buf[3] = {};
        snprintf(buf, 3, "%02X", proc->byteCode[proc->pc - 1]);
        error = CreateError(ecUnknownProcessorInstruction, buf);
        return error;
    }

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

void _Jump(Processor *const proc)
{
    ASSERT(proc->byteCodeSize > proc->pc + sizeof(unsigned int));

    unsigned tmpAddress = 0;
    memcpy(&tmpAddress, proc->byteCode + proc->pc, sizeof(unsigned int));
    proc->pc = tmpAddress;
}
