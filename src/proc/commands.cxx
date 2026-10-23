#include "../h/proc/commands.h"
#include "../h/proc/proc.h"
#include <cerrno>
#include <cstdio>
#include <cstring>

Error _Halt(Processor *proc)
{
    ASSERT(proc);

    proc->halted = true;
    return CreateSuccess();
}

Error _Push(Processor *proc)
{
    ASSERT(proc);

    Digit_t digitToPush = 0;

    ASSERT(proc->pc >= 1);
    byte_t commandCode = proc->byteCode[proc->pc - 1];

    // 8 4 2 1 01
    // 0 0 0 0 0001
    // | | |
    // m r c

    if (commandCode & 0x20)
    {
        Digit_t tmpVar = 0;
        memcpy(&tmpVar, proc->byteCode + proc->pc, sizeof(Digit_t));

        digitToPush += tmpVar;
        proc->pc += sizeof(Digit_t);
    }
    if (commandCode & 0x40)
    {
        Digit_t *regPtr = 0;
        RETURN_ERROR_IF_FAIL(GetRegisterById(proc->byteCode[proc->pc], proc, &regPtr));

        digitToPush += *regPtr;
        proc->pc += sizeof(Digit_t);
    }
    if (commandCode & 0x80)
    {
        if (digitToPush < 0 || (unsigned)digitToPush >= proc->ram->size)
        {
            char ctx[cMaxLine] = {};
            snprintf(ctx, cMaxLine - 1, "%d", digitToPush); // FIXME HARDCODED
            return CreateError(ecRamIndexOutOfRange, ctx);
        }

        digitToPush = proc->ram->data[digitToPush];
        proc->pc += sizeof(Digit_t);
    }

    return StackPush(proc->stack, digitToPush);
}

Error _Pop(Processor *proc)
{
    ASSERT(proc);

    Digit_t *addressToPop = proc->ram->data;
    Digit_t *regAddress = NULL;
    Digit_t poppedElement = 0;

    ASSERT(proc->pc >= 1);
    byte_t commandCode = proc->byteCode[proc->pc - 1];

    RETURN_ERROR_IF_FAIL(StackPop(proc->stack, &poppedElement));

    // 8 4 2 1 02
    // 0 0 0 0 0010
    // | | |
    // m r c

    if (commandCode & 0x40)
    {
        RETURN_ERROR_IF_FAIL(GetRegisterById(proc->byteCode[proc->pc], proc, &regAddress));
        proc->pc += sizeof(Digit_t);
    }

    if (commandCode & 0x80)
    {
        if (regAddress)
        {
            addressToPop += *regAddress;
        }

        if (commandCode & 0x20)
        {
            Digit_t tmpVar = 0;
            memcpy(&tmpVar, proc->byteCode + proc->pc, sizeof(Digit_t));

            addressToPop += tmpVar;
            proc->pc += sizeof(Digit_t);
        }
    }
    else
    {
        addressToPop = regAddress;
    }

    *addressToPop = poppedElement;

    return CreateSuccess();
}

Error _Add(Processor *proc)
{
    ASSERT(proc);

    Digit_t tmpVar1 = 0;
    Digit_t tmpVar2 = 0;

    RETURN_ERROR_IF_FAIL(StackPop(proc->stack, &tmpVar1));
    RETURN_ERROR_IF_FAIL(StackPop(proc->stack, &tmpVar2));
    RETURN_ERROR_IF_FAIL(StackPush(proc->stack, tmpVar1 + tmpVar2));

    return CreateSuccess();
}

Error _Sub(Processor *proc)
{
    ASSERT(proc);

    Digit_t tmpVar1 = 0;
    Digit_t tmpVar2 = 0;

    RETURN_ERROR_IF_FAIL(StackPop(proc->stack, &tmpVar1));
    RETURN_ERROR_IF_FAIL(StackPop(proc->stack, &tmpVar2));
    RETURN_ERROR_IF_FAIL(StackPush(proc->stack, tmpVar1 - tmpVar2));

    return CreateSuccess();
}

Error _Mul(Processor *proc)
{
    ASSERT(proc);

    Digit_t tmpVar1 = 0;
    Digit_t tmpVar2 = 0;

    RETURN_ERROR_IF_FAIL(StackPop(proc->stack, &tmpVar1));
    RETURN_ERROR_IF_FAIL(StackPop(proc->stack, &tmpVar2));
    RETURN_ERROR_IF_FAIL(StackPush(proc->stack, tmpVar1 * tmpVar2));

    return CreateSuccess();
}

Error _Div(Processor *proc)
{
    ASSERT(proc);

    Digit_t tmpVar1 = 0;
    Digit_t tmpVar2 = 0;

    RETURN_ERROR_IF_FAIL(StackPop(proc->stack, &tmpVar1));
    RETURN_ERROR_IF_FAIL(StackPop(proc->stack, &tmpVar2));
    RETURN_ERROR_IF_FAIL(StackPush(proc->stack, tmpVar1 / tmpVar2));

    return CreateSuccess();
}

Error _Out(Processor *proc)
{
    ASSERT(proc);

    Digit_t tmpVar = 0;

    RETURN_ERROR_IF_FAIL(StackPop(proc->stack, &tmpVar));
    if (fprintf(proc->outputfp, "%d\n", tmpVar) < 0) // FIXME hardcoded
    {
        return CreateError(TranslateErrnoCode(errno), "output file");
    }
    return CreateSuccess();
}

Error _Jmp(Processor *proc)
{
    ASSERT(proc);
    ASSERT(proc->byteCodeSize > proc->pc + sizeof(unsigned int));

    unsigned tmpAddress = 0;
    memcpy(&tmpAddress, proc->byteCode + proc->pc, sizeof(unsigned int));
    proc->pc = tmpAddress;

    return CreateSuccess();
}

Error _Ja(Processor *proc)
{
    ASSERT(proc);
    ASSERT(proc->byteCodeSize > proc->pc + 2 * sizeof(Digit_t) + sizeof(unsigned int));

    Digit_t tmpVar1 = 0;
    Digit_t tmpVar2 = 0;

    memcpy(&tmpVar1, proc->byteCode + proc->pc, sizeof(Digit_t));
    proc->pc += sizeof(Digit_t);
    memcpy(&tmpVar2, proc->byteCode + proc->pc, sizeof(Digit_t));
    proc->pc += sizeof(Digit_t);

    if (tmpVar1 > tmpVar2)
    {
        RETURN_ERROR_IF_FAIL(_Jmp(proc));
    }
    return CreateSuccess();
}
Error _Jae(Processor *proc)
{
    ASSERT(proc);
    ASSERT(proc->byteCodeSize > proc->pc + 2 * sizeof(Digit_t) + sizeof(unsigned int));

    Digit_t tmpVar1 = 0;
    Digit_t tmpVar2 = 0;

    memcpy(&tmpVar1, proc->byteCode + proc->pc, sizeof(Digit_t));
    proc->pc += sizeof(Digit_t);
    memcpy(&tmpVar2, proc->byteCode + proc->pc, sizeof(Digit_t));
    proc->pc += sizeof(Digit_t);

    if (tmpVar1 >= tmpVar2)
    {
        RETURN_ERROR_IF_FAIL(_Jmp(proc));
    }
    return CreateSuccess();
}
Error _Jb(Processor *proc)
{
    ASSERT(proc);
    ASSERT(proc->byteCodeSize > proc->pc + 2 * sizeof(Digit_t) + sizeof(unsigned int));

    Digit_t tmpVar1 = 0;
    Digit_t tmpVar2 = 0;

    memcpy(&tmpVar1, proc->byteCode + proc->pc, sizeof(Digit_t));
    proc->pc += sizeof(Digit_t);
    memcpy(&tmpVar2, proc->byteCode + proc->pc, sizeof(Digit_t));
    proc->pc += sizeof(Digit_t);

    if (tmpVar1 < tmpVar2)
    {
        RETURN_ERROR_IF_FAIL(_Jmp(proc));
    }
    return CreateSuccess();
}
Error _Jbe(Processor *proc)
{
    ASSERT(proc);
    ASSERT(proc->byteCodeSize > proc->pc + 2 * sizeof(Digit_t) + sizeof(unsigned int));

    Digit_t tmpVar1 = 0;
    Digit_t tmpVar2 = 0;

    memcpy(&tmpVar1, proc->byteCode + proc->pc, sizeof(Digit_t));
    proc->pc += sizeof(Digit_t);
    memcpy(&tmpVar2, proc->byteCode + proc->pc, sizeof(Digit_t));
    proc->pc += sizeof(Digit_t);

    if (tmpVar1 <= tmpVar2)
    {
        RETURN_ERROR_IF_FAIL(_Jmp(proc));
    }
    return CreateSuccess();
}
Error _Je(Processor *proc)
{
    ASSERT(proc);
    ASSERT(proc->byteCodeSize > proc->pc + 2 * sizeof(Digit_t) + sizeof(unsigned int));

    Digit_t tmpVar1 = 0;
    Digit_t tmpVar2 = 0;

    memcpy(&tmpVar1, proc->byteCode + proc->pc, sizeof(Digit_t));
    proc->pc += sizeof(Digit_t);
    memcpy(&tmpVar2, proc->byteCode + proc->pc, sizeof(Digit_t));
    proc->pc += sizeof(Digit_t);

    if (tmpVar1 == tmpVar2)
    {
        RETURN_ERROR_IF_FAIL(_Jmp(proc));
    }
    return CreateSuccess();
}
Error _Jne(Processor *proc)
{
    ASSERT(proc);
    ASSERT(proc->byteCodeSize > proc->pc + 2 * sizeof(Digit_t) + sizeof(unsigned int));

    Digit_t tmpVar1 = 0;
    Digit_t tmpVar2 = 0;

    memcpy(&tmpVar1, proc->byteCode + proc->pc, sizeof(Digit_t));
    proc->pc += sizeof(Digit_t);
    memcpy(&tmpVar2, proc->byteCode + proc->pc, sizeof(Digit_t));
    proc->pc += sizeof(Digit_t);

    if (tmpVar1 != tmpVar2)
    {
        RETURN_ERROR_IF_FAIL(_Jmp(proc));
    }
    return CreateSuccess();
}

Error _Call(Processor *proc)
{
    ASSERT(proc);
    ASSERT(proc->byteCodeSize > proc->pc + sizeof(unsigned int));

    RETURN_ERROR_IF_FAIL(StackPush(proc->funcStack, (StackElem_t)(proc->pc + sizeof(unsigned int))));
    _Jmp(proc);

    return CreateSuccess();
}

Error _Ret(Processor *proc)
{
    ASSERT(proc);

    Digit_t tmpVar = 0;

    RETURN_ERROR_IF_FAIL(StackPop(proc->funcStack, &tmpVar));
    proc->pc = (unsigned int)tmpVar;

    return CreateSuccess();
}
