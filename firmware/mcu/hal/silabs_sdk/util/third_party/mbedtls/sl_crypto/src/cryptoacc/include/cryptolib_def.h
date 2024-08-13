
#ifndef CRYPTOLIB_DEF_H
#define CRYPTOLIB_DEF_H

#include "em_device.h"
/* Define 'uint8_t', 'uint32_t'... */
#include <stdint.h>

/* Define 'bool' */
#include <stdbool.h>

/* Define 'NULL' */
#include <stddef.h>

/* Define supported features */
#include "sx_features.h"

/* Define 'CHECK_RESULT' macro (from cryptolib) */
#include "compiler_extentions.h"

/* Define 'block_t' (from cryptolib) */
#include "cryptolib_types.h"

/* Define 'memcpy_*' functions (from cryptolib) */
#include "sx_memcpy.h"

/* Define 'memcmp*' functions (from cryptolib) */
#include "sx_memcmp.h"

#include "sx_debug.h"

/*****************************************************************************
 ** Address map
 *****************************************************************************/

#ifndef ADDR_CRYPTOWRAP
#define ADDR_CRYPTOWRAP            CRYPTOACC_BASE
#endif

#define ADDR_CRYPTOMASTER_REGS     (ADDR_CRYPTOWRAP+0x00000000)
#define ADDR_BA431_REGS            (ADDR_CRYPTOWRAP+0x00001000)
#define ADDR_BA414EP_REGS          (ADDR_CRYPTOWRAP+0x00002000)
#define ADDR_BA431_FIFO            (ADDR_CRYPTOWRAP+0x00004000)
#define ADDR_BA414EP_CRYPTORAM     (ADDR_CRYPTOWRAP+0X00008000)
#define ADDR_BA414EP_UCODE         (ADDR_CRYPTOWRAP+0X0000C000)

/*****************************************************************************
 ** Enable polling defines
 *****************************************************************************/

#define WAIT_CRYPTOMASTER_WITH_REGISTER_POLLING     1
#define WAIT_RNG_WITH_REGISTER_POLLING              1
#define WAIT_PK_WITH_REGISTER_POLLING               1

/*****************************************************************************
 ** Bus error handling
 *****************************************************************************/

/** @brief Trigger hardfault by access to invalid memory address */
static inline void trigger_hardfault(void) {
   uint32_t *bad_mem_addr = (uint32_t*) 0x3FFFFFFC;
   *bad_mem_addr = 1;
}

#define TRIGGER_HARDFAULT_FCT()       trigger_hardfault();

/*****************************************************************************
 ** Debug
 *****************************************************************************/

#define CRYPTOLIB_PRINTF                SX_PRINTF
#define CRYPTOLIB_ASSERT(cond, msg)     SX_ASSERT(cond, msg)
#define CRYPTOLIB_ASSERT_NM(cond)       SX_ASSERT_NM(cond)
#define CRYPTOLIB_ASSERT_VAR(cond, ...) SX_ASSERT_VAR(cond, __VA_ARGS__);

/*****************************************************************************
 ** Crypto DMA
 *****************************************************************************/

#define BLK_MEMCPY_MIN_DMA_SIZE         16UL

#endif
