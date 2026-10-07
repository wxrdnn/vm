#include "../h/interpreter/instTable.h"
#include "../h/debug.h"
#include <cstdio>
#include <cstdlib>

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

InstructionNode *GetInstruction(const InstructionTable *const table, const unsigned hash)
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

Error AddInstruction(InstructionTable *const table, const unsigned hash, const unsigned code)
{
    ASSERT(table);

    Error error = CreateSuccess();
    size_t index = hash % table->size;

    InstructionNode *nodeFound = GetInstruction(table, hash);
    if (nodeFound)
    {
        printf("DEBUG: Instruction with hash <0x%x> already exists with code %u.", hash, nodeFound->code);
        return error;
    }

    InstructionNode *newNode = (InstructionNode *)calloc(1, sizeof(InstructionNode)); // Create new node
    if (!newNode)
    {
        error = CreateError(ecCantAllocateMemory, "instruction node");
        return error;
    }

    newNode->hash = hash;
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

InstructionNode *FindTailNode(const InstructionNode *const start)
{
    ASSERT(start);

    const InstructionNode *tailNode = start;
    while (tailNode->next != NULL)
    {
        tailNode = tailNode->next;
    }

    return (InstructionNode *)tailNode;
}

InstructionNode *FindFirstBeforeTailNode(const InstructionNode *const start)
{
    ASSERT(start);
    ASSERT(start->next);

    const InstructionNode *beforeTailNode = start;

    while (beforeTailNode->next->next != NULL)
    {
        beforeTailNode = beforeTailNode->next;
    }

    return (InstructionNode *)beforeTailNode;
}
