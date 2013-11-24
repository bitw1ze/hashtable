#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include "hashtable.h"

size_t hasher(const void *key, size_t key_size, size_t table_size)
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

char *rand_key(void) {
    int i, len;
    
    len = rand() % 15 + 2;
    char *key = malloc(len);
    for (i=0; i<len-1; ++i)
        key[i] = rand() % (126-32) + 32;
    key[len-1] = '\0';
    return key;
}

int main(int argc, const char *argv[])
{
    htab ht;
    int i;
    char *key = NULL;
    char *val = NULL;

    srand(time(NULL));
    htab_init(&ht, hasher);

    // put a bunch of key/value pairs in the table
    for (i=0; i<100; ++i)
    {
        key = rand_key();
        val = malloc(sizeof(int));
        *val = rand() % 256;

        htab_put(&ht, key, strlen(key), val, sizeof(int));
        int *v = htab_get(&ht, key, strlen(key));
        assert(v);
        printf("%d: %s\n", *v, key);
    }

    // try to get a bunch of values for keys that probably don't exist
    for (i=0; i<100; ++i)
    {
        key = rand_key();
        htab_get(&ht, key, strlen(key));
    }

    puts("pass");

    return 0;
}
