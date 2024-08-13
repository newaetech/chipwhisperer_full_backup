/*
 *  AES-CCM abstraction based on the CRYPTOACC peripheral on Silabs devices.
 *
 *  Copyright (C) 2017, Silicon Labs, http://www.silabs.com
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/*
 * This file includes alternative plugin implementations of various
 * functions in ccm.c using the Secure Element accelerator incorporated
 * in MCU devices from Silicon Laboratories.
 */

#include "em_device.h"

#if defined(CRYPTOACC_PRESENT)

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_AES_C)
#if defined(MBEDTLS_CCM_ALT) && defined(MBEDTLS_CCM_C)
#include "cryptoacc_management.h"
#include "sx_aes.h"
#include "sx_errors.h"
#include "cryptolib_def.h"
#include "mbedtls/ccm.h"
#include "mbedtls/aes.h"
#include <string.h>

/*
 * Initialize CCM context
 */
void mbedtls_ccm_init( mbedtls_ccm_context *ctx )
{
    if( ctx == NULL ) {
        return;
    }

    memset( ctx, 0, sizeof( mbedtls_ccm_context ) );
}

/*
 * Clear CCM context
 */
void mbedtls_ccm_free( mbedtls_ccm_context *ctx )
{
    if( ctx == NULL ) {
        return;
    }

    memset( ctx, 0, sizeof( mbedtls_ccm_context ) );
}

/*
 * CCM key schedule
 */
int mbedtls_ccm_setkey( mbedtls_ccm_context *ctx,
                        mbedtls_cipher_id_t cipher,
                        const unsigned char *key,
                        unsigned int keybits )
{
    if( ctx == NULL || key == NULL ) {
        return( MBEDTLS_ERR_CCM_BAD_INPUT );
    }

    memset( ctx, 0, sizeof( mbedtls_ccm_context ) );

    if ( cipher != MBEDTLS_CIPHER_ID_AES ) {
        return MBEDTLS_ERR_CCM_BAD_INPUT;
    }

    if ( ( 128UL != keybits ) && ( 192UL != keybits ) && ( 256UL != keybits ) ) {
        /* Unsupported key size */
        return( MBEDTLS_ERR_CCM_BAD_INPUT );
    }

    ctx->keybits = keybits;
    memcpy(ctx->key, key, keybits/8);

    return 0;
}

int mbedtls_ccm_encrypt_and_tag( mbedtls_ccm_context *ctx, size_t length,
                         const unsigned char *iv, size_t iv_len,
                         const unsigned char *add, size_t add_len,
                         const unsigned char *input, unsigned char *output,
                         unsigned char *tag, size_t tag_len )
{
    int status;
    unsigned char q;
    uint32_t sx_ret;
    block_t key;
    block_t aad_block;
    block_t tag_block;
    block_t nonce;
    block_t data_in;
    block_t data_out;

    if( ctx == NULL || tag == NULL || iv == NULL ) {
        return ( MBEDTLS_ERR_CCM_BAD_INPUT );
    }
    if (add_len > 0 && add == NULL ) {
        return ( MBEDTLS_ERR_CCM_BAD_INPUT );
    }
    if( length > 0 && (input == NULL || output == NULL) ) {
        return ( MBEDTLS_ERR_CCM_BAD_INPUT );
    }

    if ( ctx->keybits != 128UL && ctx->keybits != 192UL && ctx->keybits != 256UL) {
        return MBEDTLS_ERR_CCM_BAD_INPUT;
    }

    /* Test for invalid (too long) message length. This test is included here because
       the SE does not implement the test. When the SE ultimately implements the test
       the following test can be removed.  */
    q = 16 - 1 - (unsigned char) iv_len;
    if ( (q < sizeof(length)) && (length >= (1UL<<(q*8))) )
        return( MBEDTLS_ERR_CCM_BAD_INPUT );

    if( tag_len == 2 || tag_len > 16 || tag_len % 2 != 0 )
        return( MBEDTLS_ERR_CCM_BAD_INPUT );

    /* Also implies q is within bounds */
    if( iv_len < 7 || iv_len > 13 )
        return( MBEDTLS_ERR_CCM_BAD_INPUT );

    if( add_len > 0xFF00 ) {
        return( MBEDTLS_ERR_CCM_BAD_INPUT );
    }

    if ((uint32_t)output + length > RAM_MEM_END) {
        return MBEDTLS_ERR_CCM_BAD_INPUT;
    }

    key = block_t_convert(ctx->key, ctx->keybits / 8);
    aad_block = block_t_convert(add, add_len);
    tag_block = block_t_convert(tag, tag_len);
    nonce = block_t_convert(iv, iv_len);
    data_in = block_t_convert(input, length);
    data_out = block_t_convert(output, length);

    status = cryptoacc_management_acquire();
    if (status != 0) {
        return status;
    }
    sx_ret = sx_aes_ccm_encrypt((const block_t *)&key, (const block_t *)&data_in, &data_out,
                                (const block_t *)&nonce, &tag_block, (const block_t *)&aad_block);
    cryptoacc_management_release();

    if (sx_ret != CRYPTOLIB_SUCCESS) {
        return MBEDTLS_ERR_CCM_BAD_INPUT;
    } else {
        return 0;
    }
}

int mbedtls_ccm_auth_decrypt( mbedtls_ccm_context *ctx, size_t length,
                      const unsigned char *iv, size_t iv_len,
                      const unsigned char *add, size_t add_len,
                      const unsigned char *input, unsigned char *output,
                      const unsigned char *tag, size_t tag_len )
{
    int status;
    unsigned char q;
    uint32_t sx_ret;
    block_t key;
    block_t aad_block;
    block_t tag_block;
    block_t nonce;
    block_t data_in;
    block_t data_out;

    if( ctx == NULL || tag == NULL || iv == NULL ) {
        return ( MBEDTLS_ERR_CCM_BAD_INPUT );
    }
    if (add_len > 0 && add == NULL ) {
        return ( MBEDTLS_ERR_CCM_BAD_INPUT );
    }
    if( length > 0 && (input == NULL || output == NULL) ) {
        return ( MBEDTLS_ERR_CCM_BAD_INPUT );
    }

    if ( ctx->keybits != 128UL && ctx->keybits != 192UL && ctx->keybits != 256UL) {
        return MBEDTLS_ERR_CCM_BAD_INPUT;
    }

    /* Test for invalid (too long) message length. This test is included here because
       the SE does not implement the test. When the SE ultimately implements the test
       the following test can be removed.  */
    q = 16 - 1 - (unsigned char) iv_len;
    if ( (q < sizeof(length)) && (length >= (1UL<<(q*8))) )
        return( MBEDTLS_ERR_CCM_BAD_INPUT );

    if( tag_len == 2 || tag_len > 16 || tag_len % 2 != 0 )
        return( MBEDTLS_ERR_CCM_BAD_INPUT );

    /* Also implies q is within bounds */
    if( iv_len < 7 || iv_len > 13 )
        return( MBEDTLS_ERR_CCM_BAD_INPUT );

    if( add_len > 0xFF00 ) {
        return( MBEDTLS_ERR_CCM_BAD_INPUT );
    }

    if ((uint32_t)output + length > RAM_MEM_END) {
        return MBEDTLS_ERR_CCM_BAD_INPUT;
    }

    key = block_t_convert(ctx->key, ctx->keybits / 8);
    aad_block = block_t_convert(add, add_len);
    tag_block = block_t_convert(tag, tag_len);
    nonce = block_t_convert(iv, iv_len);
    data_in = block_t_convert(input, length);
    data_out = block_t_convert(output, length);

    status = cryptoacc_management_acquire();
    if (status != 0) {
        return status;
    }
    sx_ret = sx_aes_ccm_decrypt_verify((const block_t *)&key, (const block_t *)&data_in, &data_out,
                                       (const block_t *)&nonce, (const block_t *)&tag_block, (const block_t *)&aad_block);
    cryptoacc_management_release();

    if (sx_ret == CRYPTOLIB_SUCCESS) {
        return 0;
    } else if ( sx_ret == CRYPTOLIB_INVALID_SIGN_ERR ) {
        memset( output, 0, length );
        return MBEDTLS_ERR_CCM_AUTH_FAILED;
    } else {
        return MBEDTLS_ERR_CCM_BAD_INPUT;
    }
}

#endif /* CRYPTOACC_PRESENT */

#endif /* MBEDTLS_CCM_ALT && MBEDTLS_CCM_C */

#endif /* MBEDTLS_AES_C */
