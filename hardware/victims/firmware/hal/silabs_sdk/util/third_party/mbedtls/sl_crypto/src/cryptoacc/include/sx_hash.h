/**
 * @file
 * @brief Declares the constants and functions to make operations
 *          with the BA413 hash function
 * @copyright Copyright (c) 2016-2018 Silex Insight. All Rights reserved
 */


#ifndef SX_HASH_H
#define SX_HASH_H

#include <stdint.h>
#include "compiler_extentions.h"
#include "cryptolib_types.h"

/** @brief Size of MD5 data block in bytes */
#define MD5_BLOCKSIZE      64
/** @brief Size of MD5 initialization value in bytes */
#define MD5_INITSIZE       16
/** @brief Size of MD5 digest in bytes */
#define MD5_DIGESTSIZE     16
/** @brief Size of SHA1 data block in bytes */
#define SHA1_BLOCKSIZE     64
/** @brief Size of SHA1 initialization value in bytes */
#define SHA1_INITSIZE      20
/** @brief Size of SHA1 digest in bytes */
#define SHA1_DIGESTSIZE    20
/** @brief Size of SHA224 data block in bytes */
#define SHA224_BLOCKSIZE   64
/** @brief Size of SHA224 initialization value in bytes */
#define SHA224_INITSIZE    32
/** @brief Size of SHA224 digest in bytes */
#define SHA224_DIGESTSIZE  28
/** @brief Size of SHA256 data block in bytes */
#define SHA256_BLOCKSIZE   64
/** @brief Size of SHA256 initialization value in bytes */
#define SHA256_INITSIZE    32
/** @brief Size of SHA256 digest in bytes */
#define SHA256_DIGESTSIZE  32
/** @brief Size of SHA384 data block in bytes */
#define SHA384_BLOCKSIZE   128
/** @brief Size of SHA384 initialization value in bytes */
#define SHA384_INITSIZE    64
/** @brief Size of SHA384 digest in bytes */
#define SHA384_DIGESTSIZE  48
/** @brief Size of SHA512 data block in bytes */
#define SHA512_BLOCKSIZE   128
/** @brief Size of SHA512 initialization value in bytes */
#define SHA512_INITSIZE    64
/** @brief Size of SHA512 digest in bytes */
#define SHA512_DIGESTSIZE  64
/** @brief Size of SM3 digest in bytes */
#define SM3_BLOCKSIZE      64
/** @brief Size of SM3 initialization value in bytes */
#define SM3_INITSIZE       32
/** @brief Size of SM3 digest in bytes */
#define SM3_DIGESTSIZE     32
/** @brief Maximum block size to be supported */
#define MAX_BLOCKSIZE   SHA512_BLOCKSIZE
/** @brief Maximum digest size to be supported */
#define MAX_DIGESTSIZE  SHA512_DIGESTSIZE
/** @brief Maximum number of entries in sx_hash_array_blk and sx_hmac_array_blk */
#define SX_HASH_ARRAY_MAX_ENTRIES 8

/**
* @brief Enumeration of the supported hash algorithms
*/
typedef enum sx_hash_fct_e
{
   e_MD5     = 1,    /**< MD5    */
   e_SHA1    = 2,    /**< SHA1   */
   e_SHA224  = 3,    /**< SHA224 */
   e_SHA256  = 4,    /**< SHA256 */
   e_SHA384  = 5,    /**< SHA384 */
   e_SHA512  = 6,    /**< SHA512 */
   e_SM3     = 7     /**< SM3    */
} sx_hash_fct_t;

/**
 * @brief: Initialization vector for SM3 context
 * https://tools.ietf.org/id/draft-oscca-cfrg-sm3-01.html#rfc.section.4.1
 */
extern const uint8_t sx_sm3_iv[SM3_DIGESTSIZE];

/**
 * @brief Get digest size in bytes for the given \p hash_fct
 * @param hash_fct hash function. See ::sx_hash_fct_t.
 * @return digest size in bytes, or 0 if invalid \p hash_fct
 */
uint32_t sx_hash_get_digest_size(sx_hash_fct_t hash_fct) CHECK_RESULT;

/**
 * @brief Get block size in bytes for the given \p hash_fct
 * @param hash_fct hash function. See ::sx_hash_fct_t.
 * @return block size in bytes, or 0 if invalid \p hash_fct
 */
uint32_t sx_hash_get_block_size(sx_hash_fct_t hash_fct) CHECK_RESULT;

/**
 * @brief Get state size in bytes for the given \p hash_fct
 * @param hash_fct hash function. See ::sx_hash_fct_t.
 * @return state size in bytes, or 0 if invalid \p hash_fct
 */
uint32_t sx_hash_get_state_size(sx_hash_fct_t hash_fct) CHECK_RESULT;

/**
 * @brief Compute hash digest of the content of \p data_in and write the result in \p data_out.
 * @param hash_fct hash function to use. See ::sx_hash_fct_t.
 * @param data_in  array of input data to process
 * @param entries  length of array \p data_in
 * @param data_out output digest
 * @return CRYPTOLIB_SUCCESS if execution was successful
 */
 uint32_t sx_hash_array_blk(
       sx_hash_fct_t hash_fct,
       block_t data_in[],
       const unsigned int entries,
       block_t data_out) CHECK_RESULT;

/**
 * @brief Compute HMAC of the content of \p data_in and write the result in \p data_out.
 * @param hash_fct hash function to use. See ::sx_hash_fct_t.
 * @param key      HMAC key to use
 * @param data_in  array of input data to process
 * @param entries  length of array \p data_in
 * @param data_out output digest
 * @return CRYPTOLIB_SUCCESS if execution was successful
 */
uint32_t sx_hmac_array_blk(
      sx_hash_fct_t hash_fct,
      block_t key,
      block_t data_in[],
      const unsigned int
      entries,
      block_t data_out) CHECK_RESULT;

/**
 * @brief Compute hash digest of the content of \p data_in and write the result in \p data_out.
 * @param hash_fct hash function to use. See ::sx_hash_fct_t.
 * @param data_in input data to process
 * @param data_out output digest
 * @return CRYPTOLIB_SUCCESS if execution was successful
 */
uint32_t sx_hash_blk(
      sx_hash_fct_t hash_fct,
      block_t data_in,
      block_t data_out) CHECK_RESULT;

/**
 * @brief Compute HMAC of the content of \p data_in and write the result in \p data_out.
 * @param hash_fct hash function to use. See ::sx_hash_fct_t.
 * @param key      HMAC key to use
 * @param data_in  input data to process
 * @param data_out output digest
 * @return CRYPTOLIB_SUCCESS if execution was successful
 */
uint32_t sx_hmac_blk(
      sx_hash_fct_t hash_fct,
      block_t key,
      block_t data_in,
      block_t data_out) CHECK_RESULT;

/**
 * @brief Compute hash block operation with \p state and \p data input, and write result back to \p state.
 * @param hash_fct hash function to use. See ::sx_hash_fct_t.
 * @param state    input and output state required in context switching.
 *                 Size must be equal to state size of \p hash_fct,
 *                 if it's the first operation must be filled with initialization values.
 * @param data     input data to process. Size must be multiple of the block size of \p hash_fct
 * @return CRYPTOLIB_SUCCESS if execution was successful
 */
uint32_t sx_hash_update_blk(
      sx_hash_fct_t hash_fct,
      block_t state,
      block_t data) CHECK_RESULT;

/**
 * @brief Finish a hash operation after zero or more update operations after doing necessary padding
 * @param hash_fct  hash function to use. See ::sx_hash_fct_t.
 * @param state     input and output state required in context switching.
 *                  Size must be equal to state size of \p hash_fct,
 *                  if it's the first operation must be filled with initialization values.
 * @param data_in   input data to process , can be of any length.
 * @param data_out  output of hashing the padded input
 * @param total_len length of all data hashed including the data hashed before using update and length of data_in
 * @return CRYPTOLIB_SUCCESS if execution was successful
 */
uint32_t sx_hash_finish_blk(
      sx_hash_fct_t hash_fct,
      block_t state,
      block_t data_in,
      block_t data_out,
      uint32_t total_len);

#endif
