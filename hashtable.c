#include "hashtable.h"

static size_t __generic_hash(const void *key, size_t key_size, size_t table_size)
{
    size_t i;
    size_t sum = 0;
    unsigned char *k = (unsigned char *)key;
    for (i=0; i<key_size; ++i)
    {
        sum = (sum + (int)k[i]) % table_size;
    }
    return sum;
}

static int __generic_compare(const void *k1, size_t s1, const void *k2, size_t s2)
{
    if (!k1 || !s1 || !k2 || !s2)
    {
        return -1;
    }

    if (s1 != s2)
        return -1;

    return memcmp(k1, k2, s1);
}

static int __generic_copy(void **dst, const void *src, size_t sz)
{
    if (!dst || !src || !sz)
        return -1;

    if (!*dst)
        *dst = malloc(sz);

    if (!*dst)
        return -1;

    memcpy(*dst, src, sz);
    return 0;
}

static int __node_init(htab_node *node)
{
    if (!node)
        return -1;

    node->key = NULL;
    node->val = NULL;
    node->next = NULL;
    node->key_size = 0;
    node->val_size = 0;
    return 0;
}

static int __node_cleanup(htab_node *node)
{
    if (!node)
        return -1;

    free(node->key);
    free(node->val);
    node->key = NULL;
    node->val = NULL;
    node->next = NULL;
    node->key_size = 0;
    node->val_size = 0;
    return 0;
}

int htab_init(htab *ht, hash_t hash_f, compare_t compare_f, copy_t kcopy_f, copy_t vcopy_f)
{
    size_t i;

    if (!ht)
    {
        return -1;
    }
    ht->hash_f = hash_f ? hash_f : __generic_hash;
    ht->compare_f = compare_f ? compare_f : __generic_compare;
    ht->kcopy_f = kcopy_f ? kcopy_f : __generic_copy;
    ht->vcopy_f = vcopy_f ? vcopy_f : __generic_copy;
    ht->count = 0;
    ht->size = HTAB_INIT_SIZE;
    ht->table = calloc(sizeof(htab_node *), ht->size);
    if (!ht->table)
    {
        return -1;
    }

    for (i = 0; i < ht->size; ++i)
    {
        ht->table[i] = NULL;
    }

    return 0;
}

int htab_put(htab *ht, const void *key, size_t key_size, const void *val, size_t val_size)
{
    size_t index;
    htab_node *node;
    
    if (!ht || !ht->hash_f || !key || !key_size || !val || !val_size || !ht->compare_f)
    {
        return -1;
    }

    index = ht->hash_f(key, key_size, ht->size);
    node = ht->table[index];

    if (node)
    {
        while (node->next && ht->compare_f(key, key_size, node->key, node->key_size))
        {
            node = node->next;
        }
        if (ht->compare_f(key, key_size, node->key, node->key_size))
        {
            node->next = malloc(sizeof(htab_node));
            node = node->next;
            __node_init(node);
        }
    }
    else
    {
        node = malloc(sizeof(htab_node));
        if (!node)
        {
            return -1;
        }
        __node_init(node);
        ht->table[index] = node;
    }
    
    node->key_size = key_size;
    node->val_size = val_size;
    node->key = malloc(node->key_size);
    node->val = malloc(node->val_size);

    if (!node->key || !node->val)
    {
        return -1;
    }

    ht->kcopy_f(&node->key, key, node->key_size);
    ht->vcopy_f(&node->val, val, node->val_size);
    ++ht->count;

    return 0;
}

int htab_get(htab *ht, const void *key, size_t key_size, void **val, size_t *val_size)
{
    size_t index;
    htab_node *node;

    if (!ht || !key || !ht->compare_f || !val || !val_size)
    {
        return -1;
    }

    index = ht->hash_f(key, key_size, ht->size);
    node = ht->table[index];

    while (node && node->key && node->val)
    {
        if (node->key_size == key_size)
        {
            if (!ht->compare_f(key, key_size, node->key, node->key_size))
            {
                ht->vcopy_f(val, node->val, node->val_size);
                *val_size = node->val_size;
                return 0;
            }
            else
            {
                node = node->next;
            }
        }
        else
        {
            node = node->next;
        }
    }

    *val = NULL;
    *val_size = 0;
    return -1;
}

int htab_remove(htab *ht, const void *key, size_t key_size)
{
    size_t index;
    htab_node *node, *prev;

    if (!ht || !key || !ht->compare_f)
    {
        return -1;
    }

    index = ht->hash_f(key, key_size, ht->size);
    node = ht->table[index];
    prev = NULL;
    while (node)
    {
        if (!ht->compare_f(key, key_size, node->key, node->key_size))
        {
            if (!prev)
                ht->table[index] = ht->table[index]->next;
            else
                prev->next = node->next;

            __node_cleanup(node);
            free(node);
            --ht->count;

            return 0;
        }
        prev = node;
        node = node->next;
    }
    return -1;
}

int htab_cleanup(htab *ht)
{
    size_t i;
    htab_node *node;

    if (!ht->table || !ht->size)
        return -1;

    i = 0;
    for (i = 0; i < ht->size; ++i)
    {
        if (ht->table[i])
        {
            node = ht->table[i];
            while (node)
            {
                if (node->key)
                    free(node->key);
                else
                    return -1;

                if (node->val)
                    free(node->val);
                else
                    return -1;

                node = node->next;
            }
        }
    }

    return 0;
}
