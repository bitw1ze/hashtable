#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include "hashtable.h"

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
    int *val = NULL;
    int *val_out;
    size_t size_out;

    srand(time(NULL));
    assert(htab_init(&ht, NULL, NULL) == 0);

    // put a bunch of key/value pairs in the table
    for (i=0; i<100; ++i)
    {
        key = rand_key();
        val = malloc(sizeof(int));
        *val = rand() % 256;

        htab_put(&ht, key, strlen(key), val, sizeof(int));
        htab_get(&ht, key, strlen(key), (void **)&val_out, &size_out);
        assert(*val_out == *val);
        assert(size_out == sizeof(int));
        printf("%d: %s\n", *val_out, key);

        free(val);
        free(key);
    }

    // try to get a bunch of values for keys that probably don't exist
    for (i=0; i<100; ++i)
    {
        key = rand_key();

        htab_get(&ht, key, strlen(key), (void **)&val_out, &size_out);

        free(key);
        free(val_out);
    }

    assert(htab_cleanup(&ht) == 0);

    puts("pass");

    return 0;
}
