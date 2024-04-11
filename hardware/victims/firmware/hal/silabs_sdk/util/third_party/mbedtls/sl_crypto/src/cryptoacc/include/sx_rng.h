/**
 * @brief Random Number Generator Abstraction for CryptoSoC
 * @copyright Copyright (c) 2018 Silex Insight. All Rights reserved
 * @file
 *
 * By using the sx_rng structure defined in this file different RNG
 * implementations can be provided as parameter to functions which require
 * generation of random vales.
 *
 * Examples:
 * a. Using a Hash DRBG
 * @code
 * #include "sx_drbg_hash.h"
 *
 * struct sx_drbg_hash_ctx_s drbg_context;
 *
 * void drbg_get_rand_blk(void *param, block_t result)
 * {
 *     struct sx_drbg_hash_ctx_s* drbg_context = (struct sx_drbg_hash_ctx_s*)
 *                                               param;
 *
 *     uint32_t r = sx_drbg_hash_generate(drbg_context, result);
 *
 *     if (r == CRYPTOLIB_DRBG_RESEED_NEEDED) {
 *         sx_drbg_hash_reseed(drbg_context, ...)
 *     }
 *
 *     r = sx_drbg_hash_generate(drbg_context, result);
 *     CRYPTOLIB_ASSERT(r == CRYPTOLIB_SUCCESS, "DRBG failed") *
 * }
 *
 * struct sx_rng drbg_rng = {
 *     .param = (void*)(&drbg_context),
 *     .get_rand_blk = drbg_get_rand_blk
 * };
 *
 *   ...
 *
 *   sx_rng_get_rand_lt_n_blk(dst, n, drbg_rng);
 *   @endcode
 *
 * b. Using the TRNG
 *
 * @code
 * #include "sx_trng.h"
 *
 * struct sx_rng trng = {
 *     .param = NULL,
 *     .get_rand_blk = sx_trng_fill_blk
 * };
 *
 * ...
 *
 * sx_rng_get_rand_lt_n_blk(dst, n, trng);
 * @endcode
 */
#ifndef SX_RNG_H
#define SX_RNG_H

#include "cryptolib_types.h"
#include "compiler_extentions.h"

/**
 * @brief Structure used for passing a random number generator as a parameter to
 * functions which require one.
 */
struct sx_rng {
   void *param; /**< Pointer to the RNG state or any other element needed
   * by the get_rand_blk() function.
   */
   void (*get_rand_blk) (void *param, block_t result); /**< Pointer to the
    * function that will be called when a random data block is required.
    */
};


/**
* @brief Generate a random number k with 1 <= k < n.
* Implementation compliant with the FIPS 186-4 guideline.
* @param dst    Pointer to store generated random number k (should be buffer)
* @param n      Pointer to upper limit n (should be buffer)
* @param rng    Used random number generator
* @return       CRYPTOLIB_SUCCESS or CRYPTOLIB_INVALID_PARAM
*/
uint32_t sx_rng_get_rand_lt_n_blk(block_t dst, block_t n, struct sx_rng rng)
CHECK_RESULT;
#endif
