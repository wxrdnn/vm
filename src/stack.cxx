#include "h/stack.h"
#include "h/colors.h"
#include "h/debug.h"
#include "h/errorHandle.h"
#include "h/hash.h"
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

StackErrorCode _StackVerify(const Stack_t *const stk)
{
    if (!stk)
    {
        STACK_DUMP(stk);
        return secNullStackPointer;
    }

    if (!stk->data)
    {
        STACK_DUMP(stk);
        return secNullDataPointer;
    }

    ONDEBUG(

        if (stk->canaryTop != cCanaryValue) {
            STACK_DUMP(stk);
            return secTopCanaryChanged;
        }

        if (stk->canaryBottom != cCanaryValue) {
            STACK_DUMP(stk);
            return secBottomCanaryChanged;
        }

        if (!stk->debugData) {
            STACK_DUMP(stk);
            return secNullDebugDataPointer;
        }

        StackErrorCode debugDataCode = StackDebugDataVerify(stk->debugData);

        if (debugDataCode != secSuccess) {
            STACK_DUMP(stk);
            return debugDataCode;
        }

    )

    if (stk->size > stk->capacity)
    {
        STACK_DUMP(stk);
        return secSizeLargerThanCapacity;
    }

#ifdef USE_HASH_PROT
    uint64_t hash = CalcStackHash(stk);
    // fprintf(stderr,
    //         "DEBUG: Comparing hashes in _StackVerify. Stored hash: <0x%lx>, Calculated hash: <0x%lx>\n",
    //         stk->hash,
    //         hash);
    if (hash != stk->hash)
    {
        STACK_DUMP(stk);
        return secStackHashChangedUnexpected;
    }
#endif

    // TODO Poison value detection

    return secSuccess;
}

StackErrorCode StackDebugDataVerify(const StackDebugData *const data)
{
    if (!data)
    {
        return secNullDebugDataPointer;
    }

    if (!data->name)
    {
        return secNullNameInDebugData;
    }

    if (!data->creationFile)
    {
        return secNullCreationFileInDebugData;
    }

    if (!data->creationFunction)
    {
        return secNullCreationFunctionInDebugData;
    }

    return secSuccess;
}

Error StackInit(Stack_t *const stk, const size_t capacity ONDEBUG(, StackDebugData *const debugData))
{
    ASSERT(stk);
    ONDEBUG(ASSERT(debugData));

    Error error = CreateSuccess();
    if (!(stk))
    {
        return error = CreateError(ecCantAllocateMemory, "Stack_t structure");
    }

    (stk)->data = (StackElem_t *)calloc(capacity, sizeof(StackElem_t));
    if (!(stk)->data)
    {
        return error = CreateError(ecCantAllocateMemory, "Stack_t element");
    }

    (stk)->capacity = capacity;
    (stk)->size = 0;
    ONDEBUG(stk->debugData = debugData;)

#ifdef USE_CANARY_PROT
    stk->canaryTop = cCanaryValue;
    stk->canaryBottom = cCanaryValue;
#endif

    UpdateStackHash(stk);
    VERIFY_STACK(stk);
    return error;
}

Error StackPush(Stack_t *const stk, StackElem_t element)
{
    VERIFY_STACK(stk);
    Error error = CreateSuccess();

    // error.exitCode = ecFileIsBusy;
    // return error;

    if (stk->size == stk->capacity)
    {
        error = StackExtend(stk);

        if (IsFail(&error))
        {
            return error;
        }
    }

    ASSERT(stk->size < stk->capacity);

    stk->data[stk->size++] = element;

    UpdateStackHash(stk);
    VERIFY_STACK(stk);
    return error;
}

StackElem_t StackPop(Stack_t *const stk, Error *const error)
{
    VERIFY_STACK(stk);

    *error = CreateSuccess();

    // TODO size check before pop
    StackElem_t poppedElement = stk->data[stk->size--]; // TODO Error handling

    // if (stl->capacity / stk->size > cStackCapMultipluer)
    // TODO Shrink if stack is smaller than its 2 extensions

    UpdateStackHash(stk);
    VERIFY_STACK(stk);
    return poppedElement;
}

void StackFreeData(Stack_t *const stk)
{
    ASSERT(_StackVerify(stk) == secSuccess);

    free(stk->data);
    // ONDEBUG(free(stk->debugData));

    return;
}

// lang-format off
void _StackDump(const Stack_t *const stk) // Disables by NDEBUG
{
    ONDEBUG(

        if (stk == NULL) {
            fprintf(stderr, __RED "ERROR: StackDump() got NULL instead of Stack_t pointer!\n" __RESET);
            return;
        }

        StackErrorCode errCode = StackDebugDataVerify(stk->debugData);
        if (errCode != secSuccess) { // TODO expand it
            fprintf(stderr,
                    __RED "ERROR: StackDump() got corrupted StackDebugData in Stack_t! StackErrorCode = %d\n" __RESET,
                    errCode);
            return;
        }

        fprintf(stderr, __YELLOW "DEBUG: ------------------------------------------------------\n" __RESET);
        fprintf(stderr,
                __YELLOW "DEBUG: Begin dump of Stack_t named \"%s\" at [%p] created in %s() at %s:%lu.\n" __RESET,
                stk->debugData->name,
                stk,
                stk->debugData->creationFunction,
                stk->debugData->creationFile,
                stk->debugData->creationLine);

        if (stk->data == NULL) {
            fprintf(
                stderr,
                __RED
                "ERROR: StackDump() got NULL data pointer in Stack_t!\n" __RESET); // TODO still print size and capacity
            return;
        }

        fprintf(stderr, __YELLOW);
        fprintf(stderr, "DEBUG: Stack_t %s\n", stk->debugData->name);
        fprintf(stderr, "DEBUG: {\n");
        fprintf(stderr, "DEBUG:     capacity = %lu\n", stk->capacity);
        fprintf(stderr, "DEBUG:     size = %lu\n", stk->size);
        fprintf(stderr, "DEBUG:     data at [%p]\n", stk->data);
        fprintf(stderr, "DEBUG:     {\n");

        for (size_t i = 0; i < stk->capacity; ++i) {
            fprintf(stderr,
                    "DEBUG:         data[%lu] at [%p] = %d",
                    i,
                    stk->data + i,
                    stk->data[i]); // TODO fix hardcoded StackElem_t format

            if (stk->data[i] == cIntPoisonValue)
            {
                fprintf(stderr, "\t\t <- Poison value");
            }

            fprintf(stderr, "\n");
        }

        fprintf(stderr, "DEBUG:     }\n");
        fprintf(stderr, "DEBUG: }\n");
        fprintf(stderr, __RESET);

        fprintf(stderr,
                __YELLOW "DEBUG: End dump of Stack_t named \"%s\" at [%p] created in %s() at %s:%lu.\n" __RESET,
                stk->debugData->name,
                stk,
                stk->debugData->creationFunction,
                stk->debugData->creationFile,
                stk->debugData->creationLine);

        fprintf(stderr, __YELLOW "DEBUG: ------------------------------------------------------\n" __RESET);)
}
// lang-format on

Error StackExtend(Stack_t *stk)
{
    VERIFY_STACK(stk);

    size_t newCapacity = stk->capacity * cStackCapMultiplierOnExtend;
    size_t oldCapacity = stk->capacity;
    Error error = StackResize(stk, newCapacity);
    // fprintf(stderr,
    //         "DEBUG: Values before memset: oldCapacity = %lu, newCapacity = %lu, cIntPoisonValue = %d\n",
    //         oldCapacity,
    //         newCapacity,
    //         cIntPoisonValue);
    // Instead of memset, that sets values by 1 byte: 0xB1BAB0BA -> 0xBABABABA
    // memset(stk->data + oldCapacity, cIntPoisonValue, newCapacity - oldCapacity);
    for (size_t i = oldCapacity; i < newCapacity; ++i)
    {
        stk->data[i] = cIntPoisonValue;
    }

    UpdateStackHash(stk);
    VERIFY_STACK(stk);
    return error;
}

Error StackResize(Stack_t *stk, size_t newCapacity)
{
    VERIFY_STACK(stk);

    Error error = CreateSuccess();

    stk->data = (StackElem_t *)realloc(stk->data, newCapacity * sizeof(StackElem_t));
    if (!stk->data)
    {
        return error = CreateError(ecCantAllocateMemory, "Stack_t data extention");
    }
    ONDEBUG(fprintf(stderr,
                    __BLUE "DEBUG: Resized Stack_t %s. Old size: %lu -> new size: %lu.\n" __RESET,
                    stk->debugData->name,
                    stk->capacity,
                    newCapacity));
    stk->capacity = newCapacity;

    UpdateStackHash(stk);
    VERIFY_STACK(stk);
    return error;
}

Error StackDebugDataInit(StackDebugData *const stkDebugData, const char *const name, const char *const creationFile,
                         const char *const creationFunction, const size_t creationLine)
{
    ASSERT(stkDebugData);

    Error error = CreateSuccess();
    // if (!(stkDebugData))
    // {
    //     error = CreateError(ecCantAllocateMemory, "Stack_t debug data");
    //     return error;
    // }

    (stkDebugData)->name = name;
    (stkDebugData)->creationFile = creationFile;
    (stkDebugData)->creationFunction = creationFunction;
    (stkDebugData)->creationLine = creationLine;

    return error;
}

uint64_t CalcStackHash(const Stack_t *const stk)
{
    const char *startHashAt = (const char *)stk;

#ifdef USE_CANARY_PROT
    startHashAt += sizeof(stk->canaryTop);
#endif
    // clang-format off
    size_t nBytesToHash =
        sizeof(stk->data)
        + sizeof(stk->capacity)
        + sizeof(stk->size)
    ONDEBUG(+sizeof(stk->debugData));
    // clang-format on

    uint64_t hashOfStack = CalcHash(startHashAt, nBytesToHash);
    uint64_t hashOfStackData = CalcHash(stk->data, stk->capacity * sizeof(*stk->data));
    uint64_t hashOfStackDebugData = CalcHash(stk->debugData, sizeof(*stk->debugData));

    return hashOfStack + hashOfStackData + hashOfStackDebugData;
}

void UpdateStackHash(Stack_t *const stk)
{
    ASSERT(stk);

    stk->hash = CalcStackHash(stk);
    // fprintf(stderr, "DEBUG: Called UpdateStackHash(). New hash: <0x%lx>\n", stk->hash);
}
