#include "../h/proc/commandTable.h"
#include "../h/commandCodes.h"
#include "../h/errorHandle.h"
#include "../h/proc/commands.h"
#include <cstdlib>

Error InitializeCommandTable(CommandTable *const table)
{
    ASSERT(table);

    table->size = cCommandTableSize;
    table->data = (CommandFunc *)calloc(table->size, sizeof(CommandFunc));
    if (!table->data)
    {
        return CreateError(ecCantAllocateMemory, "command table data");
    }

    return CreateSuccess();
}

void FreeCommandTableData(CommandTable *const table)
{
    ASSERT(table);

    free(table->data);
    return;
}

void LoadCommandTable(CommandTable *const table)
{
    ASSERT(table);

    table->data[pcHLT] = _Halt;

    table->data[pcPUSHc] = _Pushc;
    table->data[pcPUSHr] = _Pushr;
    table->data[pcPUSHm] = _Pushm;
    table->data[pcPUSHrc] = _Pushrc;
    table->data[pcPUSHmc] = _Pushmc;
    table->data[pcPUSHmr] = _Pushmr;
    table->data[pcPUSHmrc] = _Pushmrc;

    table->data[pcPOPc] = _Popc;
    table->data[pcPOPr] = _Popr;
    table->data[pcPOPm] = _Popm;
    table->data[pcPOPrc] = _Poprc;
    table->data[pcPOPmc] = _Popmc;
    table->data[pcPOPmr] = _Popmr;
    table->data[pcPOPmrc] = _Popmrc;

    table->data[pcADD] = _Add;
    table->data[pcSUB] = _Sub;
    table->data[pcMUL] = _Mul;
    table->data[pcDIV] = _Div;
    table->data[pcOUT] = _Out;
    table->data[pcJMP] = _Jmp;
    table->data[pcJA] = _Ja;
    table->data[pcJAE] = _Jae;
    table->data[pcJB] = _Jb;
    table->data[pcJBE] = _Jbe;
    table->data[pcJE] = _Je;
    table->data[pcJNE] = _Jne;
    table->data[pcCALL] = _Call;
    table->data[pcRET] = _Ret;

    return;
}
