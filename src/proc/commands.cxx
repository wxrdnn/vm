#include "../h/proc/commands.h"
#include <cerrno>
#include <cstring>

Error _Halt(Processor *proc)
{
    ASSERT(proc);

    proc->halted = true;
    return CreateSuccess();
}

Error _Pushc(Processor *proc)
{
    ASSERT(proc);
    Digit_t tmpVar = 0;

    memcpy(&tmpVar, proc->byteCode + proc->pc, sizeof(Digit_t));

    RETURN_ERROR_IF_FAIL(StackPush(proc->stack, tmpVar));
    proc->pc += sizeof(Digit_t);

    return CreateSuccess();
}

Error _Pushr(Processor *proc)
{
    return CreateSuccess();
}
Error _Pushm(Processor *proc)
{
    return CreateSuccess();
}
Error _Pushrc(Processor *proc)
{
    return CreateSuccess();
}
Error _Pushmc(Processor *proc)
{
    return CreateSuccess();
}
Error _Pushmr(Processor *proc)
{
    return CreateSuccess();
}
Error _Pushmrc(Processor *proc)
{
    return CreateSuccess();
}

Error _Popc(Processor *proc)
{
    return CreateSuccess();
}
Error _Popr(Processor *proc)
{
    return CreateSuccess();
}
Error _Popm(Processor *proc)
{
    return CreateSuccess();
}
Error _Poprc(Processor *proc)
{
    return CreateSuccess();
}
Error _Popmc(Processor *proc)
{
    return CreateSuccess();
}
Error _Popmr(Processor *proc)
{
    return CreateSuccess();
}
Error _Popmrc(Processor *proc)
{
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
    if (fprintf(stdout, "%d\n", tmpVar) < 0) // FIXME hardcoded
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

    RETURN_ERROR_IF_FAIL(StackPush(proc->funcStack, proc->pc + sizeof(unsigned int)));
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
