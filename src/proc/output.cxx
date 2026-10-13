#include "../h/proc/output.h"
#include <cstdio>

void DumpByte_tArrInHex(const byte_t *const arr, const size_t size)
{
    fprintf(stderr, "DEBUG: Array dump in hex: [");

    for (size_t i = 0; i < size; ++i)
    {
        fprintf(stderr, "%02hhX", arr[i]);
        if (i < size - 1)
        {
            fprintf(stderr, ", ");
        }
    }

    fprintf(stderr, "]\n");
}
