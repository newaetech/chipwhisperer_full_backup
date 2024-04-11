/***************************************************************************//**
 * @file
 * @brief AES-GCM abstraction via Silicon Labs SE
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
 * functions in gcm.c using the SE accelerator incorporated
 * in MCU devices from Silicon Laboratories.
 */

#include "em_device.h"

#if defined(SEMAILBOX_PRESENT)

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_GCM_ALT) && defined(MBEDTLS_GCM_C)
#include "se_management.h"
#include "mbedtls/gcm.h"
#include "mbedtls/aes.h"
#include "gcm_alt.h"
#include <string.h>

/* Implementation that should never be optimized out by the compiler */
static void mbedtls_zeroize( void *v, size_t n ) {
    volatile unsigned char *p = v; while( n-- ) *p++ = 0;
}

static void sx_math_u64_to_u8array(uint64_t in, uint8_t *out)
{
    uint32_t i = 0;
    for(i = 0; i < 8; i++) out[7-i] = (in >>  8*i)&0xFF;
}

/*
 * Initialize a context
 */
void mbedtls_gcm_init( mbedtls_gcm_context *ctx )
{
    memset( ctx, 0, sizeof( mbedtls_gcm_context ) );
}

int mbedtls_gcm_setkey( mbedtls_gcm_context *ctx,
                        mbedtls_cipher_id_t cipher,
                        const unsigned char *key,
                        unsigned int keybits )
{
    const mbedtls_cipher_info_t *cipher_info;
    int ret;

    if( ctx == NULL || key == NULL ) {
        return( MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA );
    }

    cipher_info = mbedtls_cipher_info_from_values( cipher, keybits, MBEDTLS_MODE_ECB );
    if( cipher_info == NULL )
        return( MBEDTLS_ERR_GCM_BAD_INPUT );

    if( cipher_info->block_size != 16 )
        return( MBEDTLS_ERR_GCM_BAD_INPUT );

    switch( cipher_info->type )
    {
        case MBEDTLS_CIPHER_AES_128_ECB:
        case MBEDTLS_CIPHER_AES_192_ECB:
        case MBEDTLS_CIPHER_AES_256_ECB:
            break;
        default:
            return( MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA );
    }

    /* Store key in gcm context */
    ctx->keybits = keybits;
    memcpy(ctx->key, key, keybits/8);

    mbedtls_cipher_free( &ctx->cipher_ctx );

    if( ( ret = mbedtls_cipher_setup( &ctx->cipher_ctx, cipher_info ) ) != 0 )
        return( ret );

    if( ( ret = mbedtls_cipher_setkey( &ctx->cipher_ctx, key, keybits,
                               MBEDTLS_ENCRYPT ) ) != 0 )
    {
        return( ret );
    }

    return( 0 );
}

int mbedtls_gcm_starts( mbedtls_gcm_context *ctx,
                int mode,
                const unsigned char *iv,
                size_t iv_len,
                const unsigned char *add,
                size_t add_len )
{
    int status;
    SE_Response_t se_response;

    /* Check input parameters. */
    if( ctx == NULL || iv == NULL ) {
        return ( MBEDTLS_ERR_GCM_BAD_INPUT );
    }
    if (add_len > 0 && add == NULL ) {
        return ( MBEDTLS_ERR_GCM_BAD_INPUT );
    }
    if ( (iv_len != 12) ||
         /* AD are limited to 2^64 bits, so 2^61 bytes */
         ( (uint64_t) add_len ) >> 61 != 0 )
    {
        return( MBEDTLS_ERR_GCM_BAD_INPUT );
    }

    /* Store input in context data structure. */
    ctx->mode = mode;
    ctx->len = 0;
    ctx->add_len = add_len;
    ctx->last_op = false;

    if( add_len > 0 ) {
        // Start with encryption
        // Need to do encryption twice: once to create the context, the other to pre-compute the tag in case there's no more data coming
        // (SE doesn't support a type of 'finalize' command. All operations with 'END' set need to contain some data.)
        SE_Command_t gcm_cmd_enc = SE_COMMAND_DEFAULT(SE_COMMAND_AES_GCM_ENCRYPT | SE_COMMAND_OPTION_CONTEXT_START);
        SE_Command_t gcm_cmd_enc_full = SE_COMMAND_DEFAULT(SE_COMMAND_AES_GCM_ENCRYPT | SE_COMMAND_OPTION_CONTEXT_WHOLE);

        SE_DataTransfer_t key_in = SE_DATATRANSFER_DEFAULT(ctx->key, ctx->keybits / 8);
        SE_DataTransfer_t iv_in = SE_DATATRANSFER_DEFAULT((void*)iv, iv_len);
        SE_DataTransfer_t aad_in = SE_DATATRANSFER_DEFAULT((void*)add, add_len);

        SE_DataTransfer_t key_in_full = SE_DATATRANSFER_DEFAULT(ctx->key, ctx->keybits / 8);
        SE_DataTransfer_t iv_in_full = SE_DATATRANSFER_DEFAULT((void*)iv, iv_len);
        SE_DataTransfer_t aad_in_full = SE_DATATRANSFER_DEFAULT((void*)add, add_len);

        SE_DataTransfer_t ctx_out = SE_DATATRANSFER_DEFAULT(ctx->se_ctx_enc, sizeof(ctx->se_ctx_enc));
        SE_DataTransfer_t tag_out = SE_DATATRANSFER_DEFAULT(ctx->tagbuf, sizeof(ctx->tagbuf));

        SE_addDataInput(&gcm_cmd_enc, &key_in);
        SE_addDataInput(&gcm_cmd_enc, &iv_in);
        SE_addDataInput(&gcm_cmd_enc, &aad_in);

        SE_addDataInput(&gcm_cmd_enc_full, &key_in_full);
        SE_addDataInput(&gcm_cmd_enc_full, &iv_in_full);
        SE_addDataInput(&gcm_cmd_enc_full, &aad_in_full);

        SE_addDataOutput(&gcm_cmd_enc, &ctx_out);
        SE_addDataOutput(&gcm_cmd_enc_full, &tag_out);

        SE_addParameter(&gcm_cmd_enc, ctx->keybits / 8);
        SE_addParameter(&gcm_cmd_enc, add_len);
        SE_addParameter(&gcm_cmd_enc, 0);

        SE_addParameter(&gcm_cmd_enc_full, ctx->keybits / 8);
        SE_addParameter(&gcm_cmd_enc_full, add_len);
        SE_addParameter(&gcm_cmd_enc_full, 0);

        status = se_management_acquire();
        if (status != 0) {
            return status;
        }
        /* Execute GCM operation */
        SE_executeCommand(&gcm_cmd_enc_full);
        se_response = SE_readCommandResponse();
        SE_executeCommand(&gcm_cmd_enc);
        se_response |= SE_readCommandResponse();

        se_management_release();

        // Continue with decryption if needed
        if (ctx->mode == MBEDTLS_GCM_DECRYPT) {
            SE_Command_t gcm_cmd_dec = SE_COMMAND_DEFAULT(SE_COMMAND_AES_GCM_DECRYPT | SE_COMMAND_OPTION_CONTEXT_START);

            SE_DataTransfer_t key_in_dec = SE_DATATRANSFER_DEFAULT(ctx->key, ctx->keybits / 8);
            SE_DataTransfer_t iv_in_dec = SE_DATATRANSFER_DEFAULT((void*)iv, iv_len);
            SE_DataTransfer_t aad_in_dec = SE_DATATRANSFER_DEFAULT((void*)add, add_len);

            SE_DataTransfer_t ctx_out_dec = SE_DATATRANSFER_DEFAULT(ctx->se_ctx_dec, sizeof(ctx->se_ctx_dec));

            SE_addDataInput(&gcm_cmd_dec, &key_in_dec);
            SE_addDataInput(&gcm_cmd_dec, &iv_in_dec);
            SE_addDataInput(&gcm_cmd_dec, &aad_in_dec);

            SE_addDataOutput(&gcm_cmd_dec, &ctx_out_dec);

            SE_addParameter(&gcm_cmd_dec, ctx->keybits / 8);
            SE_addParameter(&gcm_cmd_dec, add_len);
            SE_addParameter(&gcm_cmd_dec, 0);

            status = se_management_acquire();
            if (status != 0) {
                return status;
            }
            /* Execute GCM operation */
            SE_executeCommand(&gcm_cmd_dec);
            se_response = SE_readCommandResponse();
            se_management_release();
        }

        if (se_response == SE_RESPONSE_OK) {
            return( 0 );
        } else {
            return( MBEDTLS_ERR_AES_HW_ACCEL_FAILED );
        }
    } else {
        memcpy(ctx->se_ctx_enc, iv, iv_len);
    }

    return 0;
}

int mbedtls_gcm_update( mbedtls_gcm_context *ctx,
                size_t length,
                const unsigned char *input,
                unsigned char *output )
{
    int status;
    SE_Response_t se_response;
    uint8_t lena_lenc[16];

    if( ctx == NULL ) {
        return ( MBEDTLS_ERR_GCM_BAD_INPUT );
    }
    if (length == 0) {
        return 0;
    }
    if( length > 0 && (input == NULL || output == NULL) ) {
        return ( MBEDTLS_ERR_GCM_BAD_INPUT );
    }
    /* Total length is restricted to 2^39 - 256 bits, ie 2^36 - 2^5 bytes
     * Also check for possible overflow */
    if( ctx->len + length < ctx->len ||
        (uint64_t) ctx->len + length > 0xFFFFFFFE0ull )
    {
        return( MBEDTLS_ERR_GCM_BAD_INPUT );
    }

    if( ctx->last_op == true ) {
        // We've already closed the input stream, no way back.
        return ( MBEDTLS_ERR_GCM_BAD_INPUT );
    }

    // Approach:
    // * Encryption: encrypt regularly with context store. If input length is not a block multiple, close the operation and store the resulting tag in a tag buffer.
    // * Decryption: decrypt regularly with context store. For each decryption, re-encrypt the decrypted data with context store. If input length is not a block multiple, close both operations and store the tag from the re-encrypt in a tag buffer.
    // * In both cases, the (re-)encryption is done twice: once assuming there is more data to follow, and once assuming this is the final block.
    //   Explanation: SE doesn't support a type of 'finalize' command. All operations with 'END' set need to contain some data.

    // Figure out whether we'll be closing out
    bool first_op = (ctx->add_len == 0 && ctx->len == 0) ? true : false;

    if( length % 16 != 0) {
        // Indicate that this is our last op
        ctx->last_op = true;
    }

    if (first_op && ctx->last_op) {
        // optimisation: delegate to all-in-one handler
        return mbedtls_gcm_crypt_and_tag( ctx, ctx->mode, length,
                                          ctx->se_ctx_enc, 12,
                                          NULL, 0,
                                          input, output,
                                          12, ctx->tagbuf );
    }

    sx_math_u64_to_u8array(ctx->add_len<<3, &lena_lenc[0]);
    sx_math_u64_to_u8array((ctx->len + length)<<3, &lena_lenc[8]);

    // Need to be sure we can get the SE before starting to change any context variables
    status = se_management_acquire();
    if (status != 0) {
        return status;
    }

    ctx->len += length;

    if (ctx->mode == MBEDTLS_GCM_DECRYPT) {
        // Run decryption first
        SE_Command_t gcm_cmd_dec = SE_COMMAND_DEFAULT(SE_COMMAND_AES_GCM_DECRYPT | (first_op ? SE_COMMAND_OPTION_CONTEXT_START : (ctx->last_op ? SE_COMMAND_OPTION_CONTEXT_END : SE_COMMAND_OPTION_CONTEXT_ADD)));

        SE_DataTransfer_t key_in_dec = SE_DATATRANSFER_DEFAULT(ctx->key, ctx->keybits / 8);
        SE_DataTransfer_t iv_ctx_in_dec = SE_DATATRANSFER_DEFAULT((first_op ? ctx->se_ctx_enc : ctx->se_ctx_dec), (first_op ? 12 : sizeof(ctx->se_ctx_dec)));
        SE_DataTransfer_t data_in_dec = SE_DATATRANSFER_DEFAULT((void*)input, length);
        SE_DataTransfer_t lenalenc_in_dec = SE_DATATRANSFER_DEFAULT(lena_lenc, sizeof(lena_lenc));
        SE_DataTransfer_t data_out_dec = SE_DATATRANSFER_DEFAULT(output, length);
        SE_DataTransfer_t ctx_out_dec = SE_DATATRANSFER_DEFAULT(ctx->se_ctx_dec, sizeof(ctx->se_ctx_dec));
        SE_DataTransfer_t mac_in_dec = SE_DATATRANSFER_DEFAULT(ctx->tagbuf, sizeof(ctx->tagbuf));

        SE_addDataInput(&gcm_cmd_dec, &key_in_dec);
        SE_addDataInput(&gcm_cmd_dec, &iv_ctx_in_dec);
        SE_addDataInput(&gcm_cmd_dec, &data_in_dec);

        SE_addDataOutput(&gcm_cmd_dec, &data_out_dec);
        if (!ctx->last_op) {
            SE_addDataOutput(&gcm_cmd_dec, &ctx_out_dec);
        } else {
            SE_addDataInput(&gcm_cmd_dec, &lenalenc_in_dec);
            SE_addDataInput(&gcm_cmd_dec, &mac_in_dec);
        }

        SE_addParameter(&gcm_cmd_dec, ctx->keybits / 8);
        SE_addParameter(&gcm_cmd_dec, 0);
        SE_addParameter(&gcm_cmd_dec, length);

        SE_executeCommand(&gcm_cmd_dec);
        se_response = SE_readCommandResponse();
    }

    if (!ctx->last_op) {
        // we need to do the final calculation first, such that we keep the input context intact
        SE_Command_t gcm_cmd_enc_final = SE_COMMAND_DEFAULT(SE_COMMAND_AES_GCM_ENCRYPT | (first_op ? SE_COMMAND_OPTION_CONTEXT_WHOLE : SE_COMMAND_OPTION_CONTEXT_END));

        SE_DataTransfer_t key_in_enc_final = SE_DATATRANSFER_DEFAULT(ctx->key, ctx->keybits / 8);
        SE_DataTransfer_t iv_ctx_in_enc_final = SE_DATATRANSFER_DEFAULT(ctx->se_ctx_enc, (first_op ? 12 : sizeof(ctx->se_ctx_enc)));
        SE_DataTransfer_t lenalenc_in_enc_final = SE_DATATRANSFER_DEFAULT(lena_lenc, sizeof(lena_lenc));
        SE_DataTransfer_t data_in_enc_final = SE_DATATRANSFER_DEFAULT(ctx->mode == MBEDTLS_GCM_ENCRYPT ? (void*)input : (void*)output, length);
        SE_DataTransfer_t data_out_enc_final = SE_DATATRANSFER_DEFAULT(NULL, length);
        data_out_enc_final.length |= SE_DATATRANSFER_DISCARD;
        SE_DataTransfer_t tag_out_final = SE_DATATRANSFER_DEFAULT(ctx->tagbuf, sizeof(ctx->tagbuf));

        SE_addDataInput(&gcm_cmd_enc_final, &key_in_enc_final);
        SE_addDataInput(&gcm_cmd_enc_final, &iv_ctx_in_enc_final);
        SE_addDataInput(&gcm_cmd_enc_final, &data_in_enc_final);

        if (!first_op) {
            SE_addDataInput(&gcm_cmd_enc_final, &lenalenc_in_enc_final);
        }

        SE_addDataOutput(&gcm_cmd_enc_final, &data_out_enc_final);
        SE_addDataOutput(&gcm_cmd_enc_final, &tag_out_final);

        SE_addParameter(&gcm_cmd_enc_final, ctx->keybits / 8);
        SE_addParameter(&gcm_cmd_enc_final, 0);
        SE_addParameter(&gcm_cmd_enc_final, length);

        SE_executeCommand(&gcm_cmd_enc_final);
        se_response = SE_readCommandResponse();
    }

    SE_Command_t gcm_cmd_enc = SE_COMMAND_DEFAULT(SE_COMMAND_AES_GCM_ENCRYPT | (first_op ? SE_COMMAND_OPTION_CONTEXT_START : (ctx->last_op ? SE_COMMAND_OPTION_CONTEXT_END : SE_COMMAND_OPTION_CONTEXT_ADD)));
    SE_DataTransfer_t key_in_enc = SE_DATATRANSFER_DEFAULT(ctx->key, ctx->keybits / 8);
    SE_DataTransfer_t iv_ctx_in_enc = SE_DATATRANSFER_DEFAULT(ctx->se_ctx_enc, (first_op ? 12 : sizeof(ctx->se_ctx_enc)));
    SE_DataTransfer_t lenalenc_in_enc = SE_DATATRANSFER_DEFAULT(lena_lenc, sizeof(lena_lenc));
    SE_DataTransfer_t data_in_enc = SE_DATATRANSFER_DEFAULT(ctx->mode == MBEDTLS_GCM_ENCRYPT ? (void*)input : (void*)output, length);

    SE_DataTransfer_t data_out_enc = SE_DATATRANSFER_DEFAULT(output, length);
    if (ctx->mode == MBEDTLS_GCM_DECRYPT) {
        data_out_enc.data = NULL;
        data_out_enc.length |= SE_DATATRANSFER_DISCARD;
    }

    SE_DataTransfer_t tag_out_enc = SE_DATATRANSFER_DEFAULT(ctx->tagbuf, sizeof(ctx->tagbuf));
    SE_DataTransfer_t ctx_out_enc = SE_DATATRANSFER_DEFAULT(ctx->se_ctx_enc, sizeof(ctx->se_ctx_enc));

    SE_addDataInput(&gcm_cmd_enc, &key_in_enc);
    SE_addDataInput(&gcm_cmd_enc, &iv_ctx_in_enc);
    SE_addDataInput(&gcm_cmd_enc, &data_in_enc);

    if (ctx->last_op) {
        SE_addDataInput(&gcm_cmd_enc, &lenalenc_in_enc);
    }

    SE_addDataOutput(&gcm_cmd_enc, &data_out_enc);

    if (ctx->last_op) {
        SE_addDataOutput(&gcm_cmd_enc, &tag_out_enc);
    } else {
        SE_addDataOutput(&gcm_cmd_enc, &ctx_out_enc);
    }

    SE_addParameter(&gcm_cmd_enc, ctx->keybits / 8);
    SE_addParameter(&gcm_cmd_enc, 0);
    SE_addParameter(&gcm_cmd_enc, length);

    SE_executeCommand(&gcm_cmd_enc);
    se_response = SE_readCommandResponse();

    se_management_release();

    if (se_response == SE_RESPONSE_OK) {
        return( 0 );
    } else {
        mbedtls_zeroize( output, length );
        return( MBEDTLS_ERR_AES_HW_ACCEL_FAILED );
    }
}

int mbedtls_gcm_finish( mbedtls_gcm_context *ctx,
                unsigned char *tag,
                size_t tag_len )
{
    if( ctx == NULL || tag == NULL ) {
        return ( MBEDTLS_ERR_GCM_BAD_INPUT );
    }

    if( tag_len < 4 || tag_len > 16 ) {
        return ( MBEDTLS_ERR_GCM_BAD_INPUT );
    }

    if (ctx->add_len == 0 && ctx->len == 0) {
        return mbedtls_gcm_crypt_and_tag(ctx, MBEDTLS_GCM_ENCRYPT,
                                         0, ctx->se_ctx_enc, 12,
                                         NULL, 0,
                                         NULL, NULL,
                                         tag_len, tag);
    }

    memcpy(tag, ctx->tagbuf, tag_len);
    return( 0 );
}

int mbedtls_gcm_crypt_and_tag( mbedtls_gcm_context *ctx,
                       int mode,
                       size_t length,
                       const unsigned char *iv,
                       size_t iv_len,
                       const unsigned char *add,
                       size_t add_len,
                       const unsigned char *input,
                       unsigned char *output,
                       size_t tag_len,
                       unsigned char *tag )
{
    SE_Response_t se_response;
    uint8_t tagbuf[16];
    int status;

    /* Check input parameters. */
    if( ctx == NULL || iv == NULL ) {
        return ( MBEDTLS_ERR_GCM_BAD_INPUT );
    }
    if (add_len > 0 && add == NULL ) {
        return ( MBEDTLS_ERR_GCM_BAD_INPUT );
    }

    if( length > 0 && input == NULL ) {
        return ( MBEDTLS_ERR_GCM_BAD_INPUT );
    }

    if( tag_len < 4 || tag_len > 16 ) {
        return ( MBEDTLS_ERR_GCM_BAD_INPUT );
    }

    if ( /* IV length is required to be 96 bits for SE.*/
         (iv_len != 12)
         /* AD are limited to 2^64 bits, so 2^61 bytes */
         || ( ( (uint64_t) add_len ) >> 61 != 0 ) ) {
        return( MBEDTLS_ERR_GCM_BAD_INPUT );
    }

    if( mode == MBEDTLS_GCM_DECRYPT ) {
        // Extract plaintext first
        SE_Command_t gcm_cmd = SE_COMMAND_DEFAULT(SE_COMMAND_AES_GCM_DECRYPT | ((tag_len & 0xFF) << 8));

        SE_DataTransfer_t key_in = SE_DATATRANSFER_DEFAULT(ctx->key, ctx->keybits / 8);
        SE_DataTransfer_t iv_in = SE_DATATRANSFER_DEFAULT((void*)iv, iv_len);
        SE_DataTransfer_t aad_in = SE_DATATRANSFER_DEFAULT((void*)add, add_len);
        SE_DataTransfer_t data_in = SE_DATATRANSFER_DEFAULT((void*)input, length);
        SE_DataTransfer_t data_out = SE_DATATRANSFER_DEFAULT(output, length);
        if (output == NULL) {
            data_out.length |= SE_DATATRANSFER_DISCARD;
        }
        SE_DataTransfer_t tag_in = SE_DATATRANSFER_DEFAULT(tag, tag_len);

        SE_addDataInput(&gcm_cmd, &key_in);
        SE_addDataInput(&gcm_cmd, &iv_in);
        SE_addDataInput(&gcm_cmd, &aad_in);
        SE_addDataInput(&gcm_cmd, &data_in);
        SE_addDataInput(&gcm_cmd, &tag_in);

        SE_addDataOutput(&gcm_cmd, &data_out);

        SE_addParameter(&gcm_cmd, ctx->keybits / 8);
        SE_addParameter(&gcm_cmd, add_len);
        SE_addParameter(&gcm_cmd, length);

        status = se_management_acquire();
        if (status != 0) {
            return status;
        }
        SE_executeCommand(&gcm_cmd);
        se_response = SE_readCommandResponse();
        se_management_release();

        // Re-encrypt the extracted plaintext to generate the tag to match
        input = output;
        output = NULL;
    }

    SE_Command_t gcm_cmd = SE_COMMAND_DEFAULT(SE_COMMAND_AES_GCM_ENCRYPT);

    SE_DataTransfer_t key_in = SE_DATATRANSFER_DEFAULT(ctx->key, ctx->keybits / 8);
    SE_DataTransfer_t iv_in = SE_DATATRANSFER_DEFAULT((void*)iv, iv_len);
    SE_DataTransfer_t aad_in = SE_DATATRANSFER_DEFAULT((void*)add, add_len);
    SE_DataTransfer_t data_in = SE_DATATRANSFER_DEFAULT((void*)input, length);
    SE_DataTransfer_t data_out = SE_DATATRANSFER_DEFAULT(output, length);
    if (output == NULL) {
        data_out.length |= SE_DATATRANSFER_DISCARD;
    }
    SE_DataTransfer_t mac_out = SE_DATATRANSFER_DEFAULT(tagbuf, sizeof(tagbuf));
    if (tag == NULL) {
        mac_out.length |= SE_DATATRANSFER_DISCARD;
    }

    SE_addDataInput(&gcm_cmd, &key_in);
    SE_addDataInput(&gcm_cmd, &iv_in);
    SE_addDataInput(&gcm_cmd, &aad_in);
    SE_addDataInput(&gcm_cmd, &data_in);

    SE_addDataOutput(&gcm_cmd, &data_out);
    SE_addDataOutput(&gcm_cmd, &mac_out);

    SE_addParameter(&gcm_cmd, ctx->keybits / 8);
    SE_addParameter(&gcm_cmd, add_len);
    SE_addParameter(&gcm_cmd, length);

    status = se_management_acquire();
    if (status != 0) {
        return status;
    }
    /* Execute GCM operation */
    SE_executeCommand(&gcm_cmd);
    se_response = SE_readCommandResponse();
    se_management_release();

    if (se_response == SE_RESPONSE_OK) {
        // For encryption, copy requested tag size to output tag buffer.
        memcpy(tag, tagbuf, tag_len);
        return( 0 );
    } else {
        mbedtls_zeroize( output, length );
        return( MBEDTLS_ERR_AES_HW_ACCEL_FAILED );
    }
}

int mbedtls_gcm_auth_decrypt( mbedtls_gcm_context *ctx,
                      size_t length,
                      const unsigned char *iv,
                      size_t iv_len,
                      const unsigned char *add,
                      size_t add_len,
                      const unsigned char *tag,
                      size_t tag_len,
                      const unsigned char *input,
                      unsigned char *output )
{
    SE_Response_t se_response;
    int status;

    /* Check input parameters. */
    if( ctx == NULL || tag == NULL || iv == NULL ) {
        return ( MBEDTLS_ERR_GCM_BAD_INPUT );
    }
    if (add_len > 0 && add == NULL ) {
        return ( MBEDTLS_ERR_GCM_BAD_INPUT );
    }

    if( length > 0 && input == NULL ) {
        return ( MBEDTLS_ERR_GCM_BAD_INPUT );
    }

    // NOTE: tag lengths != 16 byte are only supported as of SE FW v1.2.0.
    //   Earlier firmware versions will return an error trying to verify non-16-byte
    //   tags using this function.
    if( tag_len < 4 || tag_len > 16 ) {
        return ( MBEDTLS_ERR_GCM_BAD_INPUT );
    }

    if ( /* IV length is required to be 96 bits for SE.*/
         (iv_len != 12)
         /* AD are limited to 2^64 bits, so 2^61 bytes */
         || ( ( (uint64_t) add_len ) >> 61 != 0 ) ) {
        return( MBEDTLS_ERR_GCM_BAD_INPUT );
    }

    // AES-GCM encryption and decryption are symmetrical. The SE only
    // supports checking tag length of 16 bytes. In order to support
    // smaller tag lengths, the decrypt-and-check routine is implemented
    // as a call to encrypt-and-MAC, and a manual check of the MAC vs the
    // expected MAC on the right tag length.

    SE_Command_t gcm_cmd = SE_COMMAND_DEFAULT(SE_COMMAND_AES_GCM_DECRYPT | ((tag_len & 0xFF) << 8));

    SE_DataTransfer_t key_in = SE_DATATRANSFER_DEFAULT(ctx->key, ctx->keybits / 8);
    SE_DataTransfer_t iv_in = SE_DATATRANSFER_DEFAULT((void*)iv, iv_len);
    SE_DataTransfer_t aad_in = SE_DATATRANSFER_DEFAULT((void*)add, add_len);
    SE_DataTransfer_t data_in = SE_DATATRANSFER_DEFAULT((void*)input, length);
    SE_DataTransfer_t data_out = SE_DATATRANSFER_DEFAULT(output, length);
    if (output == NULL) {
        data_out.length |= SE_DATATRANSFER_DISCARD;
    }
    SE_DataTransfer_t tag_in = SE_DATATRANSFER_DEFAULT((void*)tag, tag_len);

    SE_addDataInput(&gcm_cmd, &key_in);
    SE_addDataInput(&gcm_cmd, &iv_in);
    SE_addDataInput(&gcm_cmd, &aad_in);
    SE_addDataInput(&gcm_cmd, &data_in);
    SE_addDataInput(&gcm_cmd, &tag_in);

    SE_addDataOutput(&gcm_cmd, &data_out);

    SE_addParameter(&gcm_cmd, ctx->keybits / 8);
    SE_addParameter(&gcm_cmd, add_len);
    SE_addParameter(&gcm_cmd, length);

    status = se_management_acquire();
    if (status != 0) {
        return status;
    }
    SE_executeCommand(&gcm_cmd);
    se_response = SE_readCommandResponse();
    se_management_release();

    if (se_response == SE_RESPONSE_OK) {
        return( 0 );
    } else {
        mbedtls_zeroize( output, length );
        if (se_response == SE_RESPONSE_INVALID_SIGNATURE) {
            return( MBEDTLS_ERR_GCM_AUTH_FAILED );
        } else {
            return( MBEDTLS_ERR_AES_HW_ACCEL_FAILED );
        }
    }
}

void mbedtls_gcm_free( mbedtls_gcm_context *ctx )
{
    mbedtls_cipher_free( &ctx->cipher_ctx );
    mbedtls_zeroize( ctx, sizeof( mbedtls_gcm_context ) );
}

#endif /* MBEDTLS_GCM_ALT && MBEDTLS_GCM_C */

#endif /* CRYPTOACC_PRESENT */
