/**
 * @file
 * @brief Defines the procedures to make operations with block ciphers engines:
 *        BA411E-AES, BA419-SM4 and BA424-ARIA
 * @copyright Copyright (c) 2019 Silex Insight. All Rights reserved
 */

#ifndef SX_BLK_CIPHER_H
#define SX_BLK_CIPHER_H

#include <stdint.h>
#include <stdbool.h>
#include "compiler_extentions.h"
#include "cryptolib_types.h"
#include "cryptodma_internal.h"

enum sx_blk_cipher_engine_select
{
   SX_BLK_CIPHER_AES,  /**< block cipher AES */
   SX_BLK_CIPHER_SM4,  /**< block cipher SM4 */
   SX_BLK_CIPHER_ARIA  /**< block cipher ARIA */
};

enum sx_blk_cipher_mode_select
{
   SX_BLK_CIPHER_MODE_ECB, /**< block cipher mode ECB */
   SX_BLK_CIPHER_MODE_CBC, /**< block cipher mode CBC */
   SX_BLK_CIPHER_MODE_CTR, /**< block cipher mode CTR */
   SX_BLK_CIPHER_MODE_CFB, /**< block cipher mode CFB */
   SX_BLK_CIPHER_MODE_OFB, /**< block cipher mode OFB */
   SX_BLK_CIPHER_MODE_CCM, /**< block cipher mode CCM */
   SX_BLK_CIPHER_MODE_GCM, /**< block cipher mode GCM */
   SX_BLK_CIPHER_MODE_XTS, /**< block cipher mode XTS */
   SX_BLK_CIPHER_MODE_CMA  /**< block cipher mode CMAC */
};

enum sx_blk_cipher_operation
{
   SX_BLK_CIPHER_ENCRYPT, /**< block cipher encrypt */
   SX_BLK_CIPHER_DECRYPT  /**< block cipher decrypt */
};

/** @brief Offset for Configuration word in DMA Scatter-Gather Tag */
#define SX_BLK_CIPHER_OFFSET_CFG        0
/** @brief Offset for Configuration word in DMA Scatter-Gather Tag */
#define SX_BLK_CIPHER_OFFSET_KEY        8
/** @brief Offset for Configuration word in DMA Scatter-Gather Tag */
#define SX_BLK_CIPHER_OFFSET_IV        40
/** @brief Offset for Configuration word in DMA Scatter-Gather Tag */
#define SX_BLK_CIPHER_OFFSET_IV2       56
/** @brief Offset for Configuration word in DMA Scatter-Gather Tag */
#define SX_BLK_CIPHER_OFFSET_KEY2      72
/** @brief Offset for Configuration word in DMA Scatter-Gather Tag */
#define SX_BLK_CIPHER_OFFSET_MASK      104

/** @brief Block cipher Mode Register value for ECB mode of operation */
#define SX_BLK_CIPHER_MODEID_ECB        0x00000100
/** @brief Block cipher Mode Register value for CBC mode of operation */
#define SX_BLK_CIPHER_MODEID_CBC        0x00000200
/** @brief Block cipher Mode Register value for CTR mode of operation */
#define SX_BLK_CIPHER_MODEID_CTR        0x00000400
/** @brief Block cipher Mode Register value for CFB mode of operation */
#define SX_BLK_CIPHER_MODEID_CFB        0x00000800
/** @brief Block cipher Mode Register value for OFB mode of operation */
#define SX_BLK_CIPHER_MODEID_OFB        0x00001000
/** @brief Block cipher Mode Register value for CCM mode of operation */
#define SX_BLK_CIPHER_MODEID_CCM        0x00002000
/** @brief Block cipher Mode Register value for GCM mode of operation */
#define SX_BLK_CIPHER_MODEID_GCM        0x00004000
/** @brief Block cipher Mode Register value for XTS mode of operation */
#define SX_BLK_CIPHER_MODEID_XTS        0x00008000
/** @brief Block cipher Mode Register value for CMAC mode of operation */
#define SX_BLK_CIPHER_MODEID_CMA        0x00010000

/** @brief Block cipher Mode Register value for encryption mode */
#define SX_BLK_CIPHER_MODEID_ENCRYPT    0x00000000
/** @brief Block cipher Mode Register value for decryption mode */
#define SX_BLK_CIPHER_MODEID_DECRYPT    0x00000001

/** @brief Mode Register value for context saving */
#define SX_BLK_CIPHER_MODEID_CX_SAVE    0x00000020
/** @brief Mode Register value for context loading */
#define SX_BLK_CIPHER_MODEID_CX_LOAD    0x00000010
/** @brief Mode Register value for no context */
#define SX_BLK_CIPHER_MODEID_NO_CX      0x00000000
/** @brief Mode Register value for keysize of 128 bits */
#define SX_BLK_CIPHER_MODEID_128        0x00000000
/** @brief Mode Register value for keysize of 192 bits */
#define SX_BLK_CIPHER_MODEID_192        0x00000008
/** @brief Mode Register value for keysize of 256 bits */
#define SX_BLK_CIPHER_MODEID_256        0x00000004
/** @brief Mode Register value to use Key1 */
#define SX_BLK_CIPHER_MODEID_KEY1       0x00000040
/** @brief Mode Register value to use Key2 */
#define SX_BLK_CIPHER_MODEID_KEY2       0x00000080

/** Encryption operation with mode ECB
 *
 * @param engine is the engine used
 * @param operation holds engine operation: encrypt or decrypt
 * @param key is the key involved to encrypt/decrypt the input data
 * @param input is the input data to be encrypted/decrypted
 * @param output is the output encrypted/decrypted data
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_blk_cipher_ecb(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      const block_t *key,
      const block_t *input,
      block_t *output);

/** Encryption operation with mode CBC
 *
 * @param engine is the engine used
 * @param operation holds engine operation: encrypt or decrypt
 * @param key is the key involved to encrypt/decrypt the input data
 * @param input is the input data to be encrypted/decrypted
 * @param output is the output encrypted/decrypted data
 * @param iv is the input initialization vector
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_blk_cipher_cbc(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      const block_t *key,
      const block_t *input,
      block_t *output,
      const block_t *iv);

/** Finish (final) encryption operation with modes CBC
 *  in Init-Update-Final model
 *
 * @param engine is the engine used
 * @param operation holds engine operation: encrypt or decrypt
 * @param key is the key involved to encrypt/decrypt the input data
 * @param input is the input data to be encrypted/decrypted
 * @param output is the output encrypted/decrypted data
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_cbc_init or from ::sx_cbc_update
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_blk_cipher_cbc_final(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      const block_t *key,
      const block_t *datain,
      block_t *dataout,
      const block_t *ctx_in);

/** Start (init) encryption operation with modes CBC, CTR, OFB, and CFB
 *  in Init-Update-Final model
 *
 * @param engine is the engine used
 * @param operation holds engine operation: encrypt or decrypt
 * @param mode is the mode used in block cipher
 * @param key is the key involved to encrypt/decrypt the input data
 * @param input is the input data to be encrypted/decrypted
 * @param output is the output encrypted/decrypted data
 * @param iv is the input initialization vector
 * @param ctx_out is the output intermediate context to forward to
 *                ::sx_cbc_update
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_blk_cipher_cbc_ctr_ofb_cfb_init(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      enum sx_blk_cipher_mode_select mode,
      const block_t *key,
      const block_t *input,
      block_t *output,
      const block_t *iv,
      block_t *ctx_out);

/** Continue (update) encryption operation with modes CBC, CTR, OFB, and CFB
 *  in Init-Update-Final model
 *
 * @param engine is the engine used
 * @param operation holds engine operation: encrypt or decrypt
 * @param mode is the mode used in block cipher
 * @param key is the key involved to encrypt/decrypt the input data
 * @param input is the input data to be encrypted/decrypted
 * @param output is the output encrypted/decrypted data
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_cbc_init or from ::sx_cbc_update
 * @param ctx_out is the output intermediate context to forward to
 *                ::sx_ctr_update or to ::sx_ctr_final
 * @param is_mode_supported is the flag that specifies if mode is supported
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_blk_cipher_cbc_ctr_ofb_cfb_update(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      enum sx_blk_cipher_mode_select mode,
      const block_t *key,
      const block_t *input,
      block_t *output,
      const block_t *ctx_in,
      block_t *ctx_out);

/** Finish (final) encryption operation with modes CBC, CTR, OFB, and CFB
 *  in Init-Update-Final model
 *
 * @param engine is the engine used
 * @param operation holds engine operation: encrypt or decrypt
 * @param mode is the mode used in block cipher
 * @param key is the key involved to encrypt/decrypt the input data
 * @param input is the input data to be encrypted/decrypted
 * @param output is the output encrypted/decrypted data
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_cbc_init or from ::sx_cbc_update
 * @param is_mode_supported is the flag that specifies if mode is supported
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_blk_cipher_cfb_ofb_final(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      enum sx_blk_cipher_mode_select mode,
      const block_t *key,
      const block_t *input,
      block_t *output,
      const block_t *ctx_in);

/** Encryption operation with mode CTR
 *
 * @param engine is the engine used
 * @param operation holds engine operation: encrypt or decrypt
 * @param key is the key involved to encrypt/decrypt the input data
 * @param input is the input data to be encrypted/decrypted
 * @param output is the output encrypted/decrypted data
 * @param iv is the input initialization vector
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_blk_cipher_ctr(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      const block_t *key,
      const block_t *input,
      block_t *output,
      const block_t *iv);

/** Finish (final) encryption operation with mode CTR in Init-Update-Final model
 *
 * @param engine is the engine used
 * @param operation holds engine operation: encrypt or decrypt
 * @param key is the key involved to encrypt/decrypt the input data
 * @param input is the input data to be encrypted/decrypted
 * @param output is the output encrypted/decrypted data
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_ctr_init or from ::sx_ctr_update
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_blk_cipher_ctr_final(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      const block_t *key,
      const block_t *input,
      block_t *output,
      const block_t *ctx_in);


/** Encryption operation with mode CFB or OFB
 *
 * @param engine is the engine used
 * @param operation holds engine operation: encrypt or decrypt
 * @param mode is the mode used in block cipher
 * @param key is the key involved to encrypt/decrypt the input data
 * @param input is the input data to be encrypted/decrypted
 * @param output is the output encrypted/decrypted data
 * @param iv is the input initialization vector
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_blk_cipher_cfb_ofb(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      enum sx_blk_cipher_mode_select mode,
      const block_t *key,
      const block_t *input,
      block_t *output,
      const block_t *iv);

/** Encryption operation with mode XTS
 *
 * @param engine is the engine used
 * @param operation holds engine operation: encrypt or decrypt
 * @param key is the key involved to encrypt/decrypt the input data
 * @param input is the input data to be encrypted/decrypted
 * @param output is the output encrypted/decrypted data
 * @param iv is the input initialization vector
 * @param key_xts is the "tweak" key involved in block cipher XTS
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_blk_cipher_xts(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      const block_t *key,
      const block_t *input,
      block_t *output,
      const block_t *iv,
      const block_t *key_xts);

/** Start (init) encryption operation with mode XTS in Init-Update-Final model
 *
 * @param engine is the engine used
 * @param operation holds engine operation: encrypt or decrypt
 * @param key is the key involved to encrypt/decrypt the input data
 * @param input is the input data to be encrypted/decrypted
 * @param output is the output encrypted/decrypted data
 * @param iv is the input initialization vector
 * @param ctx_out is the output intermediate context to forward to
 *                ::sx_xts_update
 * @param key_xts is the "tweak" key involved in block cipher XTS
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_blk_cipher_xts_init(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      const block_t *key,
      const block_t *input,
      block_t *output,
      const block_t *iv,
      block_t *ctx_out,
      const block_t *key_xts);

/** Continue (update) encryption operation with mode XTS in Init-Update-Final model
 *
 * @param engine is the engine used
 * @param operation holds engine operation: encrypt or decrypt
 * @param key is the key involved to encrypt/decrypt the input data
 * @param input is the input data to be encrypted/decrypted
 * @param output is the output encrypted/decrypted data
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_xts_init or from ::sx_xts_update
 * @param ctx_out is the output intermediate context to forward to
 *                ::sx_xts_update or to ::sx_xts_final
 * @param key_xts is the "tweak" key involved in block cipher XTS
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_blk_cipher_xts_update(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      const block_t *key,
      const block_t *input,
      block_t *output,
      const block_t *ctx_in,
      block_t *ctx_out,
      const block_t *key_xts);


/** Finish (final) encryption operation with mode XTS in Init-Update-Final model
 *
 * @param engine is the engine used
 * @param operation holds engine operation: encrypt or decrypt
 * @param key is the key involved to encrypt/decrypt the input data
 * @param input is the input data to be encrypted/decrypted
 * @param output is the output encrypted/decrypted data
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_xts_init or from ::sx_xts_update
 * @param key_xts is the "tweak" key involved in block cipher XTS
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_blk_cipher_xts_final(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      const block_t *key,
      const block_t *input,
      block_t *output,
      const block_t *ctx_in,
      const block_t *key_xts);

/** Encryption and authentication operation with mode GCM
 *
 * @param engine is the engine used
 * @param operation holds engine operation: encrypt or decrypt
 * @param key is the key involved to encrypt/decrypt and authenticate the input data
 * @param input is the input data to be encrypted/decrypted and authenticate
 * @param output is the output encrypted/decrypted data
 * @param iv is the input initialization vector
 * @param mac is the output Message Authentication Code of the plaintext and aad
 * @param aad are additional data passed aside the plaintext to compute the MAC
 * @return ::CRYPTOLIB_SUCCESS if encryption and authentication succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 * @note If one only needs authentication, this function may be used as GMAC
 *       and user has to only keep the \c plaintext and the \c mac
 */
uint32_t sx_blk_cipher_gcm(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      const block_t *key,
      const block_t *input,
      block_t *output,
      const block_t *iv,
      block_t *mac,
      const block_t *aad);

/** Decryption, authentication and verification operation with mode GCM
 *
 * By opposition to ::sx_gcm_decrypt, this function does not output the MAC
 * but uses an input reference to internally check if it maches the recomputed
 * one. By only checking the returned value of the function, caller knows
 * if the input data (\ciphertext) are tampered or not.
 *
 * @param engine is the engine used
 * @param operation holds engine operation: encrypt or decrypt
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
uint32_t sx_blk_cipher_gcm_decrypt_verify(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *iv,
      const block_t *mac,
      const block_t *aad);

/** Start (init) encryption and authentication operation with mode GCM in
 *  Init-Update-Final model
 *
 * @param engine is the engine used
 * @param operation holds engine operation: encrypt or decrypt
 * @param key is the key involved to encrypt/decrypt and authenticate the input data
 * @param input is the input data to be encrypted/decrypted and authenticate
 * @param output is the output encrypted/decrypted data
 * @param iv is the input initialization vector
 * @param ctx_out is the output intermediate context to forward to
 *                ::sx_gcm_update (expected size is 32B)
 * @param aad are additional data passed aside the plaintext to compute the MAC
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_blk_cipher_gcm_init(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      const block_t *key,
      const block_t *input,
      block_t *output,
      const block_t *iv,
      block_t *ctx_out,
      const block_t *aad);

/** Continue (update) encryption and authentication operation with mode GCM in
 *  Init-Update-Final model
 *
 * @param engine is the engine used
 * @param operation holds engine operation: encrypt or decrypt
 * @param key is the key involved to encrypt/decrypt and authenticate the input data
 * @param input is the input data to be encrypted/decrypted and authenticate
 * @param output is the output encrypted/decrypted data
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_gcm_init or from ::sx_gcm_update
 * @param ctx_out is the output intermediate context to forward to
 *                ::sx_gcm_update or to ::sx_gcm_final
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 * @note the expected size for GCM context is 32B
 */
uint32_t sx_blk_cipher_gcm_update(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      const block_t *key,
      const block_t *input,
      block_t *output,
      const block_t *ctx_in,
      block_t *ctx_out);

/** Finish (final) encryption and authentication operation with mode GCM in
 *  Init-Update-Final model
 *
 * @param engine is the engine used
 * @param operation holds engine operation: encrypt or decrypt
 * @param key is the key involved to encrypt/decrypt and authenticate the input data
 * @param input is the input data to be encrypted/decrypted and authenticate
 * @param output is the output encrypted/decrypted data
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_gcm_init or from ::sx_gcm_update
 * @param mac is the output Message Authentication Code of the plaintext and aad
 * @param len_a_c are length of both \c plaintext and \c aad as described in
 *        NIST SP800-38D at 7.GCM Specification
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 * @note the expected size for GCM context is 32B
 */
uint32_t sx_blk_cipher_gcm_final(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      const block_t *key,
      const block_t *input,
      block_t *output,
      const block_t *ctx_in,
      block_t *mac,
      const block_t *len_a_c);

/** Finish (final) decryption, authentication and verification operation using
 *  block cipher GCM in Init-Update-Final model
 *
 * By opposition to ::sx_gcm_decrypt_final, this function does not output
 * the MAC but uses an input reference to internally check if it maches the
 * recomputed one. By only checking the returned value of the function, caller
 * knows if the input data (\ciphertext) are tampered or not.
 *
 * @param engine is the engine used
 * @param operation holds engine operation: encrypt or decrypt
 * @param key is the key involved to decrypt and authenticate the ciphertext
 * @param ciphertext is the input data to decrypt and authenticate
 * @param plaintext is the output decrypted data
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_gcm_decrypt_init or from ::sx_gcm_decrypt_update
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
uint32_t sx_blk_cipher_gcm_decrypt_verify_final(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *ctx_in,
      const block_t *mac,
      const block_t *len_a_c);

/** Encryption and authentication operation with mode CCM
 *
 * @param engine is the engine used
 * @param operation holds engine operation: encrypt or decrypt
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
uint32_t sx_blk_cipher_ccm_encrypt(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *nonce,
      block_t *mac,
      const block_t *aad);

/** Decryption and authentication operation with mode CCM
 *
 * @param engine is the engine used
 * @param operation holds engine operation: encrypt or decrypt
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
uint32_t sx_blk_cipher_ccm_decrypt(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *nonce,
      block_t *mac,
      const block_t *aad);

/** Decryption, authentication and verification operation with mode CCM
 *
 * @param engine is the engine used
 * @param operation holds engine operation: encrypt or decrypt
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
uint32_t sx_blk_cipher_ccm_decrypt_verify(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *nonce,
      const block_t *mac,
      const block_t *aad);

/** Start (init) encryption and authentication operation with mode CCM in
 *  Init-Update-Final model
 *
 * @param engine is the engine used
 * @param operation holds engine operation: encrypt or decrypt
 * @param key is the key involved to encrypt/decrypt and authenticate the input data
 * @param input is the input data to be encrypted/decrypted and authenticate
 * @param output is the output encrypted/decrypted data
 * @param nonce is the input number to use once
 * @param ctx_out is the output intermediate context to forward to
 *                ::sx_ccm_update (expected size is 32B)
 * @param aad are additional data passed aside the plaintext to compute the MAC
 * @param mac_len is the length of the mac to output. In IUF, it is mandatory
 *        to provide it during init step as this length is required to compute
 *        CCM headers.
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_blk_cipher_ccm_init(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      const block_t *key,
      const block_t *input,
      block_t *output,
      const block_t *nonce,
      block_t *ctx_out,
      const block_t *aad,
      uint32_t mac_len);

/** Continue (update) encryption and authentication operation with mode CCM in
 *  Init-Update-Final model
 *
 * @param engine is the engine used
 * @param operation holds engine operation: encrypt or decrypt
 * @param key is the key involved to encrypt/decrypt and authenticate the input data
 * @param input is the input data to be encrypted/decrypted and authenticate
 * @param output is the output encrypted/decrypted data
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_ccm_init or from ::sx_ccm_update
 * @param ctx_out is the output intermediate context to forward to
 *                ::sx_ccm_update or to ::sx_ccm_final
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 *
 * @note the expected size for CCM context is 32B
 */
uint32_t sx_blk_cipher_ccm_update(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      const block_t *key,
      const block_t *input,
      block_t *output,
      const block_t *ctx_in,
      block_t *ctx_out);

/** Finish (final) encryption and authentication operation with mode CCM in
 *  Init-Update-Final model
 *
 * @param engine is the engine used
 * @param operation holds engine operation: encrypt or decrypt
 * @param key is the key involved to encrypt and authenticate the plaintext
 * @param plaintext is the input data to encrypt and authenticate
 * @param ciphertext is the output encrypted data
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_ccm_init or from ::sx_ccm_update
 * @param mac is the output Message Authentication Code of the plaintext and aad
 * @return ::CRYPTOLIB_SUCCESS if encryption succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 *
 * @note the expected size for CCM context is 32B
 */
uint32_t sx_blk_cipher_ccm_encrypt_final(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *ctx_in,
      block_t *mac);

/** Finish (final) decryption and authentication operation with mode CCM in
 * Init-Update-Final model
 *
 * @param engine is the engine used
 * @param operation holds engine operation: encrypt or decrypt
 * @param key is the key involved to decrypt and authenticate the ciphertext
 * @param ciphertext is the input data to decrypt and authenticate
 * @param plaintext is the output decrypted data
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_ccm_decrypt_init or from ::sx_ccm_decrypt_update
 * @param mac is an input Message Authentication Code as reference for verification
 * @return ::CRYPTOLIB_SUCCESS if decryption and authentication succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 *
 * @param engine is the engine used
 * @param operation holds engine operation: encrypt or decrypt
 * @note the expected size for CCM context is 32B
 */
uint32_t sx_blk_cipher_ccm_decrypt_final(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *ctx_in,
      block_t *mac);

/** Finish (final) decryption, authentication and verification operation using
 *  block cipher CCM in Init-Update-Final model
 *
 * @param engine is the engine used
 * @param operation holds engine operation: encrypt or decrypt
 * @param key is the key involved to decrypt and authenticate the ciphertext
 * @param ciphertext is the input data to decrypt and authenticate
 * @param plaintext is the output decrypted data
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_ccm_decrypt_init or from ::sx_ccm_decrypt_update
 * @param mac is an input Message Authentication Code as reference for verification
 * @return ::CRYPTOLIB_SUCCESS if reference MAC matches recomputed one
 *         ::CRYPTOLIB_INVALID_SIGN_ERR if reference MAC does not match
 *         recomputed one
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 *
 * @note the expected size for CCM context is 32B
 */
uint32_t sx_blk_cipher_ccm_decrypt_verify_final(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *ctx_in,
      const block_t *mac);

/** Authentication operation using CMAC
 *
 * @param engine is the engine used
 * @param key is the key involved in authentication of a message
 * @param message is the message to authenticate
 * @param mac is the output Message Authentication Code
 * @return ::CRYPTOLIB_SUCCESS if authentication generation succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_blk_cipher_cmac_generate(
      enum sx_blk_cipher_engine_select engine,
      const block_t *key,
      const block_t *message,
      block_t *mac);

/** Authentication verification operation using CMAC
 *
 * @param engine is the engine used
 * @param key is the key involved in authentication of a message
 * @param message is the message to authenticate and verify
 * @param mac is an input Message Authentication Code used as reference
 * @return ::CRYPTOLIB_SUCCESS if authentication verifiation succeeded
 *         ::CRYPTOLIB_INVALID_SIGN_ERR if authentication verification failed
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_blk_cipher_cmac_verify(
      enum sx_blk_cipher_engine_select engine,
      const block_t *key,
      const block_t *message,
      const block_t *mac);

/** Start (init) authentication operation using CMAC in Init-Update-Final model
 *
 * @param engine is the engine used
 * @param key is the key involved in authentication of a message
 * @param message is the message to authenticate
 * @param ctx_out is the output intermediate context to forward to
 *                ::sx_cmac_generate_update
 * @return ::CRYPTOLIB_SUCCESS if authentication generation succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_blk_cipher_cmac_generate_init(
      enum sx_blk_cipher_engine_select engine,

      const block_t *key,
      const block_t *message,
      block_t *ctx_out);

/** Continue (update) authentication operation using CMAC in
 *  Init-Update-Final model
 *
 * @param engine is the engine used
 * @param key is the key involved in authentication of a message
 * @param message is the message to authenticate
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_cmac_generate_init or from ::sx_cmac_generate_update
 * @param ctx_out is the output intermediate context to forward to
 *                ::sx_cmac_generate_update
 * @return ::CRYPTOLIB_SUCCESS if authentication generation succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_blk_cipher_cmac_generate_update(
      enum sx_blk_cipher_engine_select engine,
      const block_t *key,
      const block_t *message,
      const block_t *ctx_in,
      block_t *ctx_out);

/** Finish (final) authentication operation using CMAC in
 *  Init-Update-Final model
 *
 * @param engine is the engine used
 * @param key is the key involved in authentication of a message
 * @param message is the message to authenticate
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_cmac_generate_init or from ::sx_cmac_generate_update
 * @param mac is the output Message Authentication Code of the message
 * @return ::CRYPTOLIB_SUCCESS if authentication generation succeeded
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_blk_cipher_cmac_generate_final(
      enum sx_blk_cipher_engine_select engine,
      const block_t *key,
      const block_t *message,
      const block_t *ctx_in,
      block_t *mac);

/** Finish (final) authentication verification operation using CMAC in
 *  Init-Update-Final model
 *
 * @param engine is the engine used
 * @param key is the key involved in authentication of a message
 * @param message is the message to authenticate
 * @param ctx_in is the input intermediate context outputted from
 *               ::sx_cmac_generate_init or from ::sx_cmac_generate_update
 * @param mac is an input Message Authentication Code as reference for verification
 * @return ::CRYPTOLIB_SUCCESS if authentication verification succeeded
 *         ::CRYPTOLIB_INVALID_SIGN_ERR if reference MAC does not match
 *         recomputed one
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the key length is not supported
 */
uint32_t sx_blk_cipher_cmac_generate_verify_final(
      enum sx_blk_cipher_engine_select engine,
      const block_t *key,
      const block_t *message,
      const block_t *ctx_in,
      const block_t *mac);
#endif
