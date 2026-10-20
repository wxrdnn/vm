#ifndef PROC_COMMANDS_H

#define PROC_COMMANDS_H

#include "proc.h"

Error _Halt(Processor *proc);

Error _Pushc(Processor *proc);
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
