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

static void add_entry(HashMap* hash_map, WCHAR* key, WCHAR* value);

/* ---------------- Hash Functions ---------------- */

static unsigned hash(const WCHAR* s) {
    unsigned hash = SEED;
    while (*s)
        hash = hash * 101 + (unsigned)*s++;
    return hash;
}

static void rehash(HashMap* hash_map) {
    hash_map->capacity *= 2;
    int new_capacity = hash_map->capacity;

    HMNode* old_buckets = hash_map->buckets;
    hash_map->buckets = (HMNode*) Calloc(new_capacity, sizeof(HMNode));

    HMNode *curr_node, *next_node;
    int old_capacity = new_capacity/2;
    for(int i = 0; i < old_capacity; i++) {
        curr_node = old_buckets + i;
        if(!memcmp(curr_node, &EMPTY_NODE, sizeof(HMNode))) continue;

        add_entry(hash_map, curr_node->key, curr_node->value);
        next_node = curr_node->next;
        while(next_node != NULL) {
            curr_node = next_node;
            next_node = curr_node->next;
            add_entry(hash_map, curr_node->key, curr_node->value);
            Free(curr_node);       // only free the consequent nodes because the buckets themselves will be freed all at once
        }
    }

    Free(old_buckets);
}

/* ---------------- Helper Functions ---------------- */

static HMNode* create_node(WCHAR* key, WCHAR* value) {
    HMNode* node = (HMNode*) Calloc(1, sizeof(HMNode));
    node->key = key;
    node->value = value;
    return node;
}

static HMNode* get_bucket(HashMap* hash_map, WCHAR* key) {
    return hash_map->buckets + (hash(key) % hash_map->capacity);
}

static HMNode* get_node(HashMap* hash_map, WCHAR* key) {
    HMNode* bucket = get_bucket(hash_map, key);
    while(wcscmp(bucket->key, key))
        bucket = bucket->next;
    return bucket;
}

static void add_entry(HashMap* hash_map, WCHAR* key, WCHAR* value) {
    HMNode* bucket = get_bucket(hash_map, key);
    
    if(!memcmp(bucket, &EMPTY_NODE, sizeof(HMNode))) {
        bucket->key = key;
        bucket->value = value;
        return;
    }

    while(bucket->next != NULL)
        bucket = bucket->next;
    bucket->next = create_node(key, value);
}

/* ---------------- Header Implementation ---------------- */

HashMap* hm_create() {
    HashMap* hash_map = (HashMap*) Calloc(1, sizeof(HashMap));
    hash_map->capacity = DEFUALT_INITIAL_CAPACITY;
    hash_map->load_factor = DEFAULT_LOAD_FACTOR;
    hash_map->buckets = (HMNode*) Calloc(DEFUALT_INITIAL_CAPACITY, sizeof(HMNode));
    return hash_map;
}

void hm_put(HashMap* hash_map, WCHAR* key, WCHAR* value) {
    add_entry(hash_map, key, value);

    if(++hash_map->size > (hash_map->capacity * hash_map->load_factor))
        rehash(hash_map);
}

WCHAR* hm_remove(HashMap* hash_map, WCHAR* key) {
    HMNode* bucket = get_bucket(hash_map, key);

    HMNode* prev_node = NULL;
    while(wcscmp(bucket->key, key)) {
        prev_node = bucket;
        bucket = bucket->next;
    }

    WCHAR* value = bucket->value;
    if(prev_node == NULL)
        *bucket = EMPTY_NODE;
    else {
        prev_node->next = bucket->next;
        Free(bucket);
    }

    hash_map->size--;
    return value;
}

WCHAR* hm_get(HashMap* hash_map, WCHAR* key) {
    return get_node(hash_map, key)->value;
}

WCHAR* hm_replace(HashMap* hash_map, WCHAR* key, WCHAR* value) {
    HMNode* node = get_node(hash_map, key);
    WCHAR* old_value = node->value;
    node->value = value;
    return old_value;

}

void hm_clear(HashMap* hash_map) {
    HMNode* buckets = hash_map->buckets;
    int capacity = hash_map->capacity;

    HMNode *curr_node, *next_node;
    for(int i = 0; i < capacity; i++) {
        curr_node = buckets + i;
        if(!memcmp(curr_node, &EMPTY_NODE, sizeof(HMNode))) continue;

        next_node = curr_node->next;
        *curr_node = EMPTY_NODE;
        while(next_node != NULL) {
            curr_node = next_node;
            next_node = curr_node->next;
            Free(curr_node);       // only free the consequent nodes because the buckets themselves must stay
        }
    }

    hash_map->size = 0;
}

void hm_destroy(HashMap* hash_map) {
    hm_clear(hash_map);
    Free(hash_map->buckets);
    Free(hash_map);
}
