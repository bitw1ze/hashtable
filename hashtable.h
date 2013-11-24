#include <stdlib.h>
#include <string.h>

#define HTAB_INIT_LEN 64

typedef size_t (*hasher_t)(const void *key, size_t key_size, size_t table_size);

typedef struct __htab_node {
    struct __htab_node *next;
    void *key;
    size_t key_size;
    void *val;
    size_t val_size;
} htab_node;

typedef struct __hashtable {
    htab_node **table;
    size_t len;
    size_t count;
    hasher_t hasher;
    int (*compare)(const void *k1, const void *k2);
} htab;

int htab_init(htab *ht, hasher_t func);
int htab_cleanup(htab *ht);
int htab_put(htab *ht, const void *key, size_t key_size, const void *val, size_t val_size);
void * htab_get(htab *ht, const void *val, size_t key_size);
