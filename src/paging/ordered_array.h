#ifndef ORDERED_ARRAY_H
#define ORDERED_ARRAY_H

#include "../lib/mem.h"
#include <stdint.h>

typedef void  *type_t;

typedef int8_t (*lessthan_predicate_t)(type_t, type_t);

typedef struct {
    type_t              *array;
    int32_t              size;
    uint32_t             max_size;
    lessthan_predicate_t less_than;
} ordered_array_t;

int8_t          standard_lessthan_predicate(type_t a, type_t b);

ordered_array_t ordered_array_create(uint32_t             max_size,
                                     lessthan_predicate_t less_than);

ordered_array_t ordered_array_place(void                *addr,
                                    uint32_t             max_size,
                                    lessthan_predicate_t less_than);

void            ordered_array_destroy(ordered_array_t *array);

void            ordered_array_insert(type_t item, ordered_array_t *array);

type_t          ordered_array_lookup(int32_t i, ordered_array_t *array);

void            ordered_array_remove(int32_t i, ordered_array_t *array);

#endif
