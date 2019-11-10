/*
 * Copyright (c) 2019 Jie Zheng
 */
#ifndef _PHYSICAL_MEMORY_H
#define _PHYSICAL_MEMORY_H

#include <stdint.h>

void *
preallocate_physical_memory(int64_t nr_bytes);

#endif
