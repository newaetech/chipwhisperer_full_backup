/***************************************************************************//**
 * @file
 * @brief AES-CCM AEAD cipher
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
#ifndef MBEDTLS_CCM_ALT_H
#define MBEDTLS_CCM_ALT_H

/***************************************************************************//**
 * \addtogroup sl_crypto
 * \{
 ******************************************************************************/

/***************************************************************************//**
 * \addtogroup sl_crypto_ccm Accelerated AES-CCM AEAD Cipher
 * \brief Accelerated AES-CCM AEAD cipher using the SE peripheral
 *
 * \{
 ******************************************************************************/
#if defined(MBEDTLS_CCM_ALT)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief    The CCM context-type definition. The CCM context is passed
 *           to the APIs called.
 */
typedef struct {
    uint8_t  key[32];    /*!< The key in use. */
    uint32_t keybits;
}
mbedtls_ccm_context;

/**
 * \brief           This function initializes the specified CCM context,
 *                  to make references valid, and prepare the context
 *                  for mbedtls_ccm_setkey() or mbedtls_ccm_free().
 *
 * \param ctx       The CCM context to initialize.
 */
void mbedtls_ccm_init( mbedtls_ccm_context *ctx );

/**
 * \brief           This function initializes the CCM context set in the
 *                  \p ctx parameter and sets the encryption key.
 *
 * \param ctx       The CCM context to initialize.
 * \param cipher    The 128-bit block cipher to use.
 * \param key       The encryption key.
 * \param keybits   The key size in bits. This must be acceptable by the cipher.
 *
 * \return          \c 0 on success, or a cipher-specific error code.
 */
int mbedtls_ccm_setkey( mbedtls_ccm_context *ctx,
                        mbedtls_cipher_id_t cipher,
                        const unsigned char *key,
                        unsigned int keybits );

/**
 * \brief   This function releases and clears the specified CCM context
 *          and underlying cipher sub-context.
 *
 * \param ctx       The CCM context to clear.
 */
void mbedtls_ccm_free( mbedtls_ccm_context *ctx );

/**
 * \brief           This function encrypts a buffer using CCM.
 *
 * \param ctx       The CCM context to use for encryption.
 * \param length    The length of the input data in Bytes.
 * \param iv        Initialization vector (nonce).
 * \param iv_len    The length of the IV in Bytes: 7, 8, 9, 10, 11, 12, or 13.
 * \param add       The additional data field.
 * \param add_len   The length of additional data in Bytes.
 *                  Must be less than 2^16 - 2^8.
 * \param input     The buffer holding the input data.
 * \param output    The buffer holding the output data.
 *                  Must be at least \p length Bytes wide.
 * \param tag       The buffer holding the tag.
 * \param tag_len   The length of the tag to generate in Bytes:
 *                  4, 6, 8, 10, 12, 14 or 16.
 *
 * \note            The tag is written to a separate buffer. To concatenate
 *                  the \p tag with the \p output, as done in <em>RFC-3610:
 *                  Counter with CBC-MAC (CCM)</em>, use
 *                  \p tag = \p output + \p length, and make sure that the
 *                  output buffer is at least \p length + \p tag_len wide.
 *
 * \return          \c 0 on success.
 */
int mbedtls_ccm_encrypt_and_tag( mbedtls_ccm_context *ctx, size_t length,
                         const unsigned char *iv, size_t iv_len,
                         const unsigned char *add, size_t add_len,
                         const unsigned char *input, unsigned char *output,
                         unsigned char *tag, size_t tag_len );

/**
 * \brief           This function performs a CCM authenticated decryption of a
 *                  buffer.
 *
 * \param ctx       The CCM context to use for decryption.
 * \param length    The length of the input data in Bytes.
 * \param iv        Initialization vector.
 * \param iv_len    The length of the IV in Bytes: 7, 8, 9, 10, 11, 12, or 13.
 * \param add       The additional data field.
 * \param add_len   The length of additional data in Bytes.
 *                  Must be less than 2^16 - 2^8.
 * \param input     The buffer holding the input data.
 * \param output    The buffer holding the output data.
 *                  Must be at least \p length Bytes wide.
 * \param tag       The buffer holding the tag.
 * \param tag_len   The length of the tag in Bytes.
 *                  4, 6, 8, 10, 12, 14 or 16.
 *
 * \return          0 if successful and authenticated, or
 *                  #MBEDTLS_ERR_CCM_AUTH_FAILED if the tag does not match.
 */
int mbedtls_ccm_auth_decrypt( mbedtls_ccm_context *ctx, size_t length,
                      const unsigned char *iv, size_t iv_len,
                      const unsigned char *add, size_t add_len,
                      const unsigned char *input, unsigned char *output,
                      const unsigned char *tag, size_t tag_len );

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_CCM_ALT */

/** \} (end addtogroup sl_crypto_ccm) */
/** \} (end addtogroup sl_crypto) */

#endif /* MBEDTLS_CCM_ALT_H */
