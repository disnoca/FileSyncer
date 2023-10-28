#ifndef H_HASH_MAP_INCLUDED
#define H_HASH_MAP_INCLUDED

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
    double load_factor; // the maximum ratio of size/capacity before the hash_map is resized
    int size;           // the number of elements the hash_map currently has
} HashMap;

/* ---------------- Functions ---------------- */

HashMap* hm_create();

void hm_put(HashMap* hash_map, WCHAR* key, WCHAR* value);

WCHAR* hm_remove(HashMap* hash_map, WCHAR* key);

WCHAR* hm_get(HashMap* hash_map, WCHAR* key);

WCHAR* hm_replace(HashMap* hash_map, WCHAR* key, WCHAR* value);

void hm_clear(HashMap* hash_map);

void hm_destroy(HashMap* hash_map);

#endif
