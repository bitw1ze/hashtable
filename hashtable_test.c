#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include "hashtable.h"

char *rand_key(void) {
    int i, len;
    
    len = rand() % 15 + 2;
    char *key = malloc(len);
    if (!key)
        return NULL;
    for (i=0; i<len-1; ++i)
        key[i] = rand() % (126-32) + 32;
    key[len-1] = '\0';
    return key;
}

int main(int argc, const char *argv[])
{
    htab ht;
    char *key = NULL;
    int *val = NULL;
    int *val_out = NULL;
    size_t size_out;
    size_t i, ntests;

    ntests = argc == 2 ? atoi(argv[1]) : 1000;
    printf("testing hash table with %lu iterations each\n", ntests);
    srand(time(NULL));
    assert(htab_init(&ht, NULL, NULL, NULL, NULL) == 0);

    // put a bunch of key/value pairs in the table
    for (i=0; i<ntests; ++i)
    {
        key = rand_key();
        val = malloc(sizeof(int));
        *val = rand() % 256;

        htab_put(&ht, key, strlen(key), val, sizeof(int));
        htab_get(&ht, key, strlen(key), (void **)&val_out, &size_out);
        assert(*val_out == *val);
        assert(size_out == sizeof(int));

        free(val);
        free(key);
        free(val_out);
        val_out = NULL;
        val = NULL;
        key = NULL;
    }

    puts("pass put/get test");

    // try to get a bunch of values for keys that probably don't exist
    for (i=0; i<ntests; ++i)
    {
        key = rand_key();
        htab_get(&ht, key, strlen(key), (void **)&val_out, &size_out);
        free(key);
    }

    puts("pass get bad keys test");

    // try to remove a bunch of nodes that probably don't exist
    for (i=0; i<ntests; ++i)
    {
        key = rand_key();
        htab_remove(&ht, key, strlen(key));
        free(key);
    }

    puts("pass remove bad keys test");

    for (i=0; i<ntests; ++i)
    {
        key = rand_key();
        val = malloc(sizeof(int));
        *val = rand() % 256;

        htab_put(&ht, key, strlen(key), val, sizeof(int));
        htab_remove(&ht, key, strlen(key));
        htab_get(&ht, key, strlen(key), (void **)&val_out, &size_out);
        assert(val_out == NULL);
        assert(size_out == 0);

        free(val);
    }

    puts("pass delete nodes test");

    assert(htab_cleanup(&ht) == 0);

    puts("pass cleanup test");

    return 0;
}
