/**
 * @file
 * @brief Provide types, helper macros and functions used everywhere.
 * @copyright Copyright (c) 2016-2018 Silex Insight. All Rights reserved
 */

#ifndef CRYPTOLIB_TYPES_H
#define CRYPTOLIB_TYPES_H

#include <stdint.h>

/**
* @brief  Array-like data abstraction handling both contiguous memory and hardware FIFO.
*
* Embedding side by side the data pointer and only the data length provides an
* abstraction which does not cover data coming from/to a hardware FIFO (data always
* read/written at the same address). The block_t structure embeds an additional
* field carrying enough information to work in both array mode (2 different words
* are stored at 2 different addresses) and FIFO mode (2 different words are loaded
* from the same address).
*
* A set of dedicated functions is provided to handle the basic operations
* (using a similar interface to memcpy/memset/memcmp).
*/
typedef struct block_s
{
   uint8_t *addr;   /**< Start address of the data (FIFO or contiguous memory) */
   uint32_t len;    /**< Length of data expressed in bytes */
   uint32_t flags;  /**< Flags equals to ::BLOCK_S_CONST_ADDR or ::BLOCK_S_INCR_ADDR */
} block_t;

/**
 * @brief Provide a default block_t initializer
 *
 * with
 * - NULL base address
 * - zero length,
 * - contiguous memory addressing with discarding flag
 *   in case of DMA transfer (see ::DMA_AXI_DESCR_DISCARD)
 */
extern const block_t NULL_blk;

/**
 * Get the nth bit value of a uint8_t
 * @param  offset the bit number (LSB)
 * @param var Variable containing the boolean
 * @return        The bit value (on an uint32_t)
 */
#define GETBOOL(var, offset) (((var) >> (offset)) & 1)


/**
 * Swap bytes of a 32-bit word
 * @param  word   The 32-bit word to byte-swap
 * @return        The byte-swapped value
 */
#define SWAPBYTES32(word) ((((word) & 0xFF000000) >> 24) | (((word) & 0x00FF0000) >> 8) | (((word) & 0x0000FF00) << 8) | (((word) & 0x000000FF) << 24))

/** @brief Align on word boundary */
#define ALIGNED __attribute__((aligned(0x4)))


/** @brief value of ::block_s.flags to set addressing in constant mode (pointing to a FIFO) */
#define BLOCK_S_CONST_ADDR             0x10000000
/** @brief value of ::block_s.flags to set addressing in increment mode (pointing to a contiguous data array) */
#define BLOCK_S_INCR_ADDR              0x00000000
/** @brief mask for ::block_s.flags to only get DMA-related options */
#define BLOCK_S_FLAG_MASK_DMA_PROPS    0x70000000

/**
 * @brief Convert a pair of array address and length to a block_t
 * @param  array  Array address. It does not support FIFO addresses.
 * @param  length Length of data expressed in bytes
 * @return block_t
 */
static inline block_t block_t_convert(const volatile void *array, uint32_t length)
{
    block_t blk = {(uint8_t *)array, length, BLOCK_S_INCR_ADDR};
    return blk;
}

#define BLK_LITARRAY(literal) (block_t){(uint8_t *)(literal), sizeof(literal), BLOCK_S_INCR_ADDR}

#define CST_BLK_LITARRAY(literal) {(uint8_t *)(literal), sizeof(literal), BLOCK_S_INCR_ADDR}

/**
 * Round-up integer to next 32-bit multiple
 * @param  value The value to round
 * @return       The rounded value
 */
#define roundup_32(value) (((value) + 3) & ~3)


/**
 * Round-up the length of a block_t to 32-bit if the address is a FIFO
 * @param  blk    A pointer to the block_t to adapt
 */
static inline void block_t_adapt_len(block_t * blk)
{
   if (blk->flags & BLOCK_S_CONST_ADDR)
      blk->len = roundup_32(blk->len);
}

/** @brief Inline function for minimum value between 2 numbers */
static inline uint32_t SX_MIN(uint32_t a, uint32_t b)
{
   if(a<b) { return a; } else { return b; }
}

/** @brief Enum to provide endianness information */
enum sx_endianness
{
   sx_big_endian,
   sx_little_endian
};

#endif /* CRYPTOLIB_TYPES_H */
