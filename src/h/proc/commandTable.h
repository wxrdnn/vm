#ifndef PROC_COMMAND_TABLE_H

#define PROC_COMMAND_TABLE_H

#include "../errorHandle.h"
#include "../proc/types.h"

Error InitializeCommandTable(CommandTable *const table);

void FreeCommandTableData(CommandTable *const table);

void LoadCommandTable(CommandTable *const table);

#endif
