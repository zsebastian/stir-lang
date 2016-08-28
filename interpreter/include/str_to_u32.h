#ifndef STR_TO_U32_H_
#define STR_TO_U32_H_

#include "../modules/klib/khash.h"
#include <stdint.h>
#include <stdio.h>

KHASH_MAP_INIT_STR(_str_to_u32_t, uint32_t)

typedef khash_t(_str_to_u32_t) str_to_u32_t;

static inline str_to_u32_t* str_to_u32_create()
{
    return kh_init(_str_to_u32_t);
}

static inline void str_to_u32_destroy(str_to_u32_t* map)
{
    khiter_t k;
    for (k = kh_begin(map); k != kh_end(map); ++k) 
    {
        if (kh_exist(map, k)) 
        {
            free((char *)kh_key(map, k));
        }
    }
    kh_destroy(_str_to_u32_t, map);
}


static inline int str_to_u32_get(str_to_u32_t* map, char *str, uint32_t* out)
{
    khiter_t k = kh_get(_str_to_u32_t, map, str);
    if (k == kh_end(map))
    {
        printf("not found: %s\n", str);
        return 0;
    }
    else
    {
        *out = kh_val(map, k);
        return 1;
    }
}

static inline int str_to_u32_set(str_to_u32_t* map, char *str, uint32_t val)
{
    char *tmp = malloc(strlen(str) + 1);
    strcpy(tmp, str);
    str = tmp;

    int ret; 
    khiter_t k = kh_put(_str_to_u32_t, map, str, &ret); 
    kh_value(map, k) = val; 
    return ret;
}

#endif
