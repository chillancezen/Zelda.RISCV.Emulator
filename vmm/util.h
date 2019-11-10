/*
 * Copyright (c) 2019 Jie Zheng
 */
#ifndef _UTIL_H
#define _UTIL_H
#include <stdint.h>

int
preload_image(void * addr, int64_t length, const char * image_path);

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#endif
