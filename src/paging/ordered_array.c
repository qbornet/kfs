#include "ordered_array.h"
#include "kernel_heap.h"

int8_t standard_lessthan_predicate(type_t a, type_t b)
{
    return (a < b) ? 1 : 0;
}

ordered_array_t ordered_array_create(uint32_t             max_size,
                                     lessthan_predicate_t less_than)
{
    ordered_array_t a;
    uint32_t        address;

    address = kmalloc(max_size * sizeof(type_t));
    a.array = (void *)address;
    memset((uint8_t *)address, 0, max_size * sizeof(type_t));
    a.size = 0;
    a.max_size = max_size;
    a.less_than = less_than;

    return a;
}

ordered_array_t ordered_array_place(void                *address,
                                    uint32_t             max_size,
                                    lessthan_predicate_t less_than)
{
    ordered_array_t a;

    a.array = (type_t *)address;
    memset((uint8_t *)address, 0, max_size * sizeof(type_t));
    a.size = 0;
    a.max_size = max_size;
    a.less_than = less_than;

    return a;
}

void ordered_array_insert(type_t item, ordered_array_t *array)
{
    int32_t i;
    type_t  tmp, tmp2;

    // ASSERT( array -> less_than );
    i = 0;
    while(i < array->size && array->less_than(array->array[i], item)) i++;

    if(i == array->size) {
        array->array[array->size] = item;
        array->size += 1;
    }
    tmp = array->array[i];
    array->array[i] = item;
    while(i < array->size) {
        i++;
        tmp2 = array->array[i];
        array->array[i] = tmp;
        tmp = tmp2;
    }
    array->size++;
}

type_t ordered_array_lookup(int32_t i, ordered_array_t *array)
{
    // ASSERT( i < array -> size );

    return array->array[i];
}

void ordered_array_remove(int32_t i, ordered_array_t *array)
{
    while(i < array->size) {
        array->array[i] = array->array[i + 1];

        i += 1;
    }

    array->size -= 1;
}
