/*
 * Copyright (c) 2019 Jie Zheng
 */
#ifndef _SORT_H
#define _SORT_H
#include <stdint.h>
#include <string.h>

#define ELEM(type, base, index) ((index)+ (type *)((uint64_t)(base)))

// Here I use the simplest bubble sort in an ascending order
#define SORT(type, base, num, comp) {                                          \
    int idx_out = (num) - 1;                                                   \
    for (; idx_out > 0; idx_out--) {                                           \
        int idx_in = 0;                                                        \
        for (; idx_in < idx_out; idx_in++) {                                   \
            type * prev_elem = ELEM(type, base, idx_in);                       \
            type * next_elem = ELEM(type, base, idx_in + 1);                   \
            if (comp(prev_elem, next_elem) > 0) {                              \
                type tmp_elem;                                                 \
                memcpy(&tmp_elem, prev_elem, sizeof(type));                    \
                memcpy(prev_elem, next_elem, sizeof(type));                    \
                memcpy(next_elem, &tmp_elem, sizeof(type));                    \
            }                                                                  \
        }                                                                      \
    }                                                                          \
}


// FIXED: there must be a fix to improve the effenciy to sort the items.
// it's perforance critical
// the last element is the one to be inserted
// XXX: THE PERFORMANCE IS GREATLY IMPROVED
// FIXME: supersede glibc memcpy. 
#define INSERTION_SORT(type, base, num, comp) {                                \
    if (num) {                                                                 \
        type * elem_target = ELEM(type, base, ((num) - 1));                    \
        int __idx_low = 0;                                                     \
        int __idx_high = (num) - 2;                                            \
        int __idx_move = 0;                                                    \
        for (; __idx_low <= __idx_high; ) {                                    \
            type * elem_low = ELEM(type, base, __idx_low);                     \
            type * elem_high = ELEM(type, base, __idx_high);                   \
            if (comp(elem_target, elem_low) <= 0) {                            \
                __idx_move = __idx_low;                                        \
                break;                                                         \
            } else if (comp(elem_high, elem_target) <= 0) {                    \
                __idx_move = __idx_high + 1;                                   \
                break;                                                         \
            } else {                                                           \
                int __idx_mid = (__idx_low + __idx_high) / 2;                  \
                if (__idx_mid == __idx_low || __idx_mid == __idx_high) {       \
                    __idx_move = __idx_mid + 1;                                \
                    break;                                                     \
                }                                                              \
                type * elem_mid = ELEM(type, base, __idx_mid);                 \
                if (comp(elem_target, elem_mid) < 0) {                         \
                    __idx_high = __idx_mid;                                    \
                } else {                                                       \
                    __idx_low = __idx_mid;                                     \
                }                                                              \
            }                                                                  \
        }                                                                      \
        if (__idx_move < ((num) - 1)) {                                        \
            type elem_tmp;                                                     \
            memcpy(&elem_tmp, elem_target, sizeof(type));                      \
            int __idx = (num) - 1;                                             \
            for (; __idx > __idx_move; __idx--) {                              \
                memcpy(ELEM(type, base, __idx),                                \
                       ELEM(type, base, __idx - 1),                            \
                       sizeof(type));                                          \
            }                                                                  \
            memcpy(ELEM(type, base, __idx_move), &elem_tmp, sizeof(type));     \
        }                                                                      \
    }                                                                          \
}

#endif
