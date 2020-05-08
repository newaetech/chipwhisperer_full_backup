/**
 * @file
 * @brief Defines the procedures to generate random numbers
 * as described in NIST 800-90A
 *
 * Using only a TRNG to generate random numbers is feasible but will consume
 * really quickly the entropy. Seeding first a DRBG and reading its output allows
 * to generate random numbers without consuming too quickly the entropy of
 * the TRNG source. Indeed, it is mandatory to reseed the DRBG but at
 * a lower rate and will reduce TRNG entropy consumption.
 *
 * \warning It is important to keep the internal state of the DRBG hidden
 * because it allows to predict next output of the DRBG.
 *
 * @copyright Copyright (c) 2017-2018 Silex Insight. All Rights reserved
 */

#ifndef SX_DRBG_HASH_H
#define SX_DRBG_HASH_H

#include <stdbool.h>
#include <stdint.h>
#include "cryptolib_def.h"
#include "sx_hash.h"

#define DRBG_HASH_MAX_SEEDLEN (888 / 8)

/**
 * @brief Context for DRBG Hash operations
 *
 * This is a private structure and should never be directly manipulated by
 * sx_drbg_hash users.
 */
typedef struct sx_drbg_hash_ctx_s
{
   bool     is_instantiated;
   uint16_t security_strength;
   sx_hash_fct_t hash_fct;
   size_t hash_block_size;
   uint32_t seedlen;

   uint8_t V[DRBG_HASH_MAX_SEEDLEN];
   uint8_t C[DRBG_HASH_MAX_SEEDLEN];
   uint64_t reseed_counter;
} sx_drbg_hash_ctx_t;

/**
 * @brief Instantiates a Hash_DRBG context
 *
 * @param ctx                  the context describing the state of this Hash_DRBG
 * @param security_strength    the desired security strength { 112, 128, 192, 256 }
 *                             (expressed in bits)
 * @param entropy              an array of random bytes
 * @param entropy_size         the size of the entropy array (expressed in bytes)
 * @param personalization      personalization data, non-secret
 * @param personalization_size the size of the personalization array
 * @param nonce                nonce data, secret
 * @param nonce_size           the size of the nonce array
 * @return CRYPTOLIB_SUCCESS when execution was successful
 *         CRYPTOLIB_INVALID_PARAM if the passed context is already instantiated,
 *         if the entropy size is too low or
 *         if the security strength is not recognized.
 */
uint32_t sx_drbg_hash_instantiate(sx_drbg_hash_ctx_t *ctx,
      uint32_t security_strength,
      uint8_t *entropy, size_t entropy_size,
      uint8_t *personalization, size_t personalization_size,
      uint8_t *nonce, size_t nonce_size);

/**
 * @brief Re-seeds the Hash_DRBG with the supplied entropy.
 *
 * @param ctx                  the context describing the state of this Hash_DRBG
 * @param entropy              an array of random bytes
 * @param entropy_size         the size of the entropy array (expressed in bytes)
 * @return CRYPTOLIB_SUCCESS when execution was successful
 *         CRYPTOLIB_INVALID_PARAM if the reseed entropy size is too low
 *         or if the passed context is not yet instantiated
 */
uint32_t sx_drbg_hash_reseed(sx_drbg_hash_ctx_t *ctx,
        uint8_t *entropy, size_t entropy_size);

/**
 * @brief Generates random bytes using the instantiated Hash_DRBG
 *
 * @param ctx  the context describing the state of this Hash_DRBG
 * @param out  block_t to result
 * @return CRYPTOLIB_SUCCESS when execution was successful,
 *         CRYPTOLIB_DRBG_RESEED_NEEDED if it is needed to reseed the Hash_DRGB
 *         with ::sx_drbg_hash_reseed (will occur after 2^48 generations),
 *         CRYPTOLIB_INVALID_PARAM if desired length of random is too big or
   *       if not yet instantiated
 */
uint32_t sx_drbg_hash_generate(sx_drbg_hash_ctx_t *ctx, block_t out);

/**
 * @brief Uninstantiates a previously instantiated Hash_DRBG
 *
 * @param ctx  the context describing the state of this Hash_DRBG
 * @return CRYPTOLIB_SUCCESS when execution was successful
 */
uint32_t sx_drbg_hash_uninstantiate(sx_drbg_hash_ctx_t *ctx);

#endif
