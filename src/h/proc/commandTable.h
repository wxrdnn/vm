#ifndef PROC_COMMAND_TABLE_H

#define PROC_COMMAND_TABLE_H

#include "../errorHandle.h"
#include "proc.h"

typedef Error (*CommandFunc)(Processor *);

struct CommandTable
{
    CommandFunc *data;
    size_t size;
};

#endif
