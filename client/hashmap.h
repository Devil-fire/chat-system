#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define HASH_TABLE_MAX_SIZE 100
typedef struct HashNode_Struct HashNode;
HashNode* hashTable[HASH_TABLE_MAX_SIZE];

struct HashNode_Struct
{
    char* sKey;
    int nValue;
    HashNode* pNext;
};

HashNode* hash_table_lookup(HashNode* hashTable[], const char* skey);
void hash_table_remove(HashNode* hashTable[], const char* skey);
void hash_table_insert(HashNode* hashTable[],const char* skey, int nvalue);
void hash_table_init(HashNode* hashTable[]);
