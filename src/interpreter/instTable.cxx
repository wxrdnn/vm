#include "../h/interpreter/instTable.h"
#include "../h/commandCodes.h"
#include "../h/debug.h"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

Error CreateInstructionTable(const size_t size, InstructionTable *const tablePtr)
{
    ASSERT(tablePtr);

    Error error = CreateSuccess();
    tablePtr->data = (InstructionNode **)calloc(size, sizeof(InstructionNode *));
    tablePtr->size = size;

    if (!tablePtr)
    {
        error = CreateError(ecCantAllocateMemory, "Instruction table");
        return error;
    }

    return error;
}

InstructionNode *GetInstruction(const InstructionTable *const table, const uint64_t hash)
{
    ASSERT(table);

    size_t index = hash % table->size;
    InstructionNode *currentNode = table->data[index];

    if (!currentNode)
    {
        return NULL;
    }

    do
    {
        if (currentNode->hash == hash)
        {
            return currentNode;
        }
        else if (!currentNode->next)
        {
            return NULL;
        }
        else
        {
            currentNode = currentNode->next;
        }
    } while (currentNode->next != NULL);

    if (currentNode->hash == hash)
    {
        return currentNode;
    }

    return NULL;
}

Error AddInstruction(InstructionTable *const table, const uint64_t hash, const unsigned argsAmount, const byte_t code)
{
    ASSERT(table);

    Error error = CreateSuccess();
    size_t index = hash % table->size;

    InstructionNode *nodeFound = GetInstruction(table, hash);
    if (nodeFound)
    {
        printf("DEBUG: Instruction with hash <0x%lx> already exists with code %u.", hash, nodeFound->code);
        return error;
    }

    InstructionNode *newNode = (InstructionNode *)calloc(1, sizeof(InstructionNode)); // Create new node
    if (!newNode)
    {
        error = CreateError(ecCantAllocateMemory, "instruction node");
        return error;
    }

    newNode->hash = hash;
    newNode->argsAmount = argsAmount;
    newNode->code = code;
    newNode->next = NULL;

    if (table->data[index] == NULL) // Find place for new node
    {
        table->data[index] = newNode;
    }
    else
    {
        FindTailNode(table->data[index])->next = newNode;
    }

    return error;
}

void DestroyInstructionTable(InstructionTable *const tablePtr)
{
    ASSERT(tablePtr);
    ASSERT(tablePtr->data);

    for (size_t index = 0; index < tablePtr->size; index++)
    {
        if (tablePtr->data[index])
        {
            while (tablePtr->data[index]->next != NULL)
            {
                InstructionNode *firstBeforeTail = FindFirstBeforeTailNode(tablePtr->data[index]);

                ASSERT(firstBeforeTail->next);
                free(firstBeforeTail->next);
                firstBeforeTail->next = NULL;
            }
            free(tablePtr->data[index]);
        }
    }
    free(tablePtr->data);

    return;
}

InstructionNode *FindTailNode(InstructionNode *const start)
{
    ASSERT(start);

    InstructionNode *tailNode = start;
    while (tailNode->next != NULL)
    {
        tailNode = tailNode->next;
    }

    return tailNode;
}

InstructionNode *FindFirstBeforeTailNode(InstructionNode *const start)
{
    ASSERT(start);
    ASSERT(start->next);

    InstructionNode *beforeTailNode = start;

    while (beforeTailNode->next->next != NULL)
    {
        beforeTailNode = beforeTailNode->next;
    }

    return beforeTailNode;
}

uint64_t InstructionNameToHash(const char *const instructionName)
{
    ASSERT(instructionName);

    const uint64_t *p = (const uint64_t *)instructionName;
    return *p;
}

Error LoadInstructionTable(InstructionTable *const table)
{
    ASSERT(table);
    char *nameBuf[cMaxInstructionNameLength] = {};

    RETURN_ERROR_IF_FAIL(AddInstruction(table, InstructionNameToHash("HLT\0\0\0\0"), 0, (byte_t)pcHLT));

    RETURN_ERROR_IF_FAIL(AddInstruction(table, InstructionNameToHash("PUSHc\0\0"), 1, (byte_t)pcPUSHc));
    RETURN_ERROR_IF_FAIL(AddInstruction(table, InstructionNameToHash("PUSHr\0\0"), 1, (byte_t)pcPUSHr));
    RETURN_ERROR_IF_FAIL(AddInstruction(table, InstructionNameToHash("PUSHm\0\0"), 1, (byte_t)pcPUSHm));
    RETURN_ERROR_IF_FAIL(AddInstruction(table, InstructionNameToHash("PUSHrc\0"), 2, (byte_t)pcPUSHrc));
    RETURN_ERROR_IF_FAIL(AddInstruction(table, InstructionNameToHash("PUSHmc\0"), 1, (byte_t)pcPUSHmc));
    RETURN_ERROR_IF_FAIL(AddInstruction(table, InstructionNameToHash("PUSHmr\0"), 1, (byte_t)pcPUSHmr));
    RETURN_ERROR_IF_FAIL(AddInstruction(table, InstructionNameToHash("PUSHmrc"), 2, (byte_t)pcPUSHmrc));

    RETURN_ERROR_IF_FAIL(AddInstruction(table, InstructionNameToHash("POPc\0\0\0"), 1, (byte_t)pcPOPc));
    RETURN_ERROR_IF_FAIL(AddInstruction(table, InstructionNameToHash("POPr\0\0\0"), 1, (byte_t)pcPOPr));
    RETURN_ERROR_IF_FAIL(AddInstruction(table, InstructionNameToHash("POPm\0\0\0"), 1, (byte_t)pcPOPm));
    RETURN_ERROR_IF_FAIL(AddInstruction(table, InstructionNameToHash("POPrc\0\0"), 2, (byte_t)pcPOPrc));
    RETURN_ERROR_IF_FAIL(AddInstruction(table, InstructionNameToHash("POPmc\0\0"), 1, (byte_t)pcPOPmc));
    RETURN_ERROR_IF_FAIL(AddInstruction(table, InstructionNameToHash("POPmr\0\0"), 1, (byte_t)pcPOPmr));
    RETURN_ERROR_IF_FAIL(AddInstruction(table, InstructionNameToHash("POPmrc\0"), 2, (byte_t)pcPOPmrc));

    RETURN_ERROR_IF_FAIL(AddInstruction(table, InstructionNameToHash("ADD\0\0\0\0"), 0, (byte_t)pcADD));
    RETURN_ERROR_IF_FAIL(AddInstruction(table, InstructionNameToHash("SUB\0\0\0\0"), 0, (byte_t)pcSUB));
    RETURN_ERROR_IF_FAIL(AddInstruction(table, InstructionNameToHash("MUL\0\0\0\0"), 0, (byte_t)pcMUL));
    RETURN_ERROR_IF_FAIL(AddInstruction(table, InstructionNameToHash("DIV\0\0\0\0"), 0, (byte_t)pcDIV));
    RETURN_ERROR_IF_FAIL(AddInstruction(table, InstructionNameToHash("OUT\0\0\0\0"), 0, (byte_t)pcOUT));
    RETURN_ERROR_IF_FAIL(AddInstruction(table, InstructionNameToHash("JMP\0\0\0\0"), 1, (byte_t)pcJMP));
    RETURN_ERROR_IF_FAIL(AddInstruction(table, InstructionNameToHash("JA\0\0\0\0\0"), 3, (byte_t)pcJA));
    RETURN_ERROR_IF_FAIL(AddInstruction(table, InstructionNameToHash("JAE\0\0\0\0"), 3, (byte_t)pcJAE));
    RETURN_ERROR_IF_FAIL(AddInstruction(table, InstructionNameToHash("JB\0\0\0\0\0"), 3, (byte_t)pcJB));
    RETURN_ERROR_IF_FAIL(AddInstruction(table, InstructionNameToHash("JBE\0\0\0\0"), 3, (byte_t)pcJBE));
    RETURN_ERROR_IF_FAIL(AddInstruction(table, InstructionNameToHash("JE\0\0\0\0\0"), 3, (byte_t)pcJE));
    RETURN_ERROR_IF_FAIL(AddInstruction(table, InstructionNameToHash("JNE\0\0\0\0"), 3, (byte_t)pcJNE));
    RETURN_ERROR_IF_FAIL(AddInstruction(table, InstructionNameToHash("CALL\0\0\0"), 1, (byte_t)pcCALL));
    RETURN_ERROR_IF_FAIL(AddInstruction(table, InstructionNameToHash("RET\0\0\0\0"), 0, (byte_t)pcRET));
    return CreateSuccess();
}
