#ifndef HASH_MAP_H_INCLUDED
#define HASH_MAP_H_INCLUDED

#include <windows.h>

/* ---------------- Structs ---------------- */

typedef struct HMNode HMNode;

struct HMNode {
    HMNode* next;
    WCHAR* key;
    WCHAR* value;
};

typedef struct {
    HMNode* buckets;        // the pointer to the first position of the array of buckets
    unsigned capacity;      // the number of buckets
    double loadFactor;      // the maximum ratio of size/capacity before the hash map is resized
    unsigned size;          // the number of elements the hash map currently has
} HashMap;

/* ---------------- Functions ---------------- */

HashMap* CreateHashMap();

void HMPut(HashMap* hm, WCHAR* key, WCHAR* value);

WCHAR* HMRemove(HashMap* hm, WCHAR* key);

WCHAR* HMGet(HashMap* hm, WCHAR* key);

WCHAR* HMReplace(HashMap* hm, WCHAR* key, WCHAR* value);

void HMClear(HashMap* hm);

void HMDestroy(HashMap* hm);

#endif
