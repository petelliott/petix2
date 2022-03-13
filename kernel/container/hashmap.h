#ifndef CONTAINER_HASHMAP_H
#define CONTAINER_HASHMAP_H

#include <stddef.h>
#include <stdbool.h>

typedef size_t (*hash_t)(void const *);
typedef int (*cmp_t)(void const*, void const *);
typedef void (*free_t)(void *);

struct hashmap_entry {
    void *key;
    void *value;
};

struct hashmap {
    void **data;
    size_t entries;
    size_t buckets;
    hash_t hash;
    cmp_t  cmp;
    free_t free;
};

void hashmap_init(struct hashmap *hashmap, hash_t hash, cmp_t cmp, free_t freefn);
void hashmap_free(struct hashmap *hashmap);

void hashmap_insert_value(struct hashmap *hashmap, void *value);
void hashmap_delete_value(struct hashmap *hashmap, void *value);
bool hashmap_has_value(struct hashmap *hashmap, void *value);

void hashmap_insert(struct hashmap *hashmap, void *key, void *value);
void hashmap_delete(struct hashmap *hashmap, void *key);
void const *hashmap_get(struct hashmap *hashmap, void *key);

size_t ptr_value_hash(void const *);
size_t ptr_key_hash(void const *);
size_t string_value_hash(void const *);
size_t string_key_hash(void const *);

int ptr_value_cmp(void const *, void const *);
int ptr_key_cmp(void const *, void const *);
int string_value_cmp(void const *, void const *);
int string_key_cmp(void const *, void const *);

#endif
