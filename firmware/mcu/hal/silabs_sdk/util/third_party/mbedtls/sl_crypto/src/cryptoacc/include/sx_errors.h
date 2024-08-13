/**
 * @file
 * @brief Defines the return codes for the CryptoSoC functions
 * @copyright Copyright (c) 2018 Silex Insight. All Rights reserved
 */


#ifndef SX_ERRORS_H
#define SX_ERRORS_H

#include <stdbool.h>

// TODO Push all of this in a enumeration

/** @brief Operation (whatever the kind of operation) terminated nicely */
#define CRYPTOLIB_SUCCESS 0

/**
 * @brief Operation involving a parameter which does not respect
 *        the system limitations
 *
 *        (i.e. trying to store a byte string bigger than the reserved memory)
 */
#define CRYPTOLIB_UNSUPPORTED_ERR 1

/**
 * @brief Operation involving a signature verification which is failing
 *
 * Algorithms involving signature (i.e. DSA, ECDSA, ...) may return this code
 */
#define CRYPTOLIB_INVALID_SIGN_ERR 3

/**
 * @brief Operation failing to to data transfer over the DMA bus
 *
 *        For instance, it can be due to too much/not enough data transferred.
 */
#define CRYPTOLIB_DMA_ERR 4

/** @brief Operation involving a invalid ECC point (i.e. not on a curve) */
#define CRYPTOLIB_INVALID_POINT 5

/**
 * @brief Operation which didn't terminate nicely and not due to any other listed error.
 *
 *        i.e. due to a crypto engine error
 */
#define CRYPTOLIB_CRYPTO_ERR 6

/** @brief Operation involving a parameter detected as invalid
 *
 *        (i.e. out of range value)
 */
#define CRYPTOLIB_INVALID_PARAM 7

/**
 * @brief DRBG random generation can't complete
 *        because new entropy should be reseeded in the DRBG
 */
#define CRYPTOLIB_DRBG_RESEED_NEEDED  8

/**
 * @brief Operation involving a signature is failing because the signature
 *        is not invertible.
 *
 *        Issuing a new operation with slightly different parameters
 *        (i.e. random if present) may solve this issue
 */
#define CRYPTOLIB_NON_INVERT_SIGN 9

#endif
