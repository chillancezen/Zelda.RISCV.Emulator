/*
 * Copyright (c) 2019 Jie Zheng
 */


#ifndef _CONFIG_H
#define _CONFIG_H
#define BUILD_TYPE_DEBUG 0x1
#define BUILD_TYPE_RELEASE 0x2

#define BUILD_TYPE BUILD_TYPE_DEBUG

#define XLEN 32
// The maximum number of harts a vm can support
#define MAX_NR_HARTS 64

//#define DEBUG_TRACE
//#define DEBUG_TRANSLATION
#define NATIVE_DEBUGER

#define COLORED_OUTPUT


/*
 * initial logging level
 * LOG_TRACE, LOG_DEBUG, LOG_INFO,LOG_WARN, LOG_ERROR, LOG_FATAL, LOG_UART
 * it may be overriden by per-vm debug.verbosity
 */
#define LOGGING_LEVEL LOG_TRACE
#endif
