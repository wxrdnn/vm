#ifndef PROC_COMMANDS_H

#define PROC_COMMANDS_H

#include "types.h"

Error _Halt(Processor *proc);

Error _Push(Processor *proc);
Error _Pop(Processor *proc);

Error _Add(Processor *proc);
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

#endif
