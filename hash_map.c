#include <memory.h>
#include <stdlib.h>

#include "hash_map.h"

const uint64_t FNV_offset_basis = 0xCBF29CE484222325ULL;
const uint64_t FNV_prime = 0x00000100000001B3ULL;

uint64_t hash64_fnv_1a(uint8_t *data, size_t len)
{
    uint64_t hash = FNV_offset_basis;
    uint8_t byte_of_data; // why this intermediate variable

    for (size_t i = 0; i < len; ++i)
    {
        byte_of_data = data[i];
        hash = hash ^ byte_of_data;
        hash = hash * FNV_prime;
    }

    return hash;
}

uint64_t hash64(uint8_t *data, size_t len)
{
    hash64_fnv_1a(data, len);
}

void initialize_hash_map(hash_map_t *map, size_t elemCount, size_t keySize, size_t elemSize)
{
    map->elemCount = elemCount;
    map->elemSize = elemSize;

    size_t size = elemSize * elemCount;
    
    map->dataMem = (uint8_t *)malloc(size);

    map->isUsed = (_Bool *)malloc(sizeof(_Bool) * elemCount);
    memset(map->isUsed, 0, sizeof(_Bool) * elemCount);

    map->keySize = keySize;
    map->keyMem = (uint8_t *)malloc(keySize * elemCount);
    memset(map->keyMem, 0, keySize * elemCount);
}
/*
uint8_t *hash_map_subscript(hash_map_t *map, size_t idx)
{
	return &map->data[idx * map->elem_size];
}
*/
/*
uint64_t hash_map_get_index(hash_map_t *map, hash_key_t *key);

_Bool    hash_map_has(hash_map_t *map, hash_key_t *key);
uint8_t *hash_map_get(hash_map_t *map, hash_key_t *key);
_Bool    hash_map_set(hash_map_t *map, hash_key_t *key, uint8_t *data, size_t len);
*/
uint64_t hash_map_get_index(hash_map_t *map, hash_key_t *key)
{
    uint64_t idx = hash64(key->data, key->len) % map->elemCount;
    uint64_t keyOffset = idx * map->keySize;

    while (true)
    {
        _Bool used = map->isUsed[idx];
        _Bool equal = !memcmp(&map->keyMem[keyOffset], key->data, map->keySize);

        if (!used || used && equal)
            break;
        else
        {
            idx = (idx + 1) % map->elemCount;
            keyOffset += map->keySize;
        }
    }

    return idx;
}

_Bool hash_map_has(hash_map_t *map, hash_key_t *key)
{
    uint64_t idx = hash_map_get_index(map, key);
    return map->isUsed[idx];
}

const void *hash_map_get(hash_map_t *map, hash_key_t *key)
{
    uint64_t idx = hash_map_get_index(map, key);
    uint64_t keyOffset = idx * map->keySize;
    
    /*
    while (true) {
        _Bool isUsed = map->isUsed[idx];
        
        if (!isUsed) 
            return null;    // key not present in map

        bool equal = !memcmp(&map->keyMem[keyOffset], key->data, map->keySize);
        if (equal)
            break;          // bin found
        else
            idx = (idx + 1) % map->elemCount;
    }
    */
    
    uint64_t offset = idx * map->elemSize;
    uint8_t *bytes = &map->dataMem[offset];

    //printf("GET: idx: %lu; value: %u \n", idx, *(unsigned int *) bytes);
    
    return bytes;
}

const void *hash_map_get_by_index(hash_map_t *map, uint64_t idx)
{
    uint64_t offset = idx * map->elemSize;
    uint8_t *bytes = &map->dataMem[offset];
    return bytes;
}


_Bool hash_map_has_by_index(hash_map_t *map, uint64_t idx)
{
    return map->isUsed[idx];
}

_Bool hash_map_set(hash_map_t *map, hash_key_t *key, const void *data)
{
    uint64_t idx = hash_map_get_index(map, key);
    uint64_t keyOffset = idx * map->keySize;
/*
    while (true)
    {
        _Bool used = map->isUsed[idx];
        _Bool equal = !memcmp(&map->keyMem[keyOffset], key->data, map->keySize);

        if (!equal && used || !used && equal)
            idx = (idx + 1) % map->elemCount;
        else
            break;
    }
  */  

    // set used flag
    if (!map->isUsed[idx])
    {
        memcpy(&map->keyMem[keyOffset], key->data, map->keySize);
        map->isUsed[idx] = true;
    }

    uint64_t offset = idx * map->elemSize;
    uint8_t *bytes = &map->dataMem[offset];
    memcpy(bytes, data, map->elemSize);
    
    // printf("SET: idx: %lu; value: %u \n", idx, *(unsigned int *) data);

    return true;
}

void destroy_hash_map(hash_map_t *map)
{
    free(map->dataMem);
    free(map->isUsed);
    free(map->keyMem);
}