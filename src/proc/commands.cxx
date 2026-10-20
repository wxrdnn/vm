#include "../h/proc/commands.h"
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

Error _Pushr(Processor *proc);
Error _Pushm(Processor *proc);
Error _Pushrc(Processor *proc);
Error _Pushmc(Processor *proc);
Error _Pushmr(Processor *proc);
Error _Pushmrc(Processor *proc);

Error _Popc(Processor *proc);
Error _Popr(Processor *proc);
Error _Popm(Processor *proc);
Error _Poprc(Processor *proc);
Error _Popmc(Processor *proc);
Error _Popmr(Processor *proc);
Error _Popmrc(Processor *proc);

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
Error _Sub(Processor *proc);
Error _Mul(Processor *proc);
Error _Div(Processor *proc);
Error _Out(Processor *proc);
Error _Jmp(Processor *proc);
Error _Ja(Processor *proc);
Error _Jae(Processor *proc);
Error _Jb(Processor *proc);
Error _Jbe(Processor *proc);
Error _Je(Processor *proc);
Error _Jne(Processor *proc);
Error _Call(Processor *proc);
Error _Ret(Processor *proc);
