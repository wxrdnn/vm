#ifndef HASH_H

#define HASH_H

#include <cstddef>
#include <cstdint>

const uint64_t cFNVPrime = 0x100000001b3;
const uint64_t cFNVOffsetBasis = 0xcbf29ce484222325;

uint64_t CalcHash(const void *const data, size_t size); // FNV-1a

#endif
