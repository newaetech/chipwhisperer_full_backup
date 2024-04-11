/***************************************************************************//**
 * @file
 * @brief FIPS-180-2 compliant SHA-1 & SHA-256 implementation
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
 * mbedtls/libary/sha[1][256].c using the CRYPTO hardware accelerator
 * incorporated in MCU devices from Silicon Laboratories.
 */
 
/*
 *  The SHA-1 standard was published by NIST in 1993.
 *  http://www.itl.nist.gov/fipspubs/fip180-1.htm
 *
 *  The SHA-256 Secure Hash Standard was published by NIST in 2002.
 *  http://csrc.nist.gov/publications/fips/fips180-2/fips180-2.pdf
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "mbedtls/sha1.h"
#include "mbedtls/sha256.h"
#include "mbedtls/md.h"

#if ( defined(MBEDTLS_SHA256_ALT) && defined(MBEDTLS_SHA256_C) ) \
    || ( defined(MBEDTLS_SHA1_ALT) && defined(MBEDTLS_SHA1_C) )

#include "em_device.h"

#if defined(CRYPTO_PRESENT)
#include "em_crypto.h"
#include "em_core.h"
#include "crypto_management.h"
#include "em_assert.h"
#include <string.h>

#include "shax.h"

int sha_x_process(SHA_Type_t algo,
                  uint8_t* state_in,
                  const unsigned char *blockdata,
                  uint8_t* state_out,
                  uint32_t num_blocks)
{
    uint32_t temp[16];
    CORE_DECLARE_IRQ_STATE;
    CRYPTO_TypeDef *crypto = crypto_management_acquire();

    switch ( algo ) {
#if defined(MBEDTLS_SHA1_C)
        case SHA1:
            crypto->CTRL = CRYPTO_CTRL_SHA_SHA1;
            break;
#endif
#if defined(MBEDTLS_SHA256_C)
        case SHA224:
        case SHA256:
            crypto->CTRL = CRYPTO_CTRL_SHA_SHA2;
            break;
#endif
        default:
            return MBEDTLS_ERR_MD_FEATURE_UNAVAILABLE;
    }

    crypto->WAC      = 0;
    crypto->IEN      = 0;

    /* Set result width of MADD32 operation. */
    CRYPTO_ResultWidthSet(crypto, cryptoResult256Bits);

    /* Clear sequence control registers */
    crypto->SEQCTRL  = 0;
    crypto->SEQCTRLB = 0;

    /* Get state in big-endian */
    uint32_t statedata[8];
    for(size_t i = 0; i < 8; i++) {
        statedata[i] = __REV(((uint32_t*)state_in)[i]);
    }

    /* Put the state into crypto */
    CORE_ENTER_CRITICAL();
    CRYPTO_DDataWrite(&crypto->DDATA1, statedata);
    CORE_EXIT_CRITICAL();

    CRYPTO_EXECUTE_3( crypto,
                      CRYPTO_CMD_INSTR_DDATA1TODDATA0,
                      CRYPTO_CMD_INSTR_DDATA1TODDATA2,
                      CRYPTO_CMD_INSTR_SELDDATA0DDATA1 );

    /* Load the data block(s) */
    for ( size_t i = 0; i < num_blocks; i++ ) {
        const uint32_t *input_block; /* word aligned */

        if (((uint32_t)blockdata) & 0x3)
        {
            /* handle unaligned input data */
            const uint8_t * unaligned_data = &blockdata[i*64];
            memcpy(temp, unaligned_data, 64);
            input_block = temp;
        } else {
            input_block = (const uint32_t *)&blockdata[i*64];
        }
        
        CORE_ENTER_CRITICAL();
        CRYPTO_QDataWrite(&crypto->QDATA1BIG, input_block);
        CORE_EXIT_CRITICAL();

        /* Process the loaded data block */
        CRYPTO_EXECUTE_3(  crypto,
                           CRYPTO_CMD_INSTR_SHA,
                           CRYPTO_CMD_INSTR_MADD32,
                           CRYPTO_CMD_INSTR_DDATA0TODDATA1 );
    }

    /* Fetch state of the hash algorithm */
    if ((uint32_t)(state_out) & 0x3)
    {
        CORE_ENTER_CRITICAL();
        CRYPTO_DDataRead(&crypto->DDATA0, statedata);
        CORE_EXIT_CRITICAL();
        memcpy(state_out, statedata, 32);
    } else {
        CORE_ENTER_CRITICAL();
        CRYPTO_DDataRead(&crypto->DDATA0, (uint32_t*)state_out);
        CORE_EXIT_CRITICAL();
    }

    crypto_management_release( crypto );

    /* Store state in little-endian */
    for(size_t i = 0; i < 8; i++) {
        ((uint32_t*)state_out)[i] = __REV(((uint32_t*)state_out)[i]);
    }

    return 0;
}

#endif /* #if defined(CRYPTO_PRESENT) */

#endif /* #if defined(MBEDTLS_SHA256_ALT) || defined(MBEDTLS_SHA1_ALT) */
