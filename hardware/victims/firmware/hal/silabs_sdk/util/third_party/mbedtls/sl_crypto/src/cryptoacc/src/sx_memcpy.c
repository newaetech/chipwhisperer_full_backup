/**
 * @file
 * @brief defines DMA configuration functions
 * @copyright Copyright (c) 2016-2019 Silex Insight. All Rights reserved
 */

#include <string.h>
#include <stdint.h>
#include "sx_memcpy.h"
#include "cryptodma_internal.h"
#include "cryptolib_def.h"

#ifndef BLK_MEMCPY_MIN_DMA_SIZE
#define BLK_MEMCPY_MIN_DMA_SIZE 0U
#endif


void sx_memcpy(void* dst, void* src, size_t length)
{
#ifdef __aarch64__
   if(((((uint64_t) dst) & 0x7) == 0) &&
      ((((uint64_t) src) & 0x7) == 0)) {
      size_t i;
      for (i = 0; i < (length >> 3); i++)
         ((uint64_t*) dst)[i] = ((uint64_t*) src)[i];
      for (i = i * 8; i < length; i++)
         ((uint8_t*) dst)[i] = ((uint8_t*) src)[i];
   } else
      for (size_t i = 0; i < length; i++)
         ((uint8_t*) dst)[i] = ((uint8_t*) src)[i];
#else
   memcpy(dst, src, length);
#endif
}

void sx_memset(void* src, int8_t pattern, size_t length)
{
#ifdef __aarch64__
   for(size_t i = 0; i < length; i++)
      ((uint8_t *) src)[i] = pattern;
#else
   memset(src, pattern, length);
#endif
}

void memcpy_blk(block_t dest, block_t src, uint32_t length)
{
   if (dest.flags & DMA_AXI_DESCR_DISCARD) {
      return;
   }

   if (dest.len < length) {
      length = dest.len;
   }
   if (!(src.flags & BLOCK_S_CONST_ADDR) && (src.len < length)) {
      length = src.len;
   }
   if (!length)
      return;
   /* Declaring a local variable to avoid the error:
    * comparison is always true due to limited range of data type [-Werror=type-limits]
    * when BLK_MEMCPY_MIN_DMA_SIZE = 0 and -Wextra compile option is used.
    */
   uint32_t min_dma_size = BLK_MEMCPY_MIN_DMA_SIZE;
   if (length >= min_dma_size) {
      cryptodma_config_direct(dest, src, length);
      cryptodma_start();
      cryptodma_wait();
      cryptodma_check_status();
   } else {
      if (src.flags & BLOCK_S_CONST_ADDR) {
         for (uint32_t i = 0; i < length; i+=4) {
            uint32_t v = *(volatile uint32_t*)src.addr;
            size_t len = (dest.len-i) < 4 ? (dest.len-i): 4;
            sx_memcpy(dest.addr + i, &v, len);
         }
      } else {
         sx_memcpy(dest.addr, src.addr, length);
      }
   }
}



void memcpy_blkOut(block_t dest, const volatile void * src, uint32_t length)
{
   block_t s = {(uint8_t*)src, length, BLOCK_S_INCR_ADDR};
   memcpy_blk(dest, s, length);
}

void memcpy_blkIn(volatile void * dest, block_t src, uint32_t length)
{
   block_t d = {(uint8_t*)dest, length, BLOCK_S_INCR_ADDR};
   memcpy_blk(d, src, length);
}

void memcpy_array(volatile void * dest, const volatile void * src, uint32_t length)
{
   block_t s = {(uint8_t*)src, length, BLOCK_S_INCR_ADDR};
   block_t d = {(uint8_t*)dest, length, BLOCK_S_INCR_ADDR};
   memcpy_blk(d, s, length);
}
