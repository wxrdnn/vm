#ifndef STACK_H

#define STACK_H

#define USE_CANARY_PROT
#define USE_HASH_PROT

#include "debug.h"
#include "errorHandle.h"
#include <cstdint>
#include <stddef.h>

const size_t cStackCapMultiplierOnExtend = 2;
const int cIntPoisonValue = (int)0xB1BAB0BA;
const uint64_t cCanaryValue = 0xB00B1E5;

enum StackErrorCode
{
    secSuccess = 0,
    secNullStackPointer,
    secNullDataPointer,
    secNullDebugDataPointer,
    secIncorrectDebugData,
    secSizeLargerThanCapacity,
    secPoisonValueDetected,
    secNullNameInDebugData,
    secNullCreationFileInDebugData,
    secNullCreationFunctionInDebugData,
    secTopCanaryChanged,
    secBottomCanaryChanged,
    secStackHashChangedUnexpected,
};

struct StackDebugData
{
    const char *name;
    const char *creationFile;
    const char *creationFunction;
    size_t creationLine;
};

typedef int StackElem_t; // TODO remove it

struct Stack_t
{
#ifdef USE_CANARY_PROT
    uint64_t canaryTop;
#endif

    // True stack data
    StackElem_t *data;
    size_t capacity;
    size_t size;
    // End

    ONDEBUG(StackDebugData *debugData);

#ifdef USE_HASH_PROT
    uint64_t hash;
#endif

#ifdef USE_CANARY_PROT
    uint64_t canaryBottom;
#endif
};

#define VERIFY_STACK(__stk)                                                                                            \
    {                                                                                                                  \
        fprintf(stderr,                                                                                                \
                __YELLOW "DEBUG: _StackVerify() called in %s() at %s:%d\n" __RESET,                                    \
                __FUNCTION__,                                                                                          \
                __FILE__,                                                                                              \
                __LINE__);                                                                                             \
        StackErrorCode __stkErrCode = _StackVerify(__stk);                                                             \
        if (__stkErrCode != secSuccess)                                                                                \
        {                                                                                                              \
            fprintf(                                                                                                   \
                stderr, __RED "ERROR: StackVerify(%s) failed, StackErrorCode: %d.\n" __RESET, #__stk, __stkErrCode);   \
        }                                                                                                              \
        ASSERT(__stkErrCode == secSuccess);                                                                            \
    }

#define STACK_DUMP(__stk)                                                                                              \
    {                                                                                                                  \
        fprintf(stderr,                                                                                                \
                __YELLOW "DEBUG: _StackDump() called in %s() at %s:%d\n" __RESET,                                      \
                __FUNCTION__,                                                                                          \
                __FILE__,                                                                                              \
                __LINE__);                                                                                             \
        _StackDump(__stk);                                                                                             \
    }

StackErrorCode _StackVerify(const Stack_t *const stk); // Call via VERIFY_STACK

StackErrorCode StackDebugDataVerify(const StackDebugData *const data);

Error StackInit(Stack_t *const stk, const size_t capacity ONDEBUG(, StackDebugData *const debugData));

Error StackPush(Stack_t *const stk, StackElem_t element);

StackElem_t StackPop(Stack_t *const stk, Error *const error);

void StackFreeData(Stack_t *const stk);

void _StackDump(const Stack_t *const stk); // Call via STACK_DUMP

Error StackExtend(Stack_t *stk);

Error StackResize(Stack_t *stk, size_t newCapacity);

Error StackDebugDataInit(StackDebugData *const stkDebugData, const char *const name, const char *const creationFile,
                         const char *const creationFunction, const size_t creationLine);

uint64_t CalcStackHash(const Stack_t *const stk);

void UpdateStackHash(Stack_t *const stk);

#endif
