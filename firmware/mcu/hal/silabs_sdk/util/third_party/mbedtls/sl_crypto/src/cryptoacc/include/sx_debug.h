/**
 * @file libs/base/include/debug.h
 * @brief Debug definitions (PRINTFs etc)
 * @copyright Copyright (c) 2016-2018 Silex Insight. All Rights reserved
 */


#ifndef DEBUG_H
#define DEBUG_H

#include <stdint.h>
#include "sx_features.h"

#define SX_PRINTF(...) {}
#define SX_PRINT_ARRAY(addr, size) {}
#define SX_PRINT_BLK(blk) {}
#define SX_PRINT_ARRAY_32(addr, size) {}

/**
 * Throw an error in console and raise a tamper if condition cond is not met.
 * @param  cond Condition of the assertion (must always be true in theory)
 * @param  msg  Message to display in the error message (a simple string, no formating)
 */
#define SX_ASSERT(cond, msg) do { \
   if(! (cond)) { \
      SX_PRINTF("Assert in %s (%s:%d) %s\n ", __FUNCTION__, __FILE__, __LINE__, msg); \
   } \
} while (0)

/**
 * Throw an error in console if condition cond is not met but doesn't requires a message
 * @param  cond Condition of the assertion (must always be true in theory)
 */
#define SX_ASSERT_NM(cond) SX_ASSERT(cond, "")

/**
 * Throw an error in console and raise a tamper if condition cond is not met.
 * @param  cond Condition of the assertion (must always be true in theory)
 * @param  variadic parameters
 */
#define SX_ASSERT_VAR(cond, ...) do { \
   if(! (cond)) { \
      SX_PRINTF("Assert in %s (%s:%d) ", __FUNCTION__, __FILE__, __LINE__); \
      SX_PRINTF(__VA_ARGS__); \
      SX_PRINTF("\n"); \
   } \
} while (0)

/**
 * printf custom implementation (lightweight)
 * @param  format  Describes the format of the output
 * @param  VARARGS arguments to replace in the format string
 * @return         0 if success
 */
void myPrintf(const char *format, ...);

#endif
