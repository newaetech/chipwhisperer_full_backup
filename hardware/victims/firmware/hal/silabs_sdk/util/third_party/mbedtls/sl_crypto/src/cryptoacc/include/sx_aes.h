/**
 * @file
 * @brief Defines the procedures to make operations with
 *          the BA411 AES function
 * @copyright Copyright (c) 2016-2019 Silex Insight. All Rights reserved
 */

#ifndef SX_AES_H
#define SX_AES_H

#include <stdint.h>
#include "compiler_extentions.h"
#include "cryptolib_types.h"
#include "sx_blk_cipher_common.h"

/** @brief Size for IV in all modes except GCM */
#define AES_IV_SIZE (BLK_CIPHER_IV_SIZE)
/** @brief Size for IV in GCM mode */
#define AES_IV_GCM_SIZE (BLK_CIPHER_IV_GCM_SIZE)
/** @brief Size of a GCM/GMAC/CMAC message authentification code (MAC)
 *         or maximum size of a CCM MAC */
#define AES_MAC_SIZE (BLK_CIPHER_MAC_SIZE)
/** @brief Size for Context in GCM and CCM modes */
#define AES_CTX_xCM_SIZE (BLK_CIPHER_CTX_xCM_SIZE)
/** @brief Size for Context in all modes except GCM and CCM */
#define AES_CTX_SIZE (BLK_CIPHER_CTX_SIZE)

/**
 * @brief Dummy variables to use hardware keys Key1 and Key2
 *
 * @note They are declared \c extern for internal reasons, user should \e not
 *       use them because they may disappear in future release.
 */
extern uint8_t aes_hw_key2;
extern uint8_t aes_hw_key1;

/**
 * @brief First Hardware Key (of 128bits)
 *
 * Two secret hardware keys may be wired directly into to AES module, preventing
 * the CPU to read them back. This block_t provides an abstraction to pass them
 * as input (user should not use them for anything else as input) in the same way
 * that user AES keys.
 */
#define AES_KEY1_128 block_t_convert(&aes_hw_key1, 128/8)
/** @brief First Hardware Key of 256b (for description, see ::AES_KEY1_128) */
#define AES_KEY1_256 block_t_convert(&aes_hw_key1, 256/8)
/** @brief Second Hardware Key of 128b (for description, see ::AES_KEY1_128) */
#define AES_KEY2_128 block_t_convert(&aes_hw_key2, 128/8)
/** @brief Second Hardware Key of 256b (for description, see ::AES_KEY1_128) */
#define AES_KEY2_256 block_t_convert(&aes_hw_key2, 256/8)

/**
* @brief Enumeration of possible mode for AES algorithm.
*/
typedef enum sx_aes_mode_e
{
    ENC = 1,            /**< Encrypt */
    DEC = 2,            /**< Decrypt */
} sx_aes_mode_t;

/** Encryption operation using AES-ECB
 *
 * @param key is the key involved to encrypt the plaintext
 * @param plaintext is the input data to encrypt
 * @param ciphertext is the output encrypted data
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_ecb_encrypt(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext);

/** Decryption operation using AES-ECB
 *
 * @param key is the key involved to decrypt the ciphertext
 * @param ciphertext is the input data to decrypt
 * @param plaintext is the output decrypted data
 * @return ::CRYPTOLIB_SUCCESS if decryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_ecb_decrypt(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext);

/** Encryption operation using AES-CBC
 *
 * @param key is the key involved to encrypt the plaintext
 * @param plaintext is the input data to encrypt
 * @param ciphertext is the output encrypted data
 * @param iv is the input initialization vector
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_cbc_encrypt(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *iv);

/** Decryption operation using AES-CBC
 *
 * @param key is the key involved to decrypt the ciphertext
 * @param ciphertext is the input data to decrypt
 * @param plaintext is the output decrypted data
 * @param iv is the input initialization vector
 * @return ::CRYPTOLIB_SUCCESS if decryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_cbc_decrypt(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *iv);

/** Start (init) encryption operation using AES-CBC in Init-Update-Final model
 *
 * @param key is the key involved to encrypt the plaintext
 * @param plaintext is the input data to encrypt
 * @param ciphertext is the output encrypted data
 * @param iv is the input initialization vector
 * @param ctx_out is the output intermediate context to forward to
 *                ::sx_aes_cbc_encrypt_update
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_cbc_encrypt_init(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *iv,
      block_t *ctx_out);

/** Start (init) decryption operation using AES-CBC in Init-Update-Final model
 *
 * @param key is the key involved to decrypt the ciphertext
 * @param ciphertext is the input data to decrypt
 * @param plaintext is the output decrypted data
 * @param iv is the input initialization vector
 * @param ctx_out is the output intermediate context to forward to
 *                ::sx_aes_cbc_decrypt_update
 * @return ::CRYPTOLIB_SUCCESS if decryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_cbc_decrypt_init(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *iv,
      block_t *ctx_out);

/** Continue (update) encryption operation using AES-CBC in Init-Update-Final model
 *
 * @param key is the key involved to encrypt the plaintext
 * @param plaintext is the input data to encrypt
 * @param ciphertext is the output encrypted data
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_aes_cbc_encrypt_init or from ::sx_aes_cbc_encrypt_update
 * @param ctx_out is the output intermediate context to forward to
 *                ::sx_aes_ctr_encrypt_update or to ::sx_aes_ctr_encrypt_final
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_cbc_encrypt_update(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *ctx_in,
      block_t *ctx_out);

/** Continue (update) decryption operation using AES-CBC in Init-Update-Final model
 *
 * @param key is the key involved to decrypt the ciphertext
 * @param ciphertext is the input data to decrypt
 * @param plaintext is the output decrypted data
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_aes_cbc_encrypt_init or from ::sx_aes_cbc_decrypt_update
 * @param ctx_out is the output intermediate context to forward to
 *                ::sx_aes_cbc_decrypt_update or to ::sx_aes_cbc_decrypt_final
 * @return ::CRYPTOLIB_SUCCESS if decryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_cbc_decrypt_update(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *ctx_in,
      block_t *ctx_out);

/** Finish (final) encryption operation using AES-CBC in Init-Update-Final model
 *
 * @param key is the key involved to encrypt the plaintext
 * @param plaintext is the input data to encrypt
 * @param ciphertext is the output encrypted data
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_aes_cbc_encrypt_init or from ::sx_aes_cbc_encrypt_update
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_cbc_encrypt_final(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *ctx_in);

/** Finish (final) decryption operation using AES-CBC in Init-Update-Final model
 *
 * @param key is the key involved to decrypt the ciphertext
 * @param ciphertext is the input data to decrypt
 * @param plaintext is the output decrypted data
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_aes_cbc_decrypt_init or from ::sx_aes_cbc_decrypt_update
 * @return ::CRYPTOLIB_SUCCESS if decryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_cbc_decrypt_final(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *ctx_in);

/** Encryption operation using AES-CTR
 *
 * @param key is the key involved to encrypt the plaintext
 * @param plaintext is the input data to encrypt
 * @param ciphertext is the output encrypted data
 * @param iv is the input initialization vector
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_ctr_encrypt(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *iv);

/** Decryption operation using AES-CTR
 *
 * @param key is the key involved to decrypt the ciphertext
 * @param ciphertext is the input data to decrypt
 * @param plaintext is the output decrypted data
 * @param iv is the input initialization vector
 * @return ::CRYPTOLIB_SUCCESS if decryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_ctr_decrypt(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *iv);

/** Start (init) encryption operation using AES-CTR in Init-Update-Final model
 *
 * @param key is the key involved to encrypt the plaintext
 * @param plaintext is the input data to encrypt
 * @param ciphertext is the output encrypted data
 * @param iv is the input initialization vector
 * @param ctx_out is the output intermediate context to forward to
 *                ::sx_aes_ctr_encrypt_update
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_ctr_encrypt_init(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *iv,
      block_t *ctx_out);

/** Start (init) decryption operation using AES-CTR in Init-Update-Final model
 *
 * @param key is the key involved to decrypt the ciphertext
 * @param ciphertext is the input data to decrypt
 * @param plaintext is the output decrypted data
 * @param iv is the input initialization vector
 * @param ctx_out is the output intermediate context to forward to
 *                ::sx_aes_ctr_decrypt_update
 * @return ::CRYPTOLIB_SUCCESS if decryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_ctr_decrypt_init(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *iv,
      block_t *ctx_out);

/** Continue (update) encryption operation using AES-CTR in Init-Update-Final model
 *
 * @param key is the key involved to encrypt the plaintext
 * @param plaintext is the input data to encrypt
 * @param ciphertext is the output encrypted data
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_aes_ctr_encrypt_init or from ::sx_aes_ctr_encrypt_update
 * @param ctx_out is the output intermediate context to forward to
 *                ::sx_aes_ctr_encrypt_update or to ::sx_aes_ctr_encrypt_final
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_ctr_encrypt_update(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *ctx_in,
      block_t *ctx_out);

/** Continue (update) decryption operation using AES-CTR in Init-Update-Final model
 *
 * @param key is the key involved to decrypt the ciphertext
 * @param ciphertext is the input data to decrypt
 * @param plaintext is the output decrypted data
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_aes_ctr_encrypt_init or from ::sx_aes_ctr_decrypt_update
 * @param ctx_out is the output intermediate context to forward to
 *                ::sx_aes_ctr_encrypt_update or to ::sx_aes_ctr_decrypt_final
 * @return ::CRYPTOLIB_SUCCESS if decryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_ctr_decrypt_update(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *ctx_in,
      block_t *ctx_out);

/** Finish (final) encryption operation using AES-CTR in Init-Update-Final model
 *
 * @param key is the key involved to encrypt the plaintext
 * @param plaintext is the input data to decrypt
 * @param ciphertext is the output encrypted data
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_aes_ctr_encrypt_init or from ::sx_aes_ctr_encrypt_update
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_ctr_encrypt_final(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *ctx_in);

/** Finish (final) decryption operation using AES-CTR in Init-Update-Final model
 *
 * @param key is the key involved to decrypt the ciphertext
 * @param ciphertext is the input data to decrypt
 * @param plaintext is the output decrypted data
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_aes_ctr_decrypt_init or from ::sx_aes_ctr_decrypt_update
 * @return ::CRYPTOLIB_SUCCESS if decryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_ctr_decrypt_final(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *ctx_in);

/** Encryption operation using AES-CFB
 *
 * @param key is the key involved to encrypt the plaintext
 * @param plaintext is the input data to encrypt
 * @param ciphertext is the output encrypted data
 * @param iv is the input initialization vector
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_cfb_encrypt(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *iv);

/** Decryption operation using AES-CFB
 *
 * @param key is the key involved to decrypt the ciphertext
 * @param ciphertext is the input data to decrypt
 * @param plaintext is the output decrypted data
 * @param iv is the input initialization vector
 * @return ::CRYPTOLIB_SUCCESS if decryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_cfb_decrypt(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *iv);

/** Start (init) encryption operation using AES-CFB in Init-Update-Final model
 *
 * @param key is the key involved to encrypt the plaintext
 * @param plaintext is the input data to encrypt
 * @param ciphertext is the output encrypted data
 * @param iv is the input initialization vector
 * @param ctx_out is the output intermediate context to forward to
 *                ::sx_aes_cfb_encrypt_update
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_cfb_encrypt_init(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *iv,
      block_t *ctx_out);

/** Start (init) decryption operation using AES-CFB in Init-Update-Final model
 *
 * @param key is the key involved to decrypt the ciphertext
 * @param ciphertext is the input data to decrypt
 * @param plaintext is the output decrypted data
 * @param iv is the input initialization vector
 * @param ctx_out is the output intermediate context to forward to
 *                ::sx_aes_cfb_decrypt_update
 * @return ::CRYPTOLIB_SUCCESS if decryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_cfb_decrypt_init(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *iv,
      block_t *ctx_out);

/** Continue (update) encryption operation using AES-CFB in Init-Update-Final model
 *
 * @param key is the key involved to encrypt the plaintext
 * @param plaintext is the input data to encrypt
 * @param ciphertext is the output encrypted data
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_aes_cfb_encrypt_init or from ::sx_aes_cfb_encrypt_update
 * @param ctx_out is the output intermediate context to forward to
 *                ::sx_aes_cfb_encrypt_update or to ::sx_aes_cfb_encrypt_final
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_cfb_encrypt_update(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *ctx_in,
      block_t *ctx_out);

/** Continue (update) decryption operation using AES-CFB in Init-Update-Final model
 *
 * @param key is the key involved to decrypt the ciphertext
 * @param ciphertext is the input data to decrypt
 * @param plaintext is the output decrypted data
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_aes_cfb_encrypt_init or from ::sx_aes_cfb_decrypt_update
 * @param ctx_out is the output intermediate context to forward to
 *                ::sx_aes_cfb_encrypt_update or to ::sx_aes_cfb_decrypt_final
 * @return ::CRYPTOLIB_SUCCESS if decryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_cfb_decrypt_update(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *ctx_in,
      block_t *ctx_out);

/** Finish (final) encryption operation using AES-CFB in Init-Update-Final model
 *
 * @param key is the key involved to encrypt the plaintext
 * @param plaintext is the input data to decrypt
 * @param ciphertext is the output encrypted data
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_aes_cfb_encrypt_init or from ::sx_aes_cfb_encrypt_update
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_cfb_encrypt_final(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *ctx_in);

/** Finish (final) decryption operation using AES-CFB in Init-Update-Final model
 *
 * @param key is the key involved to decrypt the ciphertext
 * @param ciphertext is the input data to decrypt
 * @param plaintext is the output decrypted data
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_aes_cfb_decrypt_init or from ::sx_aes_cfb_decrypt_update
 * @return ::CRYPTOLIB_SUCCESS if decryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_cfb_decrypt_final(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *ctx_in);

/** Encryption operation using AES-OFB
 *
 * @param key is the key involved to encrypt the plaintext
 * @param plaintext is the input data to encrypt
 * @param ciphertext is the output encrypted data
 * @param iv is the input initialization vector
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_ofb_encrypt(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *iv);

/** Decryption operation using AES-OFB
 *
 * @param key is the key involved to decrypt the ciphertext
 * @param ciphertext is the input data to decrypt
 * @param plaintext is the output decrypted data
 * @param iv is the input initialization vector
 * @return ::CRYPTOLIB_SUCCESS if decryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_ofb_decrypt(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *iv);

/** Start (init) encryption operation using AES-OFB in Init-Update-Final model
 *
 * @param key is the key involved to encrypt the plaintext
 * @param plaintext is the input data to encrypt
 * @param ciphertext is the output encrypted data
 * @param iv is the input initialization vector
 * @param ctx_out is the output intermediate context to forward to
 *                ::sx_aes_ofb_encrypt_update
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_ofb_encrypt_init(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *iv,
      block_t *ctx_out);

/** Start (init) decryption operation using AES-OFB in Init-Update-Final model
 *
 * @param key is the key involved to decrypt the ciphertext
 * @param ciphertext is the input data to decrypt
 * @param plaintext is the output decrypted data
 * @param iv is the input initialization vector
 * @param ctx_out is the output intermediate context to forward to
 *                ::sx_aes_ofb_decrypt_update
 * @return ::CRYPTOLIB_SUCCESS if decryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_ofb_decrypt_init(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *iv,
      block_t *ctx_out);

/** Continue (update) encryption operation using AES-OFB in Init-Update-Final model
 *
 * @param key is the key involved to encrypt the plaintext
 * @param plaintext is the input data to encrypt
 * @param ciphertext is the output encrypted data
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_aes_ofb_encrypt_init or from ::sx_aes_ofb_encrypt_update
 * @param ctx_out is the output intermediate context to forward to
 *                ::sx_aes_ofb_encrypt_update or to ::sx_aes_ofb_encrypt_final
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_ofb_encrypt_update(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *ctx_in,
      block_t *ctx_out);

/** Continue (update) decryption operation using AES-OFB in Init-Update-Final model
 *
 * @param key is the key involved to decrypt the ciphertext
 * @param ciphertext is the input data to decrypt
 * @param plaintext is the output decrypted data
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_aes_ofb_encrypt_init or from ::sx_aes_ofb_decrypt_update
 * @param ctx_out is the output intermediate context to forward to
 *                ::sx_aes_ofb_encrypt_update or to ::sx_aes_ofb_decrypt_final
 * @return ::CRYPTOLIB_SUCCESS if decryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_ofb_decrypt_update(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *ctx_in,
      block_t *ctx_out);

/** Finish (final) encryption operation using AES-OFB in Init-Update-Final model
 *
 * @param key is the key involved to encrypt the plaintext
 * @param ciphertext is the input data to decrypt
 * @param plaintext is the output encrypted data
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_aes_ofb_encrypt_init or from ::sx_aes_ofb_encrypt_update
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_ofb_encrypt_final(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *ctx_in);

/** Finish (final) decryption operation using AES-OFB in Init-Update-Final model
 *
 * @param key is the key involved to decrypt the ciphertext
 * @param ciphertext is the input data to decrypt
 * @param plaintext is the output decrypted data
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_aes_ofb_decrypt_init or from ::sx_aes_ofb_decrypt_update
 * @return ::CRYPTOLIB_SUCCESS if decryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_ofb_decrypt_final(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *ctx_in);

/** Encryption operation using AES-XTS
 *
 * @param key is the key involved to encrypt the plaintext
 * @param plaintext is the input data to encrypt
 * @param ciphertext is the output encrypted data
 * @param iv is the input initialization vector
 * @param key_xts is the "tweak" key involved in AES-XTS
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_xts_encrypt(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *iv,
      const block_t *key_xts);

/** Decryption operation using AES-XTS
 *
 * @param key is the key involved to decrypt the ciphertext
 * @param ciphertext is the input data to decrypt
 * @param plaintext is the output decrypted data
 * @param iv is the input initialization vector
 * @param key_xts is the "tweak" key involved in AES-XTS
 * @return ::CRYPTOLIB_SUCCESS if decryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_xts_decrypt(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *iv,
      const block_t *key_xts);

/** Start (init) encryption operation using AES-XTS in Init-Update-Final model
 *
 * @param key is the key involved to encrypt the plaintext
 * @param ciphertext is the input data to decrypt
 * @param plaintext is the output encrypted data
 * @param iv is the input initialization vector
 * @param ctx_out is the output intermediate context to forward to
 *                ::sx_aes_xts_encrypt_update
 * @param key_xts is the "tweak" key involved in AES-XTS
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_xts_encrypt_init(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *iv,
      block_t *ctx_out,
      const block_t *key_xts);

/** Start (init) decryption operation using AES-XTS in Init-Update-Final model
 *
 * @param key is the key involved to decrypt the ciphertext
 * @param ciphertext is the input data to decrypt
 * @param plaintext is the output decrypted data
 * @param iv is the input initialization vector
 * @param ctx_out is the output intermediate context to forward to
 *                ::sx_aes_xts_decrypt_update
 * @param key_xts is the "tweak" key involved in AES-XTS
 * @return ::CRYPTOLIB_SUCCESS if decryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_xts_decrypt_init(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *iv,
      block_t *ctx_out,
      const block_t *key_xts);

/** Continue (update) encryption operation using AES-XTS in Init-Update-Final model
 *
 * @param key is the key involved to encrypt the plaintext
 * @param ciphertext is the input data to encrypt
 * @param plaintext is the output encrypted data
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_aes_xts_encrypt_init or from ::sx_aes_xts_encrypt_update
 * @param ctx_out is the output intermediate context to forward to
 *                ::sx_aes_xts_encrypt_update or to ::sx_aes_xts_encrypt_final
 * @param key_xts is the "tweak" key involved in AES-XTS
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_xts_encrypt_update(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *ctx_in,
      block_t *ctx_out,
      const block_t *key_xts);

/** Continue (update) decryption operation using AES-XTS in Init-Update-Final model
 *
 * @param key is the key involved to decrypt the ciphertext
 * @param ciphertext is the input data to decrypt
 * @param plaintext is the output decrypted data
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_aes_xts_encrypt_init or from ::sx_aes_xts_decrypt_update
 * @param ctx_out is the output intermediate context to forward to
 *                ::sx_aes_xts_encrypt_update or to ::sx_aes_xts_decrypt_final
 * @param key_xts is the "tweak" key involved in AES-XTS
 * @return ::CRYPTOLIB_SUCCESS if decryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_xts_decrypt_update(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *ctx_in,
      block_t *ctx_out,
      const block_t *key_xts);

/** Finish (final) encryption operation using AES-XTS in Init-Update-Final model
 *
 * @param key is the key involved to encrypt the plaintext
 * @param ciphertext is the input data to decrypt
 * @param plaintext is the output encrypted data
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_aes_xts_encrypt_init or from ::sx_aes_xts_encrypt_update
 * @param key_xts is the "tweak" key involved in AES-XTS
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_xts_encrypt_final(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *ctx_in,
      const block_t *key_xts);

/** Finish (final) decryption operation using AES-XTS in Init-Update-Final model
 *
 * @param key is the key involved to decrypt the ciphertext
 * @param ciphertext is the input data to decrypt
 * @param plaintext is the output decrypted data
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_aes_xts_decrypt_init or from ::sx_aes_xts_decrypt_update
 * @param key_xts is the "tweak" key involved in AES-XTS
 * @return ::CRYPTOLIB_SUCCESS if decryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_xts_decrypt_final(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *ctx_in,
      const block_t *key_xts);

/** Encryption and authentication operation using AES-GCM
 *
 * @param key is the key involved to encrypt and authenticate the plaintext
 * @param plaintext is the input data to encrypt and authenticate
 * @param ciphertext is the output encrypted data
 * @param iv is the input initialization vector
 * @param mac is the output Message Authentication Code of the plaintext and aad
 * @param aad are additional data passed aside the plaintext to compute the MAC
 * @return ::CRYPTOLIB_SUCCESS if encryption and authentication succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 * @note If one only needs authentication, this function may be used as GMAC
 *       and user has to only keep the \c plaintext and the \c mac
 */
uint32_t sx_aes_gcm_encrypt(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *iv,
      block_t *mac,
      const block_t *aad);

/** Decryption and authentication operation using AES-GCM
 *
 * @param key is the key involved to decrypt and authenticate the ciphertext
 * @param ciphertext is the input data to decrypt and authenticate
 * @param plaintext is the output decrypted data
 * @param iv is the input initialization vector
 * @param mac is the output Message Authentication Code of the ciphertext and aad
 * @param aad are additional data passed aside the plaintext to compute the MAC
 * @return ::CRYPTOLIB_SUCCESS if decryption and authentication succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_gcm_decrypt(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *iv,
      block_t *mac,
      const block_t *aad);

/** Decryption, authentication and verification operation using AES-GCM
 *
 * By opposition to ::sx_aes_gcm_decrypt, this function does not output the MAC
 * but uses an input reference to internally check if it maches the recomputed
 * one. By only checking the returned value of the function, caller knows
 * if the input data (\ciphertext) are tampered or not.
 *
 * @param key is the key involved to decrypt and authenticate the ciphertext
 * @param ciphertext is the input data to decrypt and authenticate
 * @param plaintext is the output decrypted data
 * @param iv is the input initialization vector
 * @param mac is an input Message Authentication Code as reference for verification
 * @param aad are additional data passed aside the ciphertext to compute the MAC
 * @return ::CRYPTOLIB_SUCCESS if reference MAC matches recomputed one
 *         ::CRYPTOLIB_INVALID_SIGN_ERR if reference MAC does not match
 *         recomputed one
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_gcm_decrypt_verify(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *iv,
      const block_t *mac,
      const block_t *aad);

/** Start (init) encryption and authentication operation using AES-GCM in
 *  Init-Update-Final model
 *
 * @param key is the key involved to encrypt and authenticate the plaintext
 * @param plaintext is the input dat to encrypt and authenticate
 * @param ciphertext is the output encrypted data
 * @param iv is the input initialization vector
 * @param ctx_out is the output intermediate context to forward to
 *                ::sx_aes_gcm_encrypt_update (expected size is 32B)
 * @param aad are additional data passed aside the plaintext to compute the MAC
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_gcm_encrypt_init(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *iv,
      block_t *ctx_out,
      const block_t *aad);

/** Start (init) decryption and authentication operation using AES-GCM in
 *  Init-Update-Final model
 *
 * @param key is the key involved to decrypt and authenticate the ciphertext
 * @param ciphertext is the input data to decrypt and authenticate
 * @param plaintext is the output decrypted data
 * @param iv is the input initialization vector
 * @param ctx_out is the output intermediate context to forward to
 *                ::sx_aes_gcm_decrypt_update (expected size is 32B)
 * @param aad are additional data passed aside the ciphertext to compute the MAC
 * @return ::CRYPTOLIB_SUCCESS if decryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_gcm_decrypt_init(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *iv,
      block_t *ctx_out,
      const block_t *aad);

/** Continue (update) encryption and authentication operation using AES-GCM in
 *  Init-Update-Final model
 *
 * @param key is the key involved to encrypt and authenticate the plaintext
 * @param plaintext is the input data to encrypt and authenticate
 * @param ciphertext is the output encrypted data
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_aes_gcm_encrypt_init or from ::sx_aes_gcm_encrypt_update
 * @param ctx_out is the output intermediate context to forward to
 *                ::sx_aes_gcm_encrypt_update or to ::sx_aes_gcm_encrypt_final
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 *
 * @note the expected size for GCM context is 32B
 */
uint32_t sx_aes_gcm_encrypt_update(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *ctx_in,
      block_t *ctx_out);

/** Continue (update) decryption and authentication operation using AES-GCM in
 *  Init-Update-Final model
 *
 * @param key is the key involved to decrypt and authenticate the ciphertext
 * @param ciphertext is the input data to decrypt and authenticate
 * @param plaintext is the output decrypted data
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_aes_gcm_encrypt_init or from ::sx_aes_gcm_decrypt_update
 * @param ctx_out is the output intermediate context to forward to
 *                ::sx_aes_gcm_decrypt_update or to ::sx_aes_gcm_decrypt_final
 * @return ::CRYPTOLIB_SUCCESS if decryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 *
 * @note the expected size for GCM context is 32B
 */
uint32_t sx_aes_gcm_decrypt_update(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *ctx_in,
      block_t *ctx_out);

/** Finish (final) encryption and authentication operation using AES-GCM in
 *  Init-Update-Final model
 *
 * @param key is the key involved to encrypt and authenticate the plaintext
 * @param plaintext is the input data to encrypt and authenticate
 * @param ciphertext is the output encrypted data
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_aes_gcm_encrypt_init or from ::sx_aes_gcm_encrypt_update
 * @param mac is the output Message Authentication Code of the plaintext and aad
 * @param len_a_c are length of both \c plaintext and \c aad as described in
 *        NIST SP800-38D at 7.GCM Specification
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 *
 * @note the expected size for GCM context is 32B
 */
uint32_t sx_aes_gcm_encrypt_final(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *ctx_in,
      block_t *mac,
      const block_t *len_a_c);

/** Finish (final) decryption and authentication operation using AES-GCM in
 *  Init-Update-Final model
 *
 * @param key is the key involved to decrypt and authenticate the ciphertext
 * @param ciphertext is the input data to decrypt and authenticate
 * @param plaintext is the output decrypted data
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_aes_gcm_decrypt_init or from ::sx_aes_gcm_decrypt_update
 * @param mac is the output Message Authentication Code of the ciphertext and aad
 * @param len_a_c are length of both \c ciphertext and \c aad as described in
 *        NIST SP800-38D at 7.GCM Specification
 * @return ::CRYPTOLIB_SUCCESS if decryption and authentication succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 *
 * @note the expected size for GCM context is 32B
 */
uint32_t sx_aes_gcm_decrypt_final(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *ctx_in,
      block_t *mac,
      const block_t *len_a_c);

/** Finish (final) decryption, authentication and verification operation using
 *  AES-GCM in Init-Update-Final model
 *
 * By opposition to ::sx_aes_gcm_decrypt_final, this function does not output
 * the MAC but uses an input reference to internally check if it maches the
 * recomputed one. By only checking the returned value of the function, caller
 * knows if the input data (\ciphertext) are tampered or not.
 *
 * @param key is the key involved to decrypt and authenticate the ciphertext
 * @param ciphertext is the input data to decrypt and authenticate
 * @param plaintext is the output decrypted data
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_aes_gcm_decrypt_init or from ::sx_aes_gcm_decrypt_update
 * @param mac is an input Message Authentication Code as reference for verification
 * @param len_a_c are length of both \c ciphertext and \c aad as described in
 *        NIST SP800-38D at 7.GCM Specification
 * @return ::CRYPTOLIB_SUCCESS if reference MAC matches recomputed one
 *         ::CRYPTOLIB_INVALID_SIGN_ERR if reference MAC does not match
 *         recomputed one
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 *
 * @note the expected size for GCM context is 32B
 */
uint32_t sx_aes_gcm_decrypt_verify_final(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *ctx_in,
      const block_t *mac,
      const block_t *len_a_c);

/** Encryption and authentication operation using AES-CCM
 *
 * @param key is the key involved to encrypt and authenticate the plaintext
 * @param plaintext is the input data to encrypt and authenticate
 * @param ciphertext is the output encrypted data
 * @param nonce is the input number to use once (expected size is bewteen 7 and 13B)
 * @param mac is the output Message Authentication Code of the plaintext and aad,
 *        expected size may be 0, 4, 6, 8, 10, 12, 14, 16B. 0B corresponds to
 *        CCM* described in B.4 Specification of generic CCM* mode of operation
 *        http://ecee.colorado.edu/~liue/teaching/comm_standards/2015S_zigbee/802.15.4-2011.pdf
 * @param aad are additional data passed aside the plaintext to compute the MAC
 * @return ::CRYPTOLIB_SUCCESS if encryption and authentication succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_ccm_encrypt(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *nonce,
      block_t *mac,
      const block_t *aad);

/** Decryption and authentication operation using AES-CCM
 *
 * @param key is the key involved to decrypt and authenticate the ciphertext
 * @param ciphertext is the input data to decrypt and authenticate
 * @param plaintext is the output decrypted data
 * @param nonce is the input number to use once (expected size is bewteen 7 and 13B)
 * @param mac is the output Message Authentication Code
 * @param aad are additional data passed aside the ciphertext to compute the MAC
 * @return ::CRYPTOLIB_SUCCESS if decryption and authentication succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_ccm_decrypt(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *nonce,
      block_t *mac,
      const block_t *aad);

/** Decryption, authentication and verification operation using AES-CCM
 *
 * @param key is the key involved to decrypt and authenticate the ciphertext
 * @param ciphertext is the input data to decrypt and authenticate
 * @param plaintext is the output decrypted data
 * @param nonce is the input number to use once (expected size is bewteen 7 and 13B)
 * @param mac is an input Message Authentication Code as reference for verification,
 * @param aad are additional data passed aside the ciphertext to compute the MAC
 * @return ::CRYPTOLIB_SUCCESS if reference MAC matches recomputed one
 *         ::CRYPTOLIB_INVALID_SIGN_ERR if reference MAC does not match
 *         recomputed one
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_ccm_decrypt_verify(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *nonce,
      const block_t *mac,
      const block_t *aad);

/** Start (init) encryption and authentication operation using AES-CCM in
 *  Init-Update-Final model
 *
 * @param key is the key involved to encrypt and authenticate the plaintext
 * @param plaintext is the input dat to encrypt and authenticate
 * @param ciphertext is the output encrypted data
 * @param nonce is the input number to use once
 * @param ctx_out is the output intermediate context to forward to
 *                ::sx_aes_ccm_encrypt_update (expected size is 32B)
 * @param aad are additional data passed aside the plaintext to compute the MAC
 * @param mac_len is the length of the mac to output. In IUF, it is mandatory
 *        to provide it during init step as this length is required to compute
 *        CCM headers.
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_ccm_encrypt_init(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *nonce,
      block_t *ctx_out,
      const block_t *aad,
      uint32_t mac_len);

/** Start (init) decryption and authentication operation using AES-CCM in
 *  Init-Update-Final model
 *
 * @param key is the key involved to decrypt and authenticate the ciphertext
 * @param ciphertext is the input data to decrypt and authenticate
 * @param plaintext is the output decrypted data
 * @param nonce is the input number to use once
 * @param ctx_out is the output intermediate context to forward to
 *                ::sx_aes_ccm_decrypt_update (expected size is 32B)
 * @param aad are additional data passed aside the ciphertext to compute the MAC
 * @param mac_len is the length of the mac to output. In IUF, it is mandatory
 *        to provide it during init step as this length is required to compute
 *        CCM headers.
 * @return ::CRYPTOLIB_SUCCESS if decryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_ccm_decrypt_init(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *nonce,
      block_t *ctx_out,
      const block_t *aad,
      uint32_t mac_len);

/** Continue (update) encryption and authentication operation using AES-CCM in
 *  Init-Update-Final model
 *
 * @param key is the key involved to encrypt and authenticate the plaintext
 * @param plaintext is the input data to encrypt and authenticate
 * @param ciphertext is the output encrypted data
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_aes_ccm_encrypt_init or from ::sx_aes_ccm_encrypt_update
 * @param ctx_out is the output intermediate context to forward to
 *                ::sx_aes_ccm_encrypt_update or to ::sx_aes_ccm_encrypt_final
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 *
 * @note the expected size for CCM context is 32B
 */
uint32_t sx_aes_ccm_encrypt_update(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *ctx_in,
      block_t *ctx_out);

/** Continue (update) decryption and authentication operation using AES-CCM in
 *  Init-Update-Final model
 *
 * @param key is the key involved to decrypt and authenticate the ciphertext
 * @param ciphertext is the input data to decrypt and authenticate
 * @param plaintext is the output decrypted data
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_aes_ccm_encrypt_init or from ::sx_aes_ccm_decrypt_update
 * @param ctx_out is the output intermediate context to forward to
 *                ::sx_aes_ccm_decrypt_update or to ::sx_aes_ccm_decrypt_verify_final
 * @return ::CRYPTOLIB_SUCCESS if decryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 *
 * @note the expected size for CCM context is 32B
 */
uint32_t sx_aes_ccm_decrypt_update(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *ctx_in,
      block_t *ctx_out);

/** Finish (final) encryption and authentication operation using AES-CCM in
 *  Init-Update-Final model
 *
 * @param key is the key involved to encrypt and authenticate the plaintext
 * @param plaintext is the input data to encrypt and authenticate
 * @param ciphertext is the output encrypted data
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_aes_ccm_encrypt_init or from ::sx_aes_ccm_encrypt_update
 * @param mac is the output Message Authentication Code of the plaintext and aad
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 *
 * @note the expected size for CCM context is 32B
 */
uint32_t sx_aes_ccm_encrypt_final(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *ctx_in,
      block_t *mac);

/** Finish (final) decryption and authentication operation using AES-CCM in
 * Init-Update-Final model
 *
 * @param key is the key involved to decrypt and authenticate the ciphertext
 * @param ciphertext is the input data to decrypt and authenticate
 * @param plaintext is the output decrypted data
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_aes_ccm_decrypt_init or from ::sx_aes_ccm_decrypt_update
 * @param mac is an input Message Authentication Code as reference for verification
 * @return ::CRYPTOLIB_SUCCESS if decryption and authentication succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 *
 * @note the expected size for CCM context is 32B
 */
uint32_t sx_aes_ccm_decrypt_final(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *ctx_in,
      block_t *mac);

/** Finish (final) decryption, authentication and verification operation using
 *  AES-CCM in Init-Update-Final model
 *
 * @param key is the key involved to decrypt and authenticate the ciphertext
 * @param ciphertext is the input data to decrypt and authenticate
 * @param plaintext is the output decrypted data
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_aes_ccm_decrypt_init or from ::sx_aes_ccm_decrypt_update
 * @param mac is an input Message Authentication Code as reference for verification
 * @return ::CRYPTOLIB_SUCCESS if reference MAC matches recomputed one
 *         ::CRYPTOLIB_INVALID_SIGN_ERR if reference MAC does not match
 *         recomputed one
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 *
 * @note the expected size for CCM context is 32B
 */
uint32_t sx_aes_ccm_decrypt_verify_final(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *ctx_in,
      const block_t *mac);

/** Authentication operation using CMAC
 *
 * @param key is the key involved in authentication of a message
 * @param message is the message to authenticate
 * @param mac is the output Message Authentication Code
 * @return ::CRYPTOLIB_SUCCESS if authentication generation succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_cmac_generate(
      const block_t *key,
      const block_t *message,
      block_t *mac);

/** Authentication verification operation using CMAC
 *
 * @param key is the key involved in authentication of a message
 * @param message is the message to authenticate and verify
 * @param mac is an input Message Authentication Code used as reference
 * @return ::CRYPTOLIB_SUCCESS if authentication verifiation succeeded
 *         ::CRYPTOLIB_INVALID_SIGN_ERR if authentication verification failed
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_cmac_verify(
      const block_t *key,
      const block_t *message,
      const block_t *mac);

/** Start (init) authentication operation using CMAC in Init-Update-Final model
 *
 * @param key is the key involved in authentication of a message
 * @param message is the message to authenticate
 * @param ctx_out is the output intermediate context to forward to
 *                ::sx_aes_cmac_generate_update
 * @return ::CRYPTOLIB_SUCCESS if authentication generation succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_cmac_generate_init(
      const block_t *key,
      const block_t *message,
      block_t *ctx_out);

/** Continue (update) authentication operation using CMAC in
 *  Init-Update-Final model
 *
 * @param key is the key involved in authentication of a message
 * @param message is the message to authenticate
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_aes_cmac_generate_init or from ::sx_aes_cmac_generate_update
 * @param ctx_out is the output intermediate context to forward to
 *                ::sx_aes_cmac_generate_update
 * @return ::CRYPTOLIB_SUCCESS if authentication generation succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_cmac_generate_update(
      const block_t *key,
      const block_t *message,
      const block_t *ctx_in,
      block_t *ctx_out);

/** Finish (final) authentication operation using CMAC in
 *  Init-Update-Final model
 *
 * @param key is the key involved in authentication of a message
 * @param message is the message to authenticate
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_aes_cmac_generate_init or from ::sx_aes_cmac_generate_update
 * @param mac is the output Message Authentication Code of the message
 * @return ::CRYPTOLIB_SUCCESS if authentication generation succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_cmac_generate_final(
      const block_t *key,
      const block_t *message,
      const block_t *ctx_in,
      block_t *mac);

/** Finish (final) authentication verification operation using CMAC in
 *  Init-Update-Final model
 *
 * @param key is the key involved in authentication of a message
 * @param message is the message to authenticate
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_aes_cmac_generate_init or from ::sx_aes_cmac_generate_update
 * @param mac is an input Message Authentication Code as reference for verification
 * @return ::CRYPTOLIB_SUCCESS if authentication verification succeeded
 *         ::CRYPTOLIB_INVALID_SIGN_ERR if reference MAC does not match
 *         recomputed one
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_aes_cmac_generate_verify_final(
      const block_t *key,
      const block_t *message,
      const block_t *ctx_in,
      const block_t *mac);

/**
 * @brief Reload random used in the AES counter-measures.
 *
 * When enabled in HW, counter-measures are available for the AES
 * (See the Technical Report: "Secure and Efficient Masking of AES - A Mission
 * Impossible?", June 2004)
 *
 * \warning It is under the user responsibility to call it after system boot
 *          (not automatically called).
 *
 * @param value new random value used to reload counter-measures
 */
void sx_aes_load_mask(uint32_t value);

#endif
