/***************************************************************************//**
 * @file
 * @brief SHA-384 and SHA-512 cryptographic hash function
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
#ifndef MBEDTLS_SHA512_ALT_H
#define MBEDTLS_SHA512_ALT_H

/***************************************************************************//**
 * \addtogroup sl_crypto
 * \{
 ******************************************************************************/

/***************************************************************************//**
 * \addtogroup sl_crypto_sha512 Accelerated SHA-384/SHA-512 Hash Function
 * \brief Accelerated SHA-384/SHA-512 cryptographic hash function using the 
 *        SE peripheral
 *
 * \{
 ******************************************************************************/

#if defined(MBEDTLS_SHA512_ALT)

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          SHA-512 context structure
 */
typedef struct
{
    uint64_t total[2];          /*!< number of bytes processed  */
    uint64_t state[8];          /*!< intermediate digest state  */
    unsigned char buffer[128];  /*!< data block being processed */
    bool is384;                 /*!< 0 => SHA-512, else SHA-384 */
}
mbedtls_sha512_context;

/**
 * \brief          Initialize SHA-512 context
 *
 * \param ctx      SHA-512 context to be initialized
 */
void mbedtls_sha512_init( mbedtls_sha512_context *ctx );

/**
 * \brief          Clear SHA-512 context
 *
 * \param ctx      SHA-512 context to be cleared
 */
void mbedtls_sha512_free( mbedtls_sha512_context *ctx );

/**
 * \brief          Clone (the state of) a SHA-512 context
 *
 * \param dst      The destination context
 * \param src      The context to be cloned
 */
void mbedtls_sha512_clone( mbedtls_sha512_context *dst,
                           const mbedtls_sha512_context *src );

/**
 * \brief          This function starts a SHA-384 or SHA-512 checksum
 *                 calculation.
 *
 * \param ctx      The SHA-512 context to initialize.
 * \param is384    Determines which function to use.
 *                 <ul><li>0: Use SHA-512.</li>
 *                 <li>1: Use SHA-384.</li></ul>
 *
 * \return         \c 0 on success.
 */
int mbedtls_sha512_starts_ret( mbedtls_sha512_context *ctx, int is384 );

/**
 * \brief          This function feeds an input buffer into an ongoing
 *                 SHA-512 checksum calculation.
 *
 * \param ctx      The SHA-512 context.
 * \param input    The buffer holding the input data.
 * \param ilen     The length of the input data.
 *
 * \return         \c 0 on success.
 */
int mbedtls_sha512_update_ret( mbedtls_sha512_context *ctx,
                    const unsigned char *input,
                    size_t ilen );

/**
 * \brief          This function finishes the SHA-512 operation, and writes
 *                 the result to the output buffer. This function is for
 *                 internal use only.
 *
 * \param ctx      The SHA-512 context.
 * \param output   The SHA-384 or SHA-512 checksum result.
 *
 * \return         \c 0 on success.
 */
int mbedtls_sha512_finish_ret( mbedtls_sha512_context *ctx,
                               unsigned char output[64] );

/**
 * \brief          This function processes a single data block within
 *                 the ongoing SHA-512 computation.
 *
 * \param ctx      The SHA-512 context.
 * \param data     The buffer holding one block of data.
 *
 * \return         \c 0 on success.
 */
int mbedtls_internal_sha512_process( mbedtls_sha512_context *ctx,
                                     const unsigned char data[128] );

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
#if defined(MBEDTLS_DEPRECATED_WARNING)
#define MBEDTLS_DEPRECATED      __attribute__((deprecated))
#else
#define MBEDTLS_DEPRECATED
#endif
/**
 * \brief          This function starts a SHA-384 or SHA-512 checksum
 *                 calculation.
 *
 * \deprecated     Superseded by mbedtls_sha512_starts_ret() in 2.7.0
 *
 * \param ctx      The SHA-512 context to initialize.
 * \param is384    Determines which function to use.
 *                 <ul><li>0: Use SHA-512.</li>
 *                 <li>1: Use SHA-384.</li></ul>
 */
MBEDTLS_DEPRECATED void mbedtls_sha512_starts( mbedtls_sha512_context *ctx,
                                               int is384 );

/**
 * \brief          This function feeds an input buffer into an ongoing
 *                 SHA-512 checksum calculation.
 *
 * \deprecated     Superseded by mbedtls_sha512_update_ret() in 2.7.0
 *
 * \param ctx      The SHA-512 context.
 * \param input    The buffer holding the data.
 * \param ilen     The length of the input data.
 */
MBEDTLS_DEPRECATED void mbedtls_sha512_update( mbedtls_sha512_context *ctx,
                                               const unsigned char *input,
                                               size_t ilen );

/**
 * \brief          This function finishes the SHA-512 operation, and writes
 *                 the result to the output buffer.
 *
 * \deprecated     Superseded by mbedtls_sha512_finish_ret() in 2.7.0
 *
 * \param ctx      The SHA-512 context.
 * \param output   The SHA-384 or SHA-512 checksum result.
 */
MBEDTLS_DEPRECATED void mbedtls_sha512_finish( mbedtls_sha512_context *ctx,
                                               unsigned char output[64] );

/**
 * \brief          This function processes a single data block within
 *                 the ongoing SHA-512 computation. This function is for
 *                 internal use only.
 *
 * \deprecated     Superseded by mbedtls_internal_sha512_process() in 2.7.0
 *
 * \param ctx      The SHA-512 context.
 * \param data     The buffer holding one block of data.
 */
MBEDTLS_DEPRECATED void mbedtls_sha512_process(
                                            mbedtls_sha512_context *ctx,
                                            const unsigned char data[128] );

#undef MBEDTLS_DEPRECATED
#endif /* !MBEDTLS_DEPRECATED_REMOVED */

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_SHA512_ALT */

/** \} (end addtogroup sl_crypto_sha512) */
/** \} (end addtogroup sl_crypto) */

#endif /* mbedtls_sha512_alt.h */
