/*
 *  AES abstraction based on the CRYPTOACC peripheral on Silabs devices.
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
 * functions in aes.c using the Secure Element accelerator incorporated
 * in MCU devices from Silicon Laboratories.
 */

/*
 *  The AES block cipher was designed by Vincent Rijmen and Joan Daemen.
 *
 *  http://csrc.nist.gov/encryption/aes/rijndael/Rijndael.pdf
 *  http://csrc.nist.gov/publications/fips/fips197/fips-197.pdf
 */

#include "em_device.h"

#if defined(CRYPTOACC_PRESENT)

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_AES_C)
#if defined(MBEDTLS_AES_ALT)
#include "cryptoacc_management.h"
#include "sx_aes.h"
#include "sx_errors.h"
#include "cryptolib_def.h"
#include "mbedtls/aes.h"
#include <string.h>

/*
 * Initialize AES context
 */
void mbedtls_aes_init( mbedtls_aes_context *ctx )
{
    if( ctx == NULL ) {
        return;
    }

    memset( ctx, 0, sizeof( mbedtls_aes_context ) );
}

/*
 * Clear AES context
 */
void mbedtls_aes_free( mbedtls_aes_context *ctx )
{
    if( ctx == NULL ) {
        return;
    }

    memset( ctx, 0, sizeof( mbedtls_aes_context ) );
}

/*
 * AES key schedule (encryption)
 */
int mbedtls_aes_setkey_enc( mbedtls_aes_context *ctx,
                            const unsigned char *key,
                            unsigned int keybits )
{
    if( ctx == NULL || key == NULL ) {
        return( MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH );
    }

    memset( ctx, 0, sizeof( mbedtls_aes_context ) );

    if ( ( 128UL != keybits ) && ( 192UL != keybits ) && ( 256UL != keybits ) ) {
        /* Unsupported key size */
        return( MBEDTLS_ERR_AES_INVALID_KEY_LENGTH );
    }

    ctx->keybits = keybits;
    memcpy(ctx->key, key, keybits/8);

    return 0;
}

/*
 * AES key schedule (decryption)
 */
int mbedtls_aes_setkey_dec( mbedtls_aes_context *ctx,
                            const unsigned char *key,
                            unsigned int keybits )
{
    return mbedtls_aes_setkey_enc(ctx, key, keybits);
}

/*
 * AES-ECB block encryption/decryption
 */
int mbedtls_aes_crypt_ecb( mbedtls_aes_context *ctx,
                           int mode,
                           const unsigned char input[16],
                           unsigned char output[16] )
{
    int status;
    uint32_t sx_ret;
    sx_aes_mode_t dir = mode == MBEDTLS_AES_ENCRYPT ? ENC : DEC;
    block_t key;
    block_t data_in;
    block_t data_out;

    if( ctx == NULL || input == NULL || output == NULL ) {
        return ( MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH );
    }

    if ( ctx->keybits != 128UL &&
	 ctx->keybits != 192UL &&
	 ctx->keybits != 256UL ) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    key = block_t_convert(ctx->key, ctx->keybits / 8);
    data_in = block_t_convert(input, 16);
    data_out = block_t_convert(output, 16);

    status = cryptoacc_management_acquire();
    if (status != 0) {
        return status;
    }
    if (dir == ENC) {
        sx_ret = sx_aes_ecb_encrypt((const block_t*)&key, (const block_t*)&data_in, &data_out);
    } else {
        sx_ret = sx_aes_ecb_decrypt((const block_t*)&key, (const block_t*)&data_in, &data_out);
    }
    cryptoacc_management_release();

    if (sx_ret != CRYPTOLIB_SUCCESS) {
        return MBEDTLS_ERR_AES_HW_ACCEL_FAILED;
    }
    else {
        return 0;
    }
}

#if defined(MBEDTLS_CIPHER_MODE_CBC)

/*
 * AES-CBC buffer encryption/decryption
 */
int mbedtls_aes_crypt_cbc( mbedtls_aes_context *ctx,
                           int mode,
                           size_t length,
                           unsigned char iv[16],
                           const unsigned char *input,
                           unsigned char *output )
{
    int status;
    uint32_t sx_ret;
    sx_aes_mode_t dir = mode == MBEDTLS_AES_ENCRYPT ? ENC : DEC;
    block_t key;
    block_t iv_block;
    block_t data_in;
    block_t data_out;

    if( ctx == NULL || input == NULL || output == NULL || iv == NULL ) {
        return ( MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH );
    }

    /* Input length must be a multiple of 16 bytes which is the AES block
       length. */
    if( length & 0xf ) {
        return( MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH );
    }

    if ( ctx->keybits != 128UL &&
	 ctx->keybits != 192UL &&
	 ctx->keybits != 256UL) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    key = block_t_convert(ctx->key, ctx->keybits / 8);
    iv_block = block_t_convert(iv, 16);
    data_in = block_t_convert(input, length);
    data_out = block_t_convert(output, length);

    status = cryptoacc_management_acquire();
    if (status != 0) {
        return status;
    }
    if (dir == ENC) {
        sx_ret = sx_aes_cbc_encrypt_update((const block_t *)&key, (const block_t *)&data_in, &data_out, (const block_t *)&iv_block, &iv_block);
    } else {
        sx_ret = sx_aes_cbc_decrypt_update((const block_t *)&key, (const block_t *)&data_in, &data_out, (const block_t *)&iv_block, &iv_block);
    }
    cryptoacc_management_release();

    if (sx_ret != CRYPTOLIB_SUCCESS) {
        return MBEDTLS_ERR_AES_HW_ACCEL_FAILED;
    }
    else {
        return 0;
    }
}
#endif /* MBEDTLS_CIPHER_MODE_CBC */

#if defined(MBEDTLS_CIPHER_MODE_CFB)
/*
 * AES-CFB128 buffer encryption/decryption
 */
int mbedtls_aes_crypt_cfb128( mbedtls_aes_context *ctx,
                              int mode,
                              size_t length,
                              size_t *iv_off,
                              unsigned char iv[16],
                              const unsigned char *input,
                              unsigned char *output )
{
    int status;
    size_t n = iv_off ? *iv_off : 0;
    size_t processed = 0;
    uint32_t sx_ret;
    sx_aes_mode_t dir = mode == MBEDTLS_AES_ENCRYPT ? ENC : DEC;
    block_t key;
    block_t iv_block;
    block_t data_in;
    block_t data_out;

    if( ctx == NULL || input == NULL || output == NULL || iv == NULL ) {
        return ( MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH );
    }

    if ( ctx->keybits != 128UL &&
	 ctx->keybits != 192UL &&
	 ctx->keybits != 256UL) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    key = block_t_convert(ctx->key, ctx->keybits / 8);
    iv_block = block_t_convert(iv, 16);
    while ( processed < length ) {
        if ( n > 0 ) {
            /* start by filling up the IV */
            if( mode == MBEDTLS_AES_ENCRYPT ) {
                iv[n] = output[processed] = (unsigned char)( iv[n] ^ input[processed] );
            } else {
                int c = input[processed];
                output[processed] = (unsigned char)( c ^ iv[n] );
                iv[n] = (unsigned char) c;
            }
            n = ( n + 1 ) & 0x0F;
            processed++;
            continue;
        } else {
            /* process one ore more blocks of data */
            size_t iterations = (length - processed) / 16;

            if ( iterations > 0 ) {
                data_in = block_t_convert(&input[processed], iterations * 16);
                data_out = block_t_convert(&output[processed], iterations * 16);

                status = cryptoacc_management_acquire();
                if (status != 0) {
                    return status;
                }
                if (dir == ENC) {
                    sx_ret = sx_aes_cfb_encrypt_update((const block_t *)&key, (const block_t *)&data_in, &data_out, (const block_t *)&iv_block, &iv_block);
                } else {
                    sx_ret = sx_aes_cfb_decrypt_update((const block_t *)&key, (const block_t *)&data_in, &data_out, (const block_t *)&iv_block, &iv_block);
                }
     	        cryptoacc_management_release();

                if (sx_ret != CRYPTOLIB_SUCCESS) {
                    return MBEDTLS_ERR_AES_HW_ACCEL_FAILED;
                }

                processed += iterations * 16;
            }

            while ( length - processed > 0 ) {
                if ( n == 0 ) {
                    // Need to update the IV but don't have a full block of input to pass to the SE
                    mbedtls_aes_crypt_ecb(ctx, MBEDTLS_AES_ENCRYPT, iv, iv);
                }
                /* Save remainder to iv */
                if( mode == MBEDTLS_AES_ENCRYPT ) {
                    iv[n] = output[processed] = (unsigned char)( iv[n] ^ input[processed] );
                } else {
                    int c = input[processed];
                    output[processed] = (unsigned char)( c ^ iv[n] );
                    iv[n] = (unsigned char) c;
                }
                n = ( n + 1 ) & 0x0F;
                processed++;
            }
        }
    }

    if ( iv_off ) {
        *iv_off = n;
    }

    return 0;
}

/*
 * AES-CFB8 buffer encryption/decryption
 */
int mbedtls_aes_crypt_cfb8( mbedtls_aes_context *ctx,
                            int mode,
                            size_t length,
                            unsigned char iv[16],
                            const unsigned char *input,
                            unsigned char *output )
{
    unsigned char c;
    unsigned char ov[17];
    int ret = 0;

    if( ctx == NULL || input == NULL || output == NULL || iv == NULL ) {
        return ( MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH );
    }

    if ( ctx->keybits != 128UL &&
	 ctx->keybits != 192UL &&
	 ctx->keybits != 256UL) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    while( length-- )
    {
        memcpy( ov, iv, 16 );
        if ( (ret = mbedtls_aes_crypt_ecb( ctx, MBEDTLS_AES_ENCRYPT, iv, iv ))
	     != 0 ) {
            return ret;
        }

        if( mode == MBEDTLS_AES_DECRYPT )
            ov[16] = *input;

        c = *output++ = (unsigned char)( iv[0] ^ *input++ );

        if( mode == MBEDTLS_AES_ENCRYPT )
            ov[16] = c;

        memcpy( iv, ov + 1, 16 );
    }

    return ret;
}
#endif /*MBEDTLS_CIPHER_MODE_CFB */

#if defined(MBEDTLS_CIPHER_MODE_CTR)
/*
 * AES-CTR buffer encryption/decryption
 */
int mbedtls_aes_crypt_ctr( mbedtls_aes_context *ctx,
                           size_t length,
                           size_t *nc_off,
                           unsigned char nonce_counter[16],
                           unsigned char stream_block[16],
                           const unsigned char *input,
                           unsigned char *output )
{
    int status;
    size_t n = nc_off ? *nc_off : 0;
    size_t processed = 0;
    uint32_t sx_ret;
    block_t key;
    block_t iv_block;
    block_t data_in;
    block_t data_out;

    if( ctx == NULL || input == NULL || output == NULL ||
	nonce_counter == NULL || stream_block == NULL ) {
        return ( MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH );
    }

    if ( ctx->keybits != 128UL &&
	 ctx->keybits != 192UL &&
	 ctx->keybits != 256UL) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    key = block_t_convert(ctx->key, ctx->keybits / 8);
    iv_block = block_t_convert(nonce_counter, 16);

    while ( processed < length ) {
        if ( n > 0 ) {
            /* start by filling up the IV */
            output[processed] = (unsigned char)( input[processed] ^ stream_block[n] );
            n = ( n + 1 ) & 0x0F;
            processed++;
            continue;
        } else {
            /* process one or more blocks of data */
            size_t iterations = (length - processed) / 16;

            if ( iterations > 0 ) {
                data_in = block_t_convert(&input[processed], iterations * 16);
                data_out = block_t_convert(&output[processed], iterations * 16);

                status = cryptoacc_management_acquire();
                if (status != 0) {
                    return status;
                }
                // AES-CTR uses the only AES encrypt operation (for both encryption and decryption)
                sx_ret = sx_aes_ctr_encrypt_update((const block_t *)&key, (const block_t *)&data_in, &data_out, (const block_t *)&iv_block, &iv_block);
                cryptoacc_management_release();

                if (sx_ret != CRYPTOLIB_SUCCESS) {
                    return MBEDTLS_ERR_AES_HW_ACCEL_FAILED;
                }

                processed += iterations * 16;
            }

            while ( length - processed > 0 ) {
                if ( n == 0 ) {
                    // Get a new stream block
                    mbedtls_aes_crypt_ecb(ctx, MBEDTLS_AES_ENCRYPT,
					  nonce_counter, stream_block);
                    // increment nonce counter...
                    for(size_t i = 0; i < 16; i++) {
                        nonce_counter[15-i] = nonce_counter[15-i] + 1;
                        if ( nonce_counter[15-i] != 0 ) {
                            break;
                        }
                    }
                }
                /* Save remainder to iv */
                output[processed] = (unsigned char)( input[processed] ^ stream_block[n] );
                n = ( n + 1 ) & 0x0F;
                processed++;
            }
        }
    }

    if ( nc_off ) {
        *nc_off = n;
    }

    return 0;
}
#endif /* MBEDTLS_CIPHER_MODE_CTR */

#endif /* MBEDTLS_AES_ALT */

#endif /* MBEDTLS_AES_C */

#endif /* CRYPTOACC_PRESENT */
