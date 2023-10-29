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
    HMNode* buckets;    // the pointer to the first position of the array of buckets
    int capacity;       // the number of buckets
    double loadFactor;  // the maximum ratio of size/capacity before the hash map is resized
    int size;           // the number of elements the hash map currently has
} HashMap;

/* ---------------- Functions ---------------- */

HashMap* HMcreate();

void HMput(HashMap* hm, WCHAR* key, WCHAR* value);

WCHAR* HMremove(HashMap* hm, WCHAR* key);

WCHAR* HMget(HashMap* hm, WCHAR* key);

WCHAR* HMreplace(HashMap* hm, WCHAR* key, WCHAR* value);

void HMclear(HashMap* hm);

void HMdestroy(HashMap* hm);

#endif
