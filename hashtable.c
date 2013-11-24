#include "hashtable.h"

static int __key_compare(const void *k1, size_t s1, const void *k2, size_t s2)
{
    if (!k1 || !s1 || !k2 || !s2)
    {
        return -1;
    }

    if (s1 != s2)
        return -1;

    return memcmp(k1, k2, s1);
}

int htab_init(htab *ht, hasher_t func) 
{
    size_t i;

    if (!ht || !func)
    {
        return -1;
    }
    ht->hasher = func;
    ht->compare = NULL;
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
    
    if (!ht || !ht->hasher || !key || !key_size || !val || !val_size)
    {
        return -1;
    }

    index = ht->hasher(key, key_size, ht->size);
    node = ht->table[index];

    if (node)
    {
        while (node->next && __key_compare(key, key_size, node->key, node->key_size))
        {
            node = node->next;
        }
        if (__key_compare(key, key_size, node->key, node->key_size))
        {
            node->next = malloc(sizeof(htab_node));
            node = node->next;
            node->key = NULL;
            node->val = NULL;
            node->next = NULL;
            node->key_size = 0;
            node->val_size = 0;
        }
    }
    else
    {
        node = malloc(sizeof(htab_node));
        if (!node)
        {
            return -1;
        }
        ht->table[index] = node;
        node->key = NULL;
        node->val = NULL;
        node->next = NULL;
        node->key_size = 0;
        node->val_size = 0;
    }
    
    node->key_size = key_size;
    node->val_size = val_size;
    node->key = malloc(node->key_size);
    node->val = malloc(node->val_size);

    if (!node->key || !node->val)
    {
        return -1;
    }

    memcpy(node->key, key, node->key_size);
    memcpy(node->val, val, node->val_size);
    ++ht->count;

    return 0;
}

void *htab_get(htab *ht, const void *key, size_t key_size)//, void **val, size_t *val_size)
{
    size_t index;
    htab_node *node;

    if (!ht || !key)
    {
        return NULL;
    }

    index = ht->hasher(key, key_size, ht->size);
    node = ht->table[index];

    while (node && node->key && node->val)
    {
        if (node->key_size == key_size)
        {
            if ((ht->compare && !ht->compare(key, node->key)))
            {
                return node->val;
            }
            else if (!memcmp(key, node->key, node->key_size))
            {
                return node->val;
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

    return NULL;
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
