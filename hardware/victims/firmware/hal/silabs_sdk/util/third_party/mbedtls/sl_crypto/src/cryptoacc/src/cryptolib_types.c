/**
 * @file
 * @brief Provide the value when user wants a default block_t
 * @copyright Copyright (c) 2016-2018 Silex Insight. All Rights reserved
 */

#include <stddef.h>
#include "cryptolib_types.h"
#include "sx_dma.h"

const block_t NULL_blk = {
   .addr = NULL,
   .len = 0,
   .flags = BLOCK_S_INCR_ADDR | DMA_AXI_DESCR_DISCARD // FIXME we are mixing BLOCK_S_ with DMA_AXI_
};
