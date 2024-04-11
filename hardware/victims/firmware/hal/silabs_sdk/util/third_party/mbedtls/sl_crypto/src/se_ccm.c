/***************************************************************************//**
 * @file
 * @brief AES-CCM abstraction based on Secure Element
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
 * functions in ccm.c using the Secure Element accelerator incorporated
 * in MCU devices from Silicon Laboratories.
 */

#include "mbedtls/ccm.h"
#include "mbedtls/aes.h"

#if defined(MBEDTLS_AES_C)
#if defined(MBEDTLS_CCM_ALT)

#include "em_device.h"

#if defined(SEMAILBOX_PRESENT)

#include "em_se.h"
#include "em_core.h"
#include "se_management.h"
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
    unsigned char q;

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

    SE_Command_t command = SE_COMMAND_DEFAULT(SE_COMMAND_AES_CCM_ENCRYPT);
    SE_DataTransfer_t key = SE_DATATRANSFER_DEFAULT(ctx->key, (ctx->keybits / 8));

    SE_DataTransfer_t in_data = SE_DATATRANSFER_DEFAULT((void*)input, length);
    SE_DataTransfer_t in_add = SE_DATATRANSFER_DEFAULT((void*)add, add_len);
    SE_DataTransfer_t in_nonce = SE_DATATRANSFER_DEFAULT((void*)iv, iv_len);
    SE_DataTransfer_t out_data = SE_DATATRANSFER_DEFAULT(output, length);
    SE_DataTransfer_t out_tag = SE_DATATRANSFER_DEFAULT(tag, tag_len);

    SE_addDataInput(&command, &key);
    SE_addDataInput(&command, &in_nonce);
    SE_addDataInput(&command, &in_add);
    SE_addDataInput(&command, &in_data);
    SE_addDataOutput(&command, &out_data);
    SE_addDataOutput(&command, &out_tag);

    SE_addParameter(&command, (ctx->keybits / 8));
    SE_addParameter(&command, ((iv_len & 0xFFFF) << 16) | (tag_len & 0xFFFF));
    SE_addParameter(&command, add_len);
    SE_addParameter(&command, length);

    int status = se_management_acquire();
    if (status != 0) {
        return status;
    }

    SE_executeCommand(&command);
    SE_Response_t command_status = SE_readCommandResponse();

    se_management_release();

    if ( command_status == SE_RESPONSE_OK ) {
        return 0;
    } else {
        return MBEDTLS_ERR_CCM_BAD_INPUT;
    }
}

int mbedtls_ccm_auth_decrypt( mbedtls_ccm_context *ctx, size_t length,
                      const unsigned char *iv, size_t iv_len,
                      const unsigned char *add, size_t add_len,
                      const unsigned char *input, unsigned char *output,
                      const unsigned char *tag, size_t tag_len )
{
    unsigned char q;

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

    SE_Command_t command = SE_COMMAND_DEFAULT(SE_COMMAND_AES_CCM_DECRYPT);
    SE_DataTransfer_t key = SE_DATATRANSFER_DEFAULT(ctx->key, (ctx->keybits / 8));

    SE_DataTransfer_t in_data = SE_DATATRANSFER_DEFAULT((void*)input, length);
    SE_DataTransfer_t in_add = SE_DATATRANSFER_DEFAULT((void*)add, add_len);
    SE_DataTransfer_t in_nonce = SE_DATATRANSFER_DEFAULT((void*)iv, iv_len);
    SE_DataTransfer_t out_data = SE_DATATRANSFER_DEFAULT(output, length);
    SE_DataTransfer_t in_tag = SE_DATATRANSFER_DEFAULT((void*)tag, tag_len);

    SE_addDataInput(&command, &key);
    SE_addDataInput(&command, &in_nonce);
    SE_addDataInput(&command, &in_add);
    SE_addDataInput(&command, &in_data);
    SE_addDataOutput(&command, &out_data);
    SE_addDataInput(&command, &in_tag);

    SE_addParameter(&command, (ctx->keybits / 8));
    SE_addParameter(&command, ((iv_len & 0xFFFF) << 16) | (tag_len & 0xFFFF));
    SE_addParameter(&command, add_len);
    SE_addParameter(&command, length);

    int status = se_management_acquire();
    if (status != 0) {
        return status;
    }

    SE_executeCommand(&command);
    SE_Response_t command_status = SE_readCommandResponse();

    se_management_release();

    if ( command_status == SE_RESPONSE_OK ) {
        return 0;
    } else if ( command_status == SE_RESPONSE_INVALID_SIGNATURE ) {
        memset( output, 0, length );
        return MBEDTLS_ERR_CCM_AUTH_FAILED;
    } else {
        return MBEDTLS_ERR_CCM_BAD_INPUT;
    }
}

#endif /* SEMAILBOX_PRESENT */

#endif /* MBEDTLS_CCM_ALT */

#endif /* MBEDTLS_AES_C */
