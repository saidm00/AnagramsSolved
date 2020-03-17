#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stdint.h>
#include <stdbool.h>

typedef struct hash_map
{
    _Bool *isUsed;
    uint8_t *keyMem;
    uint8_t *dataMem;

    size_t elemCount;
    size_t keySize;
    size_t elemSize;
} hash_map_t;


typedef struct hash_key
{
    size_t len;
    void *data;
} hash_key_t;


void initialize_hash_map(hash_map_t *map, size_t elemCount, size_t keySize, size_t elemSize);

uint64_t hash_map_get_index(hash_map_t *map, hash_key_t *key);

_Bool hash_map_has(hash_map_t *map, hash_key_t *key);
_Bool hash_map_has_by_index(hash_map_t *map, uint64_t idx);
const void *hash_map_get(hash_map_t *map, hash_key_t *key);
const void *hash_map_get_by_index(hash_map_t *map, uint64_t idx);
_Bool hash_map_set(hash_map_t *map, hash_key_t *key, const void *data);
void destroy_hash_map(hash_map_t *map);


#endif /* HASH_MAP_H */