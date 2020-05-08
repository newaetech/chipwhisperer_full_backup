/***************************************************************************//**
 * @file
 * @brief AES abstraction based on Secure Element
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

#include "mbedtls/aes.h"

#if defined(MBEDTLS_AES_C)
#if defined(MBEDTLS_AES_ALT)

#include "em_device.h"

#if defined(SEMAILBOX_PRESENT)

#include "em_se.h"
#include "em_core.h"
#include "se_management.h"
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
    SE_Response_t command_status;

    if( ctx == NULL || input == NULL || output == NULL ) {
        return ( MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH );
    }

    if ( ctx->keybits != 128UL && ctx->keybits != 192UL && ctx->keybits != 256UL) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    SE_Command_t command = SE_COMMAND_DEFAULT((mode == MBEDTLS_AES_ENCRYPT ? SE_COMMAND_AES_ENCRYPT : SE_COMMAND_AES_DECRYPT) | SE_COMMAND_OPTION_MODE_ECB | SE_COMMAND_OPTION_CONTEXT_WHOLE);
    SE_DataTransfer_t key = SE_DATATRANSFER_DEFAULT(ctx->key, (ctx->keybits / 8));
    SE_DataTransfer_t in = SE_DATATRANSFER_DEFAULT((void*)input, 16);
    SE_DataTransfer_t out = SE_DATATRANSFER_DEFAULT(output, 16);

    SE_addDataInput(&command, &key);
    SE_addDataInput(&command, &in);
    SE_addDataOutput(&command, &out);
    SE_addParameter(&command, (ctx->keybits / 8));
    SE_addParameter(&command, 16);

    int status = se_management_acquire();
    if (status != 0) {
        return status;
    }

    SE_executeCommand(&command);
    command_status = SE_readCommandResponse();

    se_management_release();

    if ( command_status == SE_RESPONSE_OK ) {
        return 0;
    } else {
        return (int)command_status;
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
    SE_Response_t command_status;

    if( ctx == NULL || input == NULL || output == NULL || iv == NULL ) {
        return ( MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH );
    }

    /* Input length must be a multiple of 16 bytes which is the AES block
       length. */
    if( length & 0xf ) {
        return( MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH );
    }

    if ( ctx->keybits != 128UL && ctx->keybits != 192UL && ctx->keybits != 256UL) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    SE_Command_t command = SE_COMMAND_DEFAULT((mode == MBEDTLS_AES_ENCRYPT ? SE_COMMAND_AES_ENCRYPT : SE_COMMAND_AES_DECRYPT) | SE_COMMAND_OPTION_MODE_CBC | SE_COMMAND_OPTION_CONTEXT_ADD);
    SE_DataTransfer_t key = SE_DATATRANSFER_DEFAULT(ctx->key, (ctx->keybits / 8));
    SE_DataTransfer_t iv_in = SE_DATATRANSFER_DEFAULT(iv, 16);
    SE_DataTransfer_t iv_out = SE_DATATRANSFER_DEFAULT(iv, 16);
    SE_DataTransfer_t in = SE_DATATRANSFER_DEFAULT((void*)input, length);
    SE_DataTransfer_t out = SE_DATATRANSFER_DEFAULT(output, length);

    SE_addDataInput(&command, &key);
    SE_addDataInput(&command, &iv_in);
    SE_addDataInput(&command, &in);
    SE_addDataOutput(&command, &out);
    SE_addDataOutput(&command, &iv_out);
    SE_addParameter(&command, (ctx->keybits / 8));
    SE_addParameter(&command, length);

    int status = se_management_acquire();
    if (status != 0) {
        return status;
    }

    SE_executeCommand(&command);
    command_status = SE_readCommandResponse();

    se_management_release();

    if ( command_status == SE_RESPONSE_OK ) {
        return 0;
    } else {
        return (int)command_status;
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
    size_t n = iv_off ? *iv_off : 0;
    size_t processed = 0;
    SE_Response_t command_status = SE_RESPONSE_OK;

    if( ctx == NULL || input == NULL || output == NULL || iv == NULL ) {
        return ( MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH );
    }

    if ( ctx->keybits != 128UL && ctx->keybits != 192UL && ctx->keybits != 256UL) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

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
                SE_Command_t command = SE_COMMAND_DEFAULT((mode == MBEDTLS_AES_ENCRYPT ? SE_COMMAND_AES_ENCRYPT : SE_COMMAND_AES_DECRYPT) | SE_COMMAND_OPTION_MODE_CFB | SE_COMMAND_OPTION_CONTEXT_ADD);
                SE_DataTransfer_t key = SE_DATATRANSFER_DEFAULT(ctx->key, (ctx->keybits / 8));
                SE_DataTransfer_t iv_in = SE_DATATRANSFER_DEFAULT(iv, 16);
                SE_DataTransfer_t iv_out = SE_DATATRANSFER_DEFAULT(iv, 16);
                SE_DataTransfer_t in = SE_DATATRANSFER_DEFAULT((void*)&input[processed], iterations * 16);
                SE_DataTransfer_t out = SE_DATATRANSFER_DEFAULT(&output[processed], iterations * 16);

                SE_addDataInput(&command, &key);
                SE_addDataInput(&command, &iv_in);
                SE_addDataInput(&command, &in);
                SE_addDataOutput(&command, &out);
                SE_addDataOutput(&command, &iv_out);
                SE_addParameter(&command, (ctx->keybits / 8));
                SE_addParameter(&command, iterations * 16);

                int status = se_management_acquire();
                if (status != 0) {
                    return status;
                }

                SE_executeCommand(&command);
                command_status = SE_readCommandResponse();

                se_management_release();
                processed += iterations * 16;
            }

            if ( command_status != SE_RESPONSE_OK ) {
                return (int)command_status;
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

    if ( command_status == SE_RESPONSE_OK ) {
        return 0;
    } else {
        return (int)command_status;
    }
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

    if ( ctx->keybits != 128UL && ctx->keybits != 192UL && ctx->keybits != 256UL) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    while( length-- )
    {
        memcpy( ov, iv, 16 );
        if ( (ret = mbedtls_aes_crypt_ecb( ctx, MBEDTLS_AES_ENCRYPT, iv, iv ) ) != 0 ) {
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
    size_t n = nc_off ? *nc_off : 0;
    size_t processed = 0;
    SE_Response_t command_status = SE_RESPONSE_OK;

    if( ctx == NULL || input == NULL || output == NULL || nonce_counter == NULL || stream_block == NULL ) {
        return ( MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH );
    }

    if ( ctx->keybits != 128UL && ctx->keybits != 192UL && ctx->keybits != 256UL) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

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
                SE_Command_t command = SE_COMMAND_DEFAULT(SE_COMMAND_AES_ENCRYPT | SE_COMMAND_OPTION_MODE_CTR | SE_COMMAND_OPTION_CONTEXT_ADD);
                SE_DataTransfer_t key = SE_DATATRANSFER_DEFAULT(ctx->key, (ctx->keybits / 8));
                SE_DataTransfer_t iv_in = SE_DATATRANSFER_DEFAULT(nonce_counter, 16);
                SE_DataTransfer_t iv_out = SE_DATATRANSFER_DEFAULT(nonce_counter, 16);
                SE_DataTransfer_t in = SE_DATATRANSFER_DEFAULT((void*)&input[processed], iterations * 16);
                SE_DataTransfer_t out = SE_DATATRANSFER_DEFAULT(&output[processed], iterations * 16);

                SE_addDataInput(&command, &key);
                SE_addDataInput(&command, &iv_in);
                SE_addDataInput(&command, &in);
                SE_addDataOutput(&command, &out);
                SE_addDataOutput(&command, &iv_out);
                SE_addParameter(&command, (ctx->keybits / 8));
                SE_addParameter(&command, iterations * 16);

                int status = se_management_acquire();
                if (status != 0) {
                    return status;
                }

                SE_executeCommand(&command);
                command_status = SE_readCommandResponse();

                se_management_release();
                processed += iterations * 16;
            }

            if ( command_status != SE_RESPONSE_OK ) {
                return (int)command_status;
            }

            while ( length - processed > 0 ) {
                if ( n == 0 ) {
                    // Get a new stream block
                    mbedtls_aes_crypt_ecb(ctx, MBEDTLS_AES_ENCRYPT, nonce_counter, stream_block);
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

    if ( command_status == SE_RESPONSE_OK ) {
        return 0;
    } else {
        return (int)command_status;
    }
}
#endif /* MBEDTLS_CIPHER_MODE_CTR */

#endif /* SEMAILBOX_PRESENT */

#endif /* MBEDTLS_AES_ALT */

#endif /* MBEDTLS_AES_C */
