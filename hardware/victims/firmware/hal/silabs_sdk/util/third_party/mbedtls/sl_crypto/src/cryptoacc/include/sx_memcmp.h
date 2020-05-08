/**
 * @file
 * @brief Provides a family of memcmp functions showing constant-time operation
 * @copyright Copyright (c) 2016-2018 Silex Insight. All Rights reserved
 */


#ifndef MEMCMP_H
#define MEMCMP_H

#include <stdint.h>
#include "cryptolib_types.h"

/**
 * Compares two buffer in time constant
 * @param  in1  pointer to the first buffer
 * @param  in2  pointer to the second buffer
 * @param  size Size to compare
 * @return      0 if the two buffers are the same 1 otherwise
 */
uint32_t memcmp_time_cst(uint8_t *in1, uint8_t *in2, uint32_t size);

/**
 * Compares two block_t in time constant on 32-bit words. Supports
 * constant addresses (FIFO)
 * @param  in1  block_t of first buffer
 * @param  in2  block_t of second buffer
 * @param  size Size to compare in bytes
 * @return      0 if the two buffers are the same 1 otherwise
 */
uint32_t memcmp32_blk_time_cst(block_t in1, block_t in2, uint32_t size);
#endif
