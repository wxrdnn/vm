#include "h/hash.h"

uint64_t CalcHash(const void *const data, size_t size) // FNV-1a
{
    uint64_t hash = cFNVOffsetBasis;
    const unsigned char *p = (const unsigned char *)data;

    for (; (size_t)p - (size_t)data < size; p++)
    {
        hash ^= *p;
        hash *= cFNVPrime;
    }

    return hash;
}
