/***************************************************************************//**
 * @file
 * @brief FIPS-180-2 compliant SHA-1, SHA-224/256, SHA-384/512 implementation
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
/*
 * This file includes an alternative implementation of the standard
 * mbedtls/libary/sha[1][256].c using the secure element
 * incorporated in MCU devices from Silicon Laboratories.
 */
/*
 *  The SHA-1 standard was published by NIST in 1993.
 *
 *  http://www.itl.nist.gov/fipspubs/fip180-1.htm
 *
 *  The SHA-256 Secure Hash Standard was published by NIST in 2002.
 *
 *  http://csrc.nist.gov/publications/fips/fips180-2/fips180-2.pdf
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if (defined(MBEDTLS_SHA256_ALT) && defined(MBEDTLS_SHA256_C)) \
    || (defined(MBEDTLS_SHA1_ALT) && defined(MBEDTLS_SHA1_C)) \
    || (defined(MBEDTLS_SHA512_ALT) && defined(MBEDTLS_SHA512_C))

#include "em_device.h"

#if defined(CRYPTOACC_PRESENT)
#include "cryptoacc_management.h"
#include "shax.h"
#include "sx_hash.h"
#include "sx_errors.h"
#include "mbedtls/sha1.h"
#include "mbedtls/sha256.h"
#include "mbedtls/sha512.h"
#include "mbedtls/md.h"
#include <string.h>

int sha_x_process(SHA_Type_t algo,
                  uint8_t* state_in,
                  const unsigned char *blockdata,
                  uint8_t* state_out,
                  uint32_t num_blocks)
{
    int status;
    uint32_t sx_ret;
    sx_hash_fct_t hash_fct;
    block_t data_in;
    block_t state;
    uint8_t statebuf[SHA512_DIGESTSIZE];

    switch(algo) {
#if defined(MBEDTLS_SHA1_ALT) && defined(MBEDTLS_SHA1_C)
        case SHA1:
            hash_fct    = e_SHA1;
            data_in.len = SHA1_BLOCKSIZE * num_blocks;
            state.len   = SHA1_INITSIZE;
            break;
#endif
#if defined(MBEDTLS_SHA256_ALT) && defined(MBEDTLS_SHA256_C)
        case SHA224:
            hash_fct    = e_SHA224;
            data_in.len = SHA224_BLOCKSIZE * num_blocks;
            state.len   = SHA224_INITSIZE;
            break;
        case SHA256:
            hash_fct    = e_SHA256;
            data_in.len = SHA256_BLOCKSIZE * num_blocks;
            state.len   = SHA256_INITSIZE;
            break;
#endif
#if defined(MBEDTLS_SHA512_ALT) && defined(MBEDTLS_SHA512_C)
        case SHA384:
            hash_fct    = e_SHA384;
            data_in.len = SHA384_BLOCKSIZE * num_blocks;
            state.len   = SHA384_INITSIZE;
            break;

        case SHA512:
            hash_fct    = e_SHA512;
            data_in.len = SHA512_BLOCKSIZE * num_blocks;
            state.len   = SHA512_INITSIZE;
            break;
#endif
        default:
            return MBEDTLS_ERR_MD_FEATURE_UNAVAILABLE;
    }

    data_in.addr    = (uint8_t*)blockdata;
    data_in.flags   = 0;
    memcpy(statebuf, state_in, state.len);
    state.addr   = (uint8_t*)statebuf;
    state.flags  = 0;

    status = cryptoacc_management_acquire();
    if (status != 0) {
        return status;
    }
    sx_ret = sx_hash_update_blk(hash_fct, state, data_in);
    cryptoacc_management_release();

    if (sx_ret == CRYPTOLIB_SUCCESS) {
        memcpy(state_out, statebuf, state.len);
        return 0;
    } else {
        return MBEDTLS_ERR_MD_HW_ACCEL_FAILED;
    }
}

#endif /* #if defined(CRYPTOACC_PRESENT) */

#endif /* #if defined(MBEDTLS_SHA256_PROCESS_ALT) || defined(MBEDTLS_SHA1_PROCESS_ALT) || defined(MBEDTLS_SHA512_PROCESS_ALT) */
