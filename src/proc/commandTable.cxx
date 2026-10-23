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

    table->data[pcPUSHc] = _Push;
    table->data[pcPUSHr] = _Push;
    // table->data[pcPUSHm] = _Push;
    table->data[pcPUSHrc] = _Push;
    table->data[pcPUSHmc] = _Push;
    table->data[pcPUSHmr] = _Push;
    table->data[pcPUSHmrc] = _Push;

    table->data[pcPOP] = _Pop;
    table->data[pcPOPr] = _Pop;
    // table->data[pcPOPm] = _Pop;
    // table->data[pcPOPrc] = _Poprc;
    table->data[pcPOPmc] = _Pop;
    table->data[pcPOPmr] = _Pop;
    table->data[pcPOPmrc] = _Pop;

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
