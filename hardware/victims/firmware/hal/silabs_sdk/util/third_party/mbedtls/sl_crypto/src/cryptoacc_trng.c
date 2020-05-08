/***************************************************************************//**
 * @file
 * @brief True Random Number Generator (TRNG) driver for Silicon Labs devices
 *        with CRYPTOACC.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: APACHE-2.0
 *
 * This software is subject to an open source license and is distributed by
 * Silicon Laboratories Inc. pursuant to the terms of the Apache License,
 * Version 2.0 available at https://www.apache.org/licenses/LICENSE-2.0.
 * Such terms and conditions may be further supplemented by the Silicon Labs
 * Master Software License Agreement (MSLA) available at www.silabs.com and its
 * sections applicable to open source software.
 *
 ******************************************************************************/

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "em_device.h"

#if defined(CRYPTOACC_PRESENT)

#if defined(MBEDTLS_ENTROPY_HARDWARE_ALT) && defined(MBEDTLS_ENTROPY_C)

#include "sx_trng.h"
#include "sx_errors.h"
#include "cryptoacc_management.h"

int mbedtls_hardware_poll( void *data,
                           unsigned char *output,
                           size_t len,
                           size_t *olen ) {
    int lock_status;
    int trng_status;
    block_t data_out;
    size_t surplus_bytes = 0;
    uint32_t surplus_word = 0;
    (void) data;

    trng_status = cryptoacc_trng_initialize();
    if (trng_status != 0) {
        return trng_status;
    }

    // Store output length since we can expect sx_rng_get_rand_blk to
    // return exactly the requested number of words (return type is void).
    *olen = len;

    // The sx_rng_get_rand_blk function returns full 32-bit words
    // of random data. If the requested number of bytes (and output buffer)
    // is not a multiple of 4 bytes we need to get the remaining/surplus
    // bytes in an additional call.
    surplus_bytes = len & 0x3;
    len &= ~0x3;

    if ( len > 0 ) {
        // Setup data block and get random data.
        data_out = block_t_convert(output, len);
        lock_status = cryptoacc_management_acquire();
        if (lock_status != 0) {
            return lock_status;
        }
        sx_trng_get_rand_blk(data_out);
        cryptoacc_management_release();
    }

    if ( surplus_bytes != 0 ) {
        // Setup data block and get random data.
        data_out = block_t_convert(&surplus_word, 4);
        lock_status = cryptoacc_management_acquire();
        if (lock_status != 0) {
            return lock_status;
        }
        sx_trng_get_rand_blk(data_out);
        cryptoacc_management_release();
        for ( size_t i = 0; i < surplus_bytes; i++ ) {
          output[len+i] = (surplus_word >> (i*8)) & 0xFF;
        }
    }

    return 0;
}

#endif /* MBEDTLS_ENTROPY_HARDWARE_ALT && MBEDTLS_ENTROPY_C */

#endif /* CRYPTOACC_PRESENT */
