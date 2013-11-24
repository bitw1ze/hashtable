#include <stdlib.h>
#include <string.h>

// TODO: increase table size to larger prime when it becomes full
#define HTAB_INIT_SIZE 71

typedef size_t (*hash_t)(const void *key, size_t key_size, size_t table_size);
typedef int (*compare_t)(const void *k1, size_t s1, const void *k2, size_t s2);
typedef struct __htab_node {
    struct __htab_node *next;
    void *key;
    size_t key_size;
    void *val;
    size_t val_size;
} htab_node;

typedef struct __hashtable {
    htab_node **table;
    size_t size;
    size_t count; // unused for now, but needed for when resizing later
    hash_t hash_f;
    compare_t compare_f;
} htab;

int htab_init(htab *ht, hash_t hash_f, compare_t compare_f);
int htab_cleanup(htab *ht);
int htab_put(htab *ht, const void *key, size_t key_size, const void *val, size_t val_size);
int htab_get(htab *ht, const void *key, size_t key_size, void **val, size_t *val_size);
