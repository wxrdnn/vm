#ifndef DEBUG_H

#define DEBUG_H

#include "colors.h"
#include <stdio.h>
#include <stdlib.h>

#ifndef NDEBUG

#define ASSERT(statement)                                                                                              \
    {                                                                                                                  \
        if (!(statement))                                                                                              \
        {                                                                                                              \
            fprintf(stderr,                                                                                            \
                    __RED "Assertion \'" #statement "\' failed at " __RESET __BLUE "%s:%d.\n" __RESET,                 \
                    __FILE_NAME__,                                                                                     \
                    __LINE__);                                                                                         \
            abort();                                                                                                   \
        }                                                                                                              \
    }

#define ONDEBUG(...) __VA_ARGS__

#else

#define ASSERT(statement) ((void)0)
#define ONDEBUG(...)

#endif

#endif
