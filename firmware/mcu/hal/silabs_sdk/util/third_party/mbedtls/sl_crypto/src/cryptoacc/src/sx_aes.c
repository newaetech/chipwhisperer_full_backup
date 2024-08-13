/**
 * @file
 * @brief Defines the procedures to make operations with
 *          the BA411E AES
 * @copyright Copyright (c) 2016-2019 Silex Insight. All Rights reserved
 */


#include <stdbool.h>
#include <stddef.h>
#include <string.h> //for memset
#include "sx_aes.h"
#include "cryptolib_def.h"
#include "cryptodma_internal.h"
#include "sx_memcpy.h"
#include "sx_memcmp.h"
#include "ba411e_config.h"
#include "sx_math.h"
#include "sx_trng.h"
#include "sx_errors.h"
#include "sx_blk_cipher.h"

uint8_t aes_hw_key1;
uint8_t aes_hw_key2;

/** @brief This function is called in sx_blk_cipher.c
 *         Update fields related to the key characteristics for block cipher
 *         operation
 *
 * The block cipher engines have a 32b configuration register with many fields.
 * A few of them depends of the block cipher key itself. The function set these
 * fields only and let the others unchanged. It allows to decouple key related
 * configuration with the algorithm (CBC/ECB,...) and the operation
 * (encryption/decryption, ...)
 *
 * @param key block cipher key involved.
 * @param config A 32b word to update based on the \c key characteristics
 * @return CRYPTOLIB_SUCCESS if key length is recognised (128 ,192 or 256b) and
 *         supported.
 *         CRYPTOLIB_UNSUPPORTED_ERR if key length is recognized but not supported
 *         CRYPTOLIB_INVALID_PARAM if key length is not recognised.
 */
uint32_t sx_aes_set_hw_config_for_key(block_t *key, uint32_t *config)
{
   // Check and configure for key length
   switch(key->len) {
   case 16:
      if ((BA411E_HW_CFG_1 & AES_HW_CFG_KEY_SIZE_128_SUPPORTED_MASK)
           != AES_HW_CFG_KEY_SIZE_128_SUPPORTED_MASK)
         return CRYPTOLIB_UNSUPPORTED_ERR;
      *config |= SX_BLK_CIPHER_MODEID_128;
      break;
   case 24:
      if ((BA411E_HW_CFG_1 & AES_HW_CFG_KEY_SIZE_192_SUPPORTED_MASK)
           != AES_HW_CFG_KEY_SIZE_192_SUPPORTED_MASK)
         return CRYPTOLIB_UNSUPPORTED_ERR;
      *config |= SX_BLK_CIPHER_MODEID_192;
      break;
   case 32:
      if ((BA411E_HW_CFG_1 & AES_HW_CFG_KEY_SIZE_256_SUPPORTED_MASK)
           != AES_HW_CFG_KEY_SIZE_256_SUPPORTED_MASK)
         return CRYPTOLIB_UNSUPPORTED_ERR;
      *config |= SX_BLK_CIPHER_MODEID_256;
      break;
   default:
      return CRYPTOLIB_INVALID_PARAM;
    }

   // Configure to use HW keys
   if (key->addr == &aes_hw_key1) {
      *config |= SX_BLK_CIPHER_MODEID_KEY1;
      key->len = 0; // We don't need to transfer key content when using HW ones
   }
   else if (key->addr == &aes_hw_key2) {
      *config |= SX_BLK_CIPHER_MODEID_KEY2;
      key->len = 0; // We don't need to transfer key content when using HW ones
   }
   return CRYPTOLIB_SUCCESS;
}

// AES-ECB
// -----------------------------------------------------------------------------
uint32_t sx_aes_ecb_encrypt(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_ECB_SUPPORTED_MASK)
        != AES_HW_CFG_ECB_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_ecb(SX_BLK_CIPHER_AES, SX_BLK_CIPHER_ENCRYPT,
         key, plaintext, ciphertext);
}

uint32_t sx_aes_ecb_decrypt(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_ECB_SUPPORTED_MASK)
        != AES_HW_CFG_ECB_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_ecb(SX_BLK_CIPHER_AES, SX_BLK_CIPHER_DECRYPT,
         key, ciphertext, plaintext);
}

// AES-CBC
// -----------------------------------------------------------------------------
uint32_t sx_aes_cbc_encrypt(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *iv)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_CBC_SUPPORTED_MASK)
        != AES_HW_CFG_CBC_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_cbc(SX_BLK_CIPHER_AES, SX_BLK_CIPHER_ENCRYPT,
         key, plaintext, ciphertext, iv);
}
uint32_t sx_aes_cbc_decrypt(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *iv)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_CBC_SUPPORTED_MASK)
        != AES_HW_CFG_CBC_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_cbc(SX_BLK_CIPHER_AES, SX_BLK_CIPHER_DECRYPT,
         key, ciphertext, plaintext, iv);
}

uint32_t sx_aes_cbc_encrypt_init(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *iv,
      block_t *ctx_out)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_CBC_SUPPORTED_MASK)
        != AES_HW_CFG_CBC_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_cbc_ctr_ofb_cfb_init(SX_BLK_CIPHER_AES,
         SX_BLK_CIPHER_ENCRYPT, SX_BLK_CIPHER_MODE_CBC, key, plaintext,
         ciphertext, iv, ctx_out);
}

uint32_t sx_aes_cbc_decrypt_init(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *iv,
      block_t *ctx_out)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_CBC_SUPPORTED_MASK)
        != AES_HW_CFG_CBC_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_cbc_ctr_ofb_cfb_init(SX_BLK_CIPHER_AES,
         SX_BLK_CIPHER_DECRYPT, SX_BLK_CIPHER_MODE_CBC, key, ciphertext,
         plaintext, iv, ctx_out);
}

uint32_t sx_aes_cbc_encrypt_update(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *ctx_in,
      block_t *ctx_out)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_CBC_SUPPORTED_MASK)
        != AES_HW_CFG_CBC_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_cbc_ctr_ofb_cfb_update(SX_BLK_CIPHER_AES,
         SX_BLK_CIPHER_ENCRYPT, SX_BLK_CIPHER_MODE_CBC, key, plaintext,
         ciphertext, ctx_in, ctx_out);
}

uint32_t sx_aes_cbc_decrypt_update(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *ctx_in,
      block_t *ctx_out)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_CBC_SUPPORTED_MASK)
        != AES_HW_CFG_CBC_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_cbc_ctr_ofb_cfb_update(SX_BLK_CIPHER_AES,
         SX_BLK_CIPHER_DECRYPT, SX_BLK_CIPHER_MODE_CBC, key, ciphertext,
         plaintext, ctx_in, ctx_out);
}

uint32_t sx_aes_cbc_encrypt_final(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *ctx_in)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_CBC_SUPPORTED_MASK)
        != AES_HW_CFG_CBC_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_cbc_final(SX_BLK_CIPHER_AES, SX_BLK_CIPHER_ENCRYPT,
         key, plaintext, ciphertext, ctx_in);
}

uint32_t sx_aes_cbc_decrypt_final(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *ctx_in)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_CBC_SUPPORTED_MASK)
        != AES_HW_CFG_CBC_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_cbc_final(SX_BLK_CIPHER_AES, SX_BLK_CIPHER_DECRYPT,
         key, ciphertext, plaintext, ctx_in);
}

// AES-CTR
// -----------------------------------------------------------------------------
uint32_t sx_aes_ctr_encrypt(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *iv)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_CTR_SUPPORTED_MASK)
        != AES_HW_CFG_CTR_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_ctr(SX_BLK_CIPHER_AES, SX_BLK_CIPHER_ENCRYPT,
         key, plaintext, ciphertext, iv);
}

uint32_t sx_aes_ctr_decrypt(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *iv)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_CTR_SUPPORTED_MASK)
        != AES_HW_CFG_CTR_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_ctr(SX_BLK_CIPHER_AES, SX_BLK_CIPHER_DECRYPT,
         key, ciphertext, plaintext, iv);
}

uint32_t sx_aes_ctr_encrypt_init(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *iv,
      block_t *ctx_out)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_CTR_SUPPORTED_MASK)
        != AES_HW_CFG_CTR_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_cbc_ctr_ofb_cfb_init(SX_BLK_CIPHER_AES,
         SX_BLK_CIPHER_ENCRYPT, SX_BLK_CIPHER_MODE_CTR, key, plaintext,
         ciphertext, iv, ctx_out);
}

uint32_t sx_aes_ctr_decrypt_init(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *iv,
      block_t *ctx_out)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_CTR_SUPPORTED_MASK)
        != AES_HW_CFG_CTR_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_cbc_ctr_ofb_cfb_init(SX_BLK_CIPHER_AES,
         SX_BLK_CIPHER_DECRYPT, SX_BLK_CIPHER_MODE_CTR, key, ciphertext,
         plaintext, iv, ctx_out);
}

uint32_t sx_aes_ctr_encrypt_update(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *ctx_in,
      block_t *ctx_out)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_CTR_SUPPORTED_MASK)
        != AES_HW_CFG_CTR_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_cbc_ctr_ofb_cfb_update(SX_BLK_CIPHER_AES,
         SX_BLK_CIPHER_ENCRYPT, SX_BLK_CIPHER_MODE_CTR, key, plaintext,
         ciphertext, ctx_in, ctx_out);
}

uint32_t sx_aes_ctr_decrypt_update(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *ctx_in,
      block_t *ctx_out)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_CTR_SUPPORTED_MASK)
        != AES_HW_CFG_CTR_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_cbc_ctr_ofb_cfb_update(SX_BLK_CIPHER_AES,
         SX_BLK_CIPHER_DECRYPT, SX_BLK_CIPHER_MODE_CTR, key, ciphertext,
         plaintext, ctx_in, ctx_out);
}

uint32_t sx_aes_ctr_encrypt_final(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *ctx_in)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_CTR_SUPPORTED_MASK)
        != AES_HW_CFG_CTR_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_ctr_final(SX_BLK_CIPHER_AES, SX_BLK_CIPHER_ENCRYPT,
         key, plaintext, ciphertext, ctx_in);
}

uint32_t sx_aes_ctr_decrypt_final(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *ctx_in)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_CTR_SUPPORTED_MASK)
        != AES_HW_CFG_CTR_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_ctr_final(SX_BLK_CIPHER_AES, SX_BLK_CIPHER_DECRYPT,
         key, ciphertext, plaintext, ctx_in);
}

// AES-CFB
// -----------------------------------------------------------------------------
uint32_t sx_aes_cfb_encrypt(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *iv)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_CFB_SUPPORTED_MASK)
        != AES_HW_CFG_CFB_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_cfb_ofb(SX_BLK_CIPHER_AES, SX_BLK_CIPHER_ENCRYPT,
         SX_BLK_CIPHER_MODE_CFB, key, plaintext, ciphertext, iv);
}

uint32_t sx_aes_cfb_decrypt(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *iv)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_CFB_SUPPORTED_MASK)
        != AES_HW_CFG_CFB_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_cfb_ofb(SX_BLK_CIPHER_AES, SX_BLK_CIPHER_DECRYPT,
         SX_BLK_CIPHER_MODE_CFB, key, ciphertext, plaintext, iv);
}

uint32_t sx_aes_cfb_encrypt_init(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *iv,
      block_t *ctx_out)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_CFB_SUPPORTED_MASK)
        != AES_HW_CFG_CFB_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_cbc_ctr_ofb_cfb_init(SX_BLK_CIPHER_AES,
         SX_BLK_CIPHER_ENCRYPT, SX_BLK_CIPHER_MODE_CFB, key, plaintext,
         ciphertext, iv, ctx_out);
}

uint32_t sx_aes_cfb_decrypt_init(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *iv,
      block_t *ctx_out)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_CFB_SUPPORTED_MASK)
        != AES_HW_CFG_CFB_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_cbc_ctr_ofb_cfb_init(SX_BLK_CIPHER_AES,
         SX_BLK_CIPHER_DECRYPT, SX_BLK_CIPHER_MODE_CFB, key, ciphertext,
         plaintext, iv, ctx_out);
}

uint32_t sx_aes_cfb_encrypt_update(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *ctx_in,
      block_t *ctx_out)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_CFB_SUPPORTED_MASK)
        != AES_HW_CFG_CFB_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_cbc_ctr_ofb_cfb_update(SX_BLK_CIPHER_AES,
         SX_BLK_CIPHER_ENCRYPT, SX_BLK_CIPHER_MODE_CFB, key, plaintext,
         ciphertext, ctx_in, ctx_out);
}

uint32_t sx_aes_cfb_decrypt_update(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *ctx_in,
      block_t *ctx_out)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_CFB_SUPPORTED_MASK)
        != AES_HW_CFG_CFB_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_cbc_ctr_ofb_cfb_update(SX_BLK_CIPHER_AES,
         SX_BLK_CIPHER_DECRYPT, SX_BLK_CIPHER_MODE_CFB, key, ciphertext,
         plaintext, ctx_in, ctx_out);
}

uint32_t sx_aes_cfb_encrypt_final(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *ctx_in)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_CFB_SUPPORTED_MASK)
        != AES_HW_CFG_CFB_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_cfb_ofb_final(SX_BLK_CIPHER_AES, SX_BLK_CIPHER_ENCRYPT,
         SX_BLK_CIPHER_MODE_CFB, key, plaintext, ciphertext, ctx_in);
}

uint32_t sx_aes_cfb_decrypt_final(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *ctx_in)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_CFB_SUPPORTED_MASK)
        != AES_HW_CFG_CFB_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_cfb_ofb_final(SX_BLK_CIPHER_AES, SX_BLK_CIPHER_DECRYPT,
         SX_BLK_CIPHER_MODE_CFB, key, ciphertext, plaintext, ctx_in);
}

// AES-OFB
// -----------------------------------------------------------------------------
uint32_t sx_aes_ofb_encrypt(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *iv)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_OFB_SUPPORTED_MASK)
        != AES_HW_CFG_OFB_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_cfb_ofb(SX_BLK_CIPHER_AES, SX_BLK_CIPHER_ENCRYPT,
         SX_BLK_CIPHER_MODE_OFB, key, plaintext, ciphertext, iv);
}

uint32_t sx_aes_ofb_decrypt(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *iv)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_OFB_SUPPORTED_MASK)
        != AES_HW_CFG_OFB_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_cfb_ofb(SX_BLK_CIPHER_AES, SX_BLK_CIPHER_DECRYPT,
         SX_BLK_CIPHER_MODE_OFB, key, ciphertext, plaintext, iv);
}

uint32_t sx_aes_ofb_encrypt_init(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *iv,
      block_t *ctx_out)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_OFB_SUPPORTED_MASK)
        != AES_HW_CFG_OFB_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_cbc_ctr_ofb_cfb_init(SX_BLK_CIPHER_AES,
         SX_BLK_CIPHER_ENCRYPT, SX_BLK_CIPHER_MODE_OFB, key, plaintext,
         ciphertext, iv, ctx_out);
}

uint32_t sx_aes_ofb_decrypt_init(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *iv,
      block_t *ctx_out)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_OFB_SUPPORTED_MASK)
        != AES_HW_CFG_OFB_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_cbc_ctr_ofb_cfb_init(SX_BLK_CIPHER_AES,
         SX_BLK_CIPHER_DECRYPT, SX_BLK_CIPHER_MODE_OFB, key, ciphertext,
         plaintext, iv, ctx_out);
}

uint32_t sx_aes_ofb_encrypt_update(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *ctx_in,
      block_t *ctx_out)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_OFB_SUPPORTED_MASK)
        != AES_HW_CFG_OFB_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_cbc_ctr_ofb_cfb_update(SX_BLK_CIPHER_AES,
         SX_BLK_CIPHER_ENCRYPT, SX_BLK_CIPHER_MODE_OFB, key, plaintext,
         ciphertext, ctx_in, ctx_out);
}

uint32_t sx_aes_ofb_decrypt_update(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *ctx_in,
      block_t *ctx_out)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_OFB_SUPPORTED_MASK)
        != AES_HW_CFG_OFB_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_cbc_ctr_ofb_cfb_update(SX_BLK_CIPHER_AES,
         SX_BLK_CIPHER_DECRYPT, SX_BLK_CIPHER_MODE_OFB, key, ciphertext,
         plaintext, ctx_in, ctx_out);
}

uint32_t sx_aes_ofb_encrypt_final(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *ctx_in)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_OFB_SUPPORTED_MASK)
        != AES_HW_CFG_OFB_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_cfb_ofb_final(SX_BLK_CIPHER_AES, SX_BLK_CIPHER_ENCRYPT,
         SX_BLK_CIPHER_MODE_OFB, key, plaintext, ciphertext, ctx_in);
}

uint32_t sx_aes_ofb_decrypt_final(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *ctx_in)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_OFB_SUPPORTED_MASK)
        != AES_HW_CFG_OFB_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_cfb_ofb_final(SX_BLK_CIPHER_AES, SX_BLK_CIPHER_DECRYPT,
         SX_BLK_CIPHER_MODE_OFB, key, ciphertext, plaintext, ctx_in);
}

// AES-XTS
// -----------------------------------------------------------------------------
uint32_t sx_aes_xts_encrypt(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *iv,
      const block_t *key_xts)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_XTS_SUPPORTED_MASK)
        != AES_HW_CFG_XTS_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_xts(SX_BLK_CIPHER_AES, SX_BLK_CIPHER_ENCRYPT,
         key, plaintext, ciphertext, iv, key_xts);
}

uint32_t sx_aes_xts_decrypt(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *iv,
      const block_t *key_xts)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_XTS_SUPPORTED_MASK)
        != AES_HW_CFG_XTS_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_xts(SX_BLK_CIPHER_AES, SX_BLK_CIPHER_DECRYPT,
         key, ciphertext, plaintext, iv, key_xts);
}

uint32_t sx_aes_xts_encrypt_init(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *iv,
      block_t *ctx_out,
      const block_t *key_xts)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_XTS_SUPPORTED_MASK)
        != AES_HW_CFG_XTS_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_xts_init(SX_BLK_CIPHER_AES, SX_BLK_CIPHER_ENCRYPT,
         key, plaintext, ciphertext, iv, ctx_out,
         key_xts);
}

uint32_t sx_aes_xts_decrypt_init(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *iv,
      block_t *ctx_out,
      const block_t *key_xts)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_XTS_SUPPORTED_MASK)
        != AES_HW_CFG_XTS_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_xts_init(SX_BLK_CIPHER_AES, SX_BLK_CIPHER_DECRYPT,
         key, ciphertext, plaintext, iv, ctx_out,
         key_xts);
}

uint32_t sx_aes_xts_encrypt_update(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *ctx_in,
      block_t *ctx_out,
      const block_t *key_xts)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_XTS_SUPPORTED_MASK)
        != AES_HW_CFG_XTS_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_xts_update(SX_BLK_CIPHER_AES, SX_BLK_CIPHER_ENCRYPT,
         key, plaintext, ciphertext, ctx_in, ctx_out,
         key_xts);
}

uint32_t sx_aes_xts_decrypt_update(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *ctx_in,
      block_t *ctx_out,
      const block_t *key_xts)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_XTS_SUPPORTED_MASK)
        != AES_HW_CFG_XTS_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_xts_update(SX_BLK_CIPHER_AES, SX_BLK_CIPHER_DECRYPT,
         key, ciphertext, plaintext, ctx_in, ctx_out,
         key_xts);
}

uint32_t sx_aes_xts_encrypt_final(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *ctx_in,
      const block_t *key_xts)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_XTS_SUPPORTED_MASK)
        != AES_HW_CFG_XTS_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_xts_final(SX_BLK_CIPHER_AES, SX_BLK_CIPHER_ENCRYPT,
         key, plaintext, ciphertext, ctx_in, key_xts);
}

uint32_t sx_aes_xts_decrypt_final(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *ctx_in,
      const block_t *key_xts)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_XTS_SUPPORTED_MASK)
        != AES_HW_CFG_XTS_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_xts_final(SX_BLK_CIPHER_AES, SX_BLK_CIPHER_DECRYPT,
         key, ciphertext, plaintext, ctx_in, key_xts);
}

// AES-GCM
// -----------------------------------------------------------------------------
uint32_t sx_aes_gcm_encrypt(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *iv,
      block_t *mac,
      const block_t *aad)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_GCM_SUPPORTED_MASK)
        != AES_HW_CFG_GCM_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_gcm(SX_BLK_CIPHER_AES, SX_BLK_CIPHER_ENCRYPT,
         key, plaintext, ciphertext, iv, mac, aad);
}

uint32_t sx_aes_gcm_decrypt(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *iv,
      block_t *mac,
      const block_t *aad)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_GCM_SUPPORTED_MASK)
        != AES_HW_CFG_GCM_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_gcm(SX_BLK_CIPHER_AES, SX_BLK_CIPHER_DECRYPT,
         key, ciphertext, plaintext, iv, mac, aad);
}

uint32_t sx_aes_gcm_decrypt_verify(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *iv,
      const block_t *mac,
      const block_t *aad)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_GCM_SUPPORTED_MASK)
        != AES_HW_CFG_GCM_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_gcm_decrypt_verify(SX_BLK_CIPHER_AES,
         SX_BLK_CIPHER_DECRYPT, key, ciphertext,
         plaintext, iv, mac, aad);
}

uint32_t sx_aes_gcm_encrypt_init(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *iv,
      block_t *ctx_out,
      const block_t *aad)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_GCM_SUPPORTED_MASK)
        != AES_HW_CFG_GCM_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_gcm_init(SX_BLK_CIPHER_AES, SX_BLK_CIPHER_ENCRYPT,
         key, plaintext, ciphertext, iv, ctx_out, aad);

}

uint32_t sx_aes_gcm_decrypt_init(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *iv,
      block_t *ctx_out,
      const block_t *aad)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_GCM_SUPPORTED_MASK)
        != AES_HW_CFG_GCM_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_gcm_init(SX_BLK_CIPHER_AES, SX_BLK_CIPHER_DECRYPT,
         key, ciphertext, plaintext, iv, ctx_out, aad);
}

uint32_t sx_aes_gcm_encrypt_update(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *ctx_in,
      block_t *ctx_out)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_GCM_SUPPORTED_MASK)
        != AES_HW_CFG_GCM_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_gcm_update(SX_BLK_CIPHER_AES, SX_BLK_CIPHER_ENCRYPT,
         key, plaintext, ciphertext, ctx_in, ctx_out);
}

uint32_t sx_aes_gcm_decrypt_update(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *ctx_in,
      block_t *ctx_out)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_GCM_SUPPORTED_MASK)
        != AES_HW_CFG_GCM_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_gcm_update(SX_BLK_CIPHER_AES, SX_BLK_CIPHER_DECRYPT,
         key, ciphertext, plaintext, ctx_in, ctx_out);
}

uint32_t sx_aes_gcm_encrypt_final(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *ctx_in,
      block_t *mac,
      const block_t *len_a_c)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_GCM_SUPPORTED_MASK)
        != AES_HW_CFG_GCM_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_gcm_final(SX_BLK_CIPHER_AES, SX_BLK_CIPHER_ENCRYPT,
         key, plaintext, ciphertext, ctx_in, mac,
         len_a_c);
}

uint32_t sx_aes_gcm_decrypt_final(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *ctx_in,
      block_t *mac,
      const block_t *len_a_c)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_GCM_SUPPORTED_MASK)
        != AES_HW_CFG_GCM_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_gcm_final(SX_BLK_CIPHER_AES, SX_BLK_CIPHER_DECRYPT,
         key, ciphertext, plaintext, ctx_in, mac,
         len_a_c);
}

uint32_t sx_aes_gcm_decrypt_verify_final(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *ctx_in,
      const block_t *mac,
      const block_t *len_a_c)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_GCM_SUPPORTED_MASK)
        != AES_HW_CFG_GCM_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_gcm_decrypt_verify_final(SX_BLK_CIPHER_AES,
         SX_BLK_CIPHER_DECRYPT, key, ciphertext,
         plaintext, ctx_in, mac, len_a_c);
}

// AES-CCM
// -----------------------------------------------------------------------------
uint32_t sx_aes_ccm_encrypt(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *nonce,
      block_t *mac,
      const block_t *aad)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_CCM_SUPPORTED_MASK)
        != AES_HW_CFG_CCM_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_ccm_encrypt(SX_BLK_CIPHER_AES, SX_BLK_CIPHER_ENCRYPT,
         key, plaintext, ciphertext, nonce, mac, aad);
}

uint32_t sx_aes_ccm_decrypt(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *nonce,
      block_t *mac,
      const block_t *aad)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_CCM_SUPPORTED_MASK)
        != AES_HW_CFG_CCM_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_ccm_decrypt(SX_BLK_CIPHER_AES, SX_BLK_CIPHER_DECRYPT,
         key, ciphertext, plaintext, nonce, mac, aad);
}

uint32_t sx_aes_ccm_decrypt_verify(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *nonce,
      const block_t *mac,
      const block_t *aad)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_CCM_SUPPORTED_MASK)
        != AES_HW_CFG_CCM_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_ccm_decrypt_verify(SX_BLK_CIPHER_AES,
         SX_BLK_CIPHER_DECRYPT, key, ciphertext,
         plaintext, nonce, mac, aad);
}

uint32_t sx_aes_ccm_encrypt_init(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *nonce,
      block_t *ctx_out,
      const block_t *aad,
      uint32_t mac_len)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_CCM_SUPPORTED_MASK)
        != AES_HW_CFG_CCM_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_ccm_init(SX_BLK_CIPHER_AES, SX_BLK_CIPHER_ENCRYPT,
         key, plaintext, ciphertext, nonce, ctx_out,
         aad, mac_len);
}

uint32_t sx_aes_ccm_decrypt_init(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *nonce,
      block_t *ctx_out,
      const block_t *aad,
      uint32_t mac_len)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_CCM_SUPPORTED_MASK)
        != AES_HW_CFG_CCM_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_ccm_init(SX_BLK_CIPHER_AES, SX_BLK_CIPHER_DECRYPT,
         key, ciphertext, plaintext, nonce, ctx_out,
         aad, mac_len);
}


uint32_t sx_aes_ccm_encrypt_update(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *ctx_in,
      block_t *ctx_out)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_CCM_SUPPORTED_MASK)
        != AES_HW_CFG_CCM_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_ccm_update(SX_BLK_CIPHER_AES, SX_BLK_CIPHER_ENCRYPT,
         key, plaintext, ciphertext, ctx_in, ctx_out);
}

uint32_t sx_aes_ccm_decrypt_update(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *ctx_in,
      block_t *ctx_out)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_CCM_SUPPORTED_MASK)
        != AES_HW_CFG_CCM_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_ccm_update(SX_BLK_CIPHER_AES, SX_BLK_CIPHER_DECRYPT,
         key, ciphertext, plaintext, ctx_in, ctx_out);
}

uint32_t sx_aes_ccm_encrypt_final(
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *ctx_in,
      block_t *mac)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_CCM_SUPPORTED_MASK)
        != AES_HW_CFG_CCM_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_ccm_encrypt_final(SX_BLK_CIPHER_AES,
         SX_BLK_CIPHER_ENCRYPT, key, plaintext,
         ciphertext, ctx_in, mac);
}

uint32_t sx_aes_ccm_decrypt_final(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *ctx_in,
      block_t *mac)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_CCM_SUPPORTED_MASK)
        != AES_HW_CFG_CCM_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_ccm_decrypt_final(SX_BLK_CIPHER_AES,
         SX_BLK_CIPHER_DECRYPT, key, ciphertext,
         plaintext, ctx_in, mac);
}

uint32_t sx_aes_ccm_decrypt_verify_final(
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *ctx_in,
      const block_t *mac)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_CCM_SUPPORTED_MASK)
        != AES_HW_CFG_CCM_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_ccm_decrypt_verify_final(SX_BLK_CIPHER_AES,
         SX_BLK_CIPHER_DECRYPT, key, ciphertext,
         plaintext, ctx_in, mac);
}

// AES-CMAC
// -----------------------------------------------------------------------------
// Whole
uint32_t sx_aes_cmac_generate(
      const block_t *key,
      const block_t *message,
      block_t *mac)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_CMAC_SUPPORTED_MASK)
        != AES_HW_CFG_CMAC_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_cmac_generate(SX_BLK_CIPHER_AES, key, message, mac);
}

uint32_t sx_aes_cmac_verify(
      const block_t *key,
      const block_t *message,
      const block_t *mac)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_CMAC_SUPPORTED_MASK)
        != AES_HW_CFG_CMAC_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_cmac_verify(SX_BLK_CIPHER_AES, key, message, mac);
}

// Init
uint32_t sx_aes_cmac_generate_init(
      const block_t *key,
      const block_t *message,
      block_t *ctx_out)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_CMAC_SUPPORTED_MASK)
        != AES_HW_CFG_CMAC_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_cmac_generate_init(SX_BLK_CIPHER_AES, key, message,
         ctx_out);
}

// Update
uint32_t sx_aes_cmac_generate_update(
      const block_t *key,
      const block_t *message,
      const block_t *ctx_in,
      block_t *ctx_out)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_CMAC_SUPPORTED_MASK)
        != AES_HW_CFG_CMAC_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_cmac_generate_update(SX_BLK_CIPHER_AES, key, message,
         ctx_in, ctx_out);
}

// Final
uint32_t sx_aes_cmac_generate_final(
      const block_t *key,
      const block_t *message,
      const block_t *ctx_in,
      block_t *mac)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_CMAC_SUPPORTED_MASK)
        != AES_HW_CFG_CMAC_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_cmac_generate_final(SX_BLK_CIPHER_AES, key, message,
         ctx_in, mac);
}

uint32_t sx_aes_cmac_generate_verify_final(
      const block_t *key,
      const block_t *message,
      const block_t *ctx_in,
      const block_t *mac)
{
   if ((BA411E_HW_CFG_1 & AES_HW_CFG_CMAC_SUPPORTED_MASK)
        != AES_HW_CFG_CMAC_SUPPORTED_MASK)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_blk_cipher_cmac_generate_verify_final(SX_BLK_CIPHER_AES, key,
         message, ctx_in, mac);
}

void sx_aes_load_mask(uint32_t value)
{
   struct dma_sg_descr_s desc_to;
   struct dma_sg_descr_s desc_fr;

   // Fetcher descriptor to store random in AES
   write_desc(
         &desc_to,
         &value,
         sizeof(value),
         DMA_AXI_DESCR_REALIGN,
         DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISCONFIG |
         DMA_SG_TAG_SETCFGOFFSET(SX_BLK_CIPHER_OFFSET_MASK));
   set_last_desc(&desc_to);

   // Dummy pusher descriptor
   write_desc_always(&desc_fr, NULL, 0, DMA_AXI_DESCR_REALIGN, 0);
   set_last_desc(&desc_fr);

   // RUN
   cryptodma_run_sg(&desc_to, &desc_fr);
}
