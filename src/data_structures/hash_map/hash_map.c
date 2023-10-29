/**
 * Hash Map implementation.
 * 
 * @author Samuel Pires
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../wrapper_functions.h"
#include "hash_map.h"

static const HMNode EMPTY_NODE;

// Seed for the hash function
static const unsigned SEED = 0;

// The initial capacity - MUST be a power of two.
static const int DEFUALT_INITIAL_CAPACITY = 1 << 4;    // aka 16
static const double DEFAULT_LOAD_FACTOR = 0.75;

static void AddEntry(HashMap* hm, WCHAR* key, WCHAR* value);

/* ---------------- Hash Functions ---------------- */

static unsigned Hash(const WCHAR* s) {
    unsigned hash = SEED;
    while (*s)
        hash = hash * 101 + (unsigned)*s++;
    return hash;
}

static void Rehash(HashMap* hm) {
    hm->capacity *= 2;
    int newCapacity = hm->capacity;

    HMNode* oldBuckets = hm->buckets;
    hm->buckets = (HMNode*) Calloc(newCapacity, sizeof(HMNode));

    HMNode *currNode, *nextNode;
    int oldCapacity = newCapacity/2;
    for(int i = 0; i < oldCapacity; i++) {
        currNode = oldBuckets + i;
        if(!memcmp(currNode, &EMPTY_NODE, sizeof(HMNode))) continue;

        AddEntry(hm, currNode->key, currNode->value);
        nextNode = currNode->next;
        while(nextNode != NULL) {
            currNode = nextNode;
            nextNode = currNode->next;
            AddEntry(hm, currNode->key, currNode->value);
            Free(currNode);       // only free the consequent nodes because the buckets themselves will be freed all at once
        }
    }

    Free(oldBuckets);
}

/* ---------------- Helper Functions ---------------- */

static HMNode* CreateNode(WCHAR* key, WCHAR* value) {
    HMNode* node = (HMNode*) Calloc(1, sizeof(HMNode));
    node->key = key;
    node->value = value;
    return node;
}

static HMNode* GetBucket(HashMap* hm, WCHAR* key) {
    return hm->buckets + (Hash(key) % hm->capacity);
}

static HMNode* GetNode(HashMap* hm, WCHAR* key) {
    HMNode* bucket = GetBucket(hm, key);
    while(wcscmp(bucket->key, key))
        bucket = bucket->next;
    return bucket;
}

static void AddEntry(HashMap* hm, WCHAR* key, WCHAR* value) {
    HMNode* bucket = GetBucket(hm, key);
    
    if(!memcmp(bucket, &EMPTY_NODE, sizeof(HMNode))) {
        bucket->key = key;
        bucket->value = value;
        return;
    }

    while(bucket->next != NULL)
        bucket = bucket->next;
    bucket->next = CreateNode(key, value);
}

/* ---------------- Header Implementation ---------------- */

HashMap* HMcreate() {
    HashMap* hm = (HashMap*) Calloc(1, sizeof(HashMap));
    hm->capacity = DEFUALT_INITIAL_CAPACITY;
    hm->loadFactor = DEFAULT_LOAD_FACTOR;
    hm->buckets = (HMNode*) Calloc(DEFUALT_INITIAL_CAPACITY, sizeof(HMNode));
    return hm;
}

void HMput(HashMap* hm, WCHAR* key, WCHAR* value) {
    AddEntry(hm, key, value);

    if(++hm->size > (hm->capacity * hm->loadFactor))
        Rehash(hm);
}

WCHAR* HMremove(HashMap* hm, WCHAR* key) {
    HMNode* bucket = GetBucket(hm, key);

    HMNode* prevNode = NULL;
    while(wcscmp(bucket->key, key)) {
        prevNode = bucket;
        bucket = bucket->next;
    }

    WCHAR* value = bucket->value;
    if(prevNode == NULL)
        *bucket = EMPTY_NODE;
    else {
        prevNode->next = bucket->next;
        Free(bucket);
    }

    hm->size--;
    return value;
}

WCHAR* HMget(HashMap* hm, WCHAR* key) {
    return GetNode(hm, key)->value;
}

WCHAR* HMreplace(HashMap* hm, WCHAR* key, WCHAR* value) {
    HMNode* node = GetNode(hm, key);
    WCHAR* old_value = node->value;
    node->value = value;
    return old_value;

}

void HMclear(HashMap* hm) {
    HMNode* buckets = hm->buckets;
    int capacity = hm->capacity;

    HMNode *currNode, *nextNode;
    for(int i = 0; i < capacity; i++) {
        currNode = buckets + i;
        if(!memcmp(currNode, &EMPTY_NODE, sizeof(HMNode))) continue;

        nextNode = currNode->next;
        *currNode = EMPTY_NODE;
        while(nextNode != NULL) {
            currNode = nextNode;
            nextNode = currNode->next;
            Free(currNode);       // only free the consequent nodes because the buckets themselves must stay
        }
    }

    hm->size = 0;
}

void HMdestroy(HashMap* hm) {
    HMclear(hm);
    Free(hm->buckets);
    Free(hm);
}
