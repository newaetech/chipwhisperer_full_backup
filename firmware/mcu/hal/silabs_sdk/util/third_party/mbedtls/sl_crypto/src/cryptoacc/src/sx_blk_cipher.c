/**
 * @file
 * @brief Defines the procedures to make operations with block ciphers engines:
 *        BA411E-AES, BA419-SM4 and BA424-ARIA
 * @copyright Copyright (c) 2019 Silex Insight. All Rights reserved
 */

#include <stddef.h>
#include <string.h> //for memset
#include <stdbool.h>
#include "sx_aes.h"
#include "ba411e_config.h"
#include "cryptolib_def.h"
#include "cryptodma_internal.h"
#include "sx_memcpy.h"
#include "sx_memcmp.h"
#include "sx_math.h"
#include "sx_trng.h"
#include "sx_errors.h"
#include "sx_blk_cipher.h"

extern uint32_t sx_aes_set_hw_config_for_key(block_t *key, uint32_t *config);
extern uint32_t sx_sm4_set_hw_config_for_key(block_t *key, uint32_t *config);
extern uint32_t sx_aria_set_hw_config_for_key(block_t *key, uint32_t *config);

const uint32_t engine_select[] = {
      DMA_SG_ENGINESELECT_BA411E,
      DMA_SG_ENGINESELECT_BA419,
      DMA_SG_ENGINESELECT_BA424};

const uint32_t operation_select[] = {
      SX_BLK_CIPHER_MODEID_ENCRYPT,
      SX_BLK_CIPHER_MODEID_DECRYPT};

const uint32_t mode_select[] = {
      SX_BLK_CIPHER_MODEID_ECB,
      SX_BLK_CIPHER_MODEID_CBC,
      SX_BLK_CIPHER_MODEID_CTR,
      SX_BLK_CIPHER_MODEID_CFB,
      SX_BLK_CIPHER_MODEID_OFB,
      SX_BLK_CIPHER_MODEID_CCM,
      SX_BLK_CIPHER_MODEID_GCM,
      SX_BLK_CIPHER_MODEID_XTS,
      SX_BLK_CIPHER_MODEID_CMA};

/* Number of descriptors used for pushing data to block cipher engine */
#define DESC_N_TO_ENGINE  8

/* Number of descriptors used for fetching data from block cipher engine */
#define DESC_N_FROM_ENGINE  6

uint32_t generate_ccm_header(
      block_t nonce,
      uint32_t aad_len,
      uint32_t data_len,
      uint32_t tag_len,
      block_t *header);

static uint32_t sx_blk_cipher_build_descr(
      enum sx_blk_cipher_engine_select engine,
      uint32_t hw_config,
      const block_t *key,
      const block_t *xtskey,
      const block_t *iv,
      const block_t *datain,
      block_t *dataout,
      const block_t *aad1,
      const block_t *aad2,
      const block_t *extrain,
      block_t *tag_out,
      block_t *ctx_ptr);

bool is_len_valid_for_cbc_xts(enum sx_blk_cipher_engine_select engine, uint32_t len)
{
   if(SX_BLK_CIPHER_AES == engine)
      if ((BA411E_HW_CFG_1 & AES_HW_CFG_CS_EN_MASK) == AES_HW_CFG_CS_EN_MASK)
         return len >= BLK_CIPHER_BLOCK_SIZE;
   return !(len % BLK_CIPHER_BLOCK_SIZE) && (len > 0);
}

// ECB
// -----------------------------------------------------------------------------
uint32_t sx_blk_cipher_ecb(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      const block_t *key,
      const block_t *datain,
      block_t *dataout)
{
   if ((datain->len % 16) || !datain->len || (dataout->len != datain->len))
      return CRYPTOLIB_INVALID_PARAM;

   uint32_t config = operation_select[operation] | SX_BLK_CIPHER_MODEID_ECB;
   block_t dummy = NULL_blk;
   return sx_blk_cipher_build_descr(
         engine,
         config,
         key,
         &dummy, &dummy,
         datain,
         dataout,
         &dummy, &dummy, &dummy, &dummy, &dummy);
}

// CBC -CTR -OFB -CFB
// -----------------------------------------------------------------------------
uint32_t sx_blk_cipher_cbc_ctr_ofb_cfb_init(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      enum sx_blk_cipher_mode_select mode,
      const block_t *key,
      const block_t *datain,
      block_t *dataout,
      const block_t *iv,
      block_t *ctx_out)
{
   uint32_t config = operation_select[operation] | mode_select[mode]
         | SX_BLK_CIPHER_MODEID_CX_SAVE;

   if ((datain->len % 16) || !datain->len || (dataout->len != datain->len) ||
         (iv->len != BLK_CIPHER_IV_SIZE) || (ctx_out->len != BLK_CIPHER_CTX_SIZE))
      return CRYPTOLIB_INVALID_PARAM;

   block_t dummy = NULL_blk;
   return sx_blk_cipher_build_descr(
         engine,
         config,
         key,
         &dummy,
         iv,
         datain,
         dataout,
         &dummy, &dummy, &dummy, &dummy,
         ctx_out);
}

uint32_t sx_blk_cipher_cbc_ctr_ofb_cfb_update(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      enum sx_blk_cipher_mode_select mode,
      const block_t *key,
      const block_t *datain,
      block_t *dataout,
      const block_t *ctx_in,
      block_t *ctx_out)
{
   uint32_t config = operation_select[operation] | mode_select[mode]
         | SX_BLK_CIPHER_MODEID_CX_SAVE | SX_BLK_CIPHER_MODEID_CX_LOAD;

   if ((datain->len % 16) || !datain->len || (dataout->len != datain->len) ||
         (ctx_in->len != BLK_CIPHER_CTX_SIZE) || (ctx_out->len != BLK_CIPHER_CTX_SIZE))
      return CRYPTOLIB_INVALID_PARAM;

   block_t dummy = NULL_blk;
   return sx_blk_cipher_build_descr(
         engine,
         config,
         key,
         &dummy,
         ctx_in,
         datain,
         dataout,
         &dummy, &dummy, &dummy, &dummy,
         ctx_out);
}

// CBC
// -----------------------------------------------------------------------------
uint32_t sx_blk_cipher_cbc(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      const block_t *key,
      const block_t *datain,
      block_t *dataout,
      const block_t *iv)
{
   if (!is_len_valid_for_cbc_xts(engine, datain->len) ||
         (dataout->len != datain->len) || (iv->len != BLK_CIPHER_IV_SIZE))
      return CRYPTOLIB_INVALID_PARAM;

   uint32_t config = operation_select[operation] | SX_BLK_CIPHER_MODEID_CBC;
   block_t dummy = NULL_blk;
   return sx_blk_cipher_build_descr(
         engine,
         config,
         key,
         &dummy,
         iv,
         datain,
         dataout,
         &dummy, &dummy, &dummy, &dummy, &dummy);
}

uint32_t sx_blk_cipher_cbc_final(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      const block_t *key,
      const block_t *datain,
      block_t *dataout,
      const block_t *ctx_in)
{
   uint32_t config = operation_select[operation] | SX_BLK_CIPHER_MODEID_CBC |
         SX_BLK_CIPHER_MODEID_CX_LOAD;
   if (!is_len_valid_for_cbc_xts(engine, datain->len) || (dataout->len != datain->len) ||
         (ctx_in->len != BLK_CIPHER_CTX_SIZE))
      return CRYPTOLIB_INVALID_PARAM;

   block_t dummy = NULL_blk;
   return sx_blk_cipher_build_descr(
         engine,
         config,
         key,
         &dummy,
         ctx_in,
         datain,
         dataout,
         &dummy, &dummy, &dummy, &dummy, &dummy);
}

// CTR
// -----------------------------------------------------------------------------
uint32_t sx_blk_cipher_ctr(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      const block_t *key,
      const block_t *datain,
      block_t *dataout,
      const block_t *iv)
{
   uint32_t config = operation_select[operation] | SX_BLK_CIPHER_MODEID_CTR;

   if (!datain->len || dataout->len != datain->len || iv->len != BLK_CIPHER_IV_SIZE)
      return CRYPTOLIB_INVALID_PARAM;

   block_t dummy = NULL_blk;
   return sx_blk_cipher_build_descr(
         engine,
         config,
         key,
         &dummy,
         iv,
         datain,
         dataout,
         &dummy, &dummy, &dummy, &dummy, &dummy);
}


uint32_t sx_blk_cipher_ctr_final(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      const block_t *key,
      const block_t *datain,
      block_t *dataout,
      const block_t *ctx_in)
{
   uint32_t config = operation_select[operation] | SX_BLK_CIPHER_MODEID_CTR |
         SX_BLK_CIPHER_MODEID_CX_LOAD;

   if (! datain->len || (dataout->len != datain->len) ||
         (ctx_in->len != BLK_CIPHER_CTX_SIZE))
      return CRYPTOLIB_INVALID_PARAM;

   block_t dummy = NULL_blk;
   return sx_blk_cipher_build_descr(
         engine,
         config,
         key,
         &dummy,
         ctx_in,
         datain,
         dataout,
         &dummy, &dummy, &dummy, &dummy, &dummy);
}

// CFB-OFB
// -----------------------------------------------------------------------------
uint32_t sx_blk_cipher_cfb_ofb(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      enum sx_blk_cipher_mode_select mode,
      const block_t *key,
      const block_t *datain,
      block_t *dataout,
      const block_t *iv)
{
   if ((datain->len % 16) || !datain->len || (dataout->len != datain->len) ||
         (iv->len != BLK_CIPHER_IV_SIZE))
      return CRYPTOLIB_INVALID_PARAM;

   uint32_t config = operation_select[operation] | mode_select[mode];
   block_t dummy = NULL_blk;
   return sx_blk_cipher_build_descr(
         engine,
         config,
         key,
         &dummy,
         iv,
         datain,
         dataout,
         &dummy, &dummy, &dummy, &dummy, &dummy);
}

uint32_t sx_blk_cipher_cfb_ofb_final(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      enum sx_blk_cipher_mode_select mode,
      const block_t *key,
      const block_t *datain,
      block_t *dataout,
      const block_t *ctx_in)
{
   uint32_t config = operation_select[operation] | mode_select[mode]
         | SX_BLK_CIPHER_MODEID_CX_LOAD;

   if ((datain->len % 16) || !datain->len || (dataout->len != datain->len) ||
         (ctx_in->len != BLK_CIPHER_CTX_SIZE))
      return CRYPTOLIB_INVALID_PARAM;

   block_t dummy = NULL_blk;
   return sx_blk_cipher_build_descr(
         engine,
         config,
         key,
         &dummy,
         ctx_in,
         datain,
         dataout,
         &dummy, &dummy, &dummy, &dummy, &dummy);
}

// XTS
// -----------------------------------------------------------------------------
uint32_t sx_blk_cipher_xts(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      const block_t *key,
      const block_t *datain,
      block_t *dataout,
      const block_t *iv,
      const block_t *key_xts)
{
   uint32_t config = operation_select[operation] | SX_BLK_CIPHER_MODEID_XTS;

   if (!is_len_valid_for_cbc_xts(engine, datain->len) ||
         dataout->len != datain->len || (iv->len != BLK_CIPHER_IV_SIZE) ||
         (key->len != key_xts->len))
      return CRYPTOLIB_INVALID_PARAM;

   block_t dummy = NULL_blk;
   return sx_blk_cipher_build_descr(
         engine,
         config,
         key,
         key_xts,
         iv,
         datain,
         dataout,
         &dummy, &dummy, &dummy, &dummy, &dummy);
}

uint32_t sx_blk_cipher_xts_init(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      const block_t *key,
      const block_t *datain,
      block_t *dataout,
      const block_t *iv,
      block_t *ctx_out,
      const block_t *key_xts)
{
   uint32_t config = operation_select[operation] | SX_BLK_CIPHER_MODEID_XTS |
         SX_BLK_CIPHER_MODEID_CX_SAVE;

   if ((datain->len % 16) || !datain->len || (dataout->len != datain->len) ||
         (iv->len != BLK_CIPHER_IV_SIZE) || (ctx_out->len != BLK_CIPHER_CTX_SIZE) ||
         (key->len != key_xts->len))
      return CRYPTOLIB_INVALID_PARAM;

   block_t dummy = NULL_blk;
   return sx_blk_cipher_build_descr(
         engine,
         config,
         key,
         key_xts,
         iv,
         datain,
         dataout,
         &dummy, &dummy, &dummy, &dummy,
         ctx_out);
}

uint32_t sx_blk_cipher_xts_update(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      const block_t *key,
      const block_t *datain,
      block_t *dataout,
      const block_t *ctx_in,
      block_t *ctx_out,
      const block_t *key_xts)
{
   uint32_t config = operation_select[operation] | SX_BLK_CIPHER_MODEID_XTS |
         SX_BLK_CIPHER_MODEID_CX_SAVE | SX_BLK_CIPHER_MODEID_CX_LOAD;

   if ((datain->len % 16) || !datain->len || (dataout->len != datain->len) ||
         (ctx_in->len != BLK_CIPHER_CTX_SIZE) || (ctx_out->len != BLK_CIPHER_CTX_SIZE) ||
         (key->len != key_xts->len))
      return CRYPTOLIB_INVALID_PARAM;

   block_t dummy = NULL_blk;
   return sx_blk_cipher_build_descr(
         engine,
         config,
         key,
         key_xts,
         ctx_in,
         datain,
         dataout,
         &dummy, &dummy, &dummy, &dummy,
         ctx_out);
}

uint32_t sx_blk_cipher_xts_final(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      const block_t *key,
      const block_t *datain,
      block_t *dataout,
      const block_t *ctx_in,
      const block_t *key_xts)
{
    uint32_t config = operation_select[operation] | SX_BLK_CIPHER_MODEID_XTS |
          SX_BLK_CIPHER_MODEID_CX_LOAD;

   if (!is_len_valid_for_cbc_xts(engine, datain->len) || (dataout->len != datain->len) ||
         (ctx_in->len != BLK_CIPHER_CTX_SIZE) || (key->len != key_xts->len))
      return CRYPTOLIB_INVALID_PARAM;

   block_t dummy = NULL_blk;
   return sx_blk_cipher_build_descr(
         engine,
         config,
         key,
         key_xts,
         ctx_in,
         datain,
         dataout,
         &dummy, &dummy, &dummy, &dummy, &dummy);
}

// GCM
// -----------------------------------------------------------------------------
uint32_t sx_blk_cipher_gcm(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      const block_t *key,
      const block_t *datain,
      block_t *dataout,
      const block_t *iv,
      block_t *mac,
      const block_t *aad)
{
   if ((dataout->len != datain->len) || (iv->len != BLK_CIPHER_IV_GCM_SIZE) ||
         (mac->len != BLK_CIPHER_MAC_SIZE))
      return CRYPTOLIB_INVALID_PARAM;

   uint32_t config = operation_select[operation] | SX_BLK_CIPHER_MODEID_GCM;

   // Compute len(AAD) + len(data)
   uint8_t len_a_c_buff[16];
   block_t len_a_c = BLK_LITARRAY(len_a_c_buff);
   sx_math_u64_to_u8array(aad->len << 3, &len_a_c_buff[0], sx_big_endian);
   sx_math_u64_to_u8array(datain->len << 3, &len_a_c_buff[8], sx_big_endian);

   block_t dummy = NULL_blk;
   return sx_blk_cipher_build_descr(
         engine,
         config,
         key,
         &dummy,
         iv,
         datain,
         dataout,
         &dummy,
         aad,
         &len_a_c,
         mac,
         &dummy);
}

uint32_t sx_blk_cipher_gcm_decrypt_verify(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *iv,
      const block_t *mac,
      const block_t *aad)
{
   if (mac->len != BLK_CIPHER_MAC_SIZE)
      return CRYPTOLIB_INVALID_PARAM;

   uint8_t mac_recomputed_buf[BLK_CIPHER_MAC_SIZE] = {0};
   block_t mac_recomputed = block_t_convert(
         mac_recomputed_buf, sizeof(mac_recomputed_buf));
   uint32_t status = sx_blk_cipher_gcm(engine, operation, key, ciphertext,
         plaintext, iv, &mac_recomputed, aad);
   if (status)
      return status;

   if (memcmp_time_cst(mac->addr, mac_recomputed_buf, mac->len))
      return CRYPTOLIB_INVALID_SIGN_ERR;
   return CRYPTOLIB_SUCCESS;
}

uint32_t sx_blk_cipher_gcm_init(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      const block_t *key,
      const block_t *datain,
      block_t *dataout,
      const block_t *iv,
      block_t *ctx_out,
      const block_t *aad)
{
    uint32_t config = operation_select[operation] | SX_BLK_CIPHER_MODEID_GCM |
          SX_BLK_CIPHER_MODEID_CX_SAVE;

   if ((dataout->len != datain->len) || (iv->len != BLK_CIPHER_IV_GCM_SIZE) ||
         (ctx_out->len != BLK_CIPHER_CTX_xCM_SIZE))
      return CRYPTOLIB_INVALID_PARAM;

   block_t dummy = NULL_blk;
   return sx_blk_cipher_build_descr(
         engine,
         config,
         key,
         &dummy,
         iv,
         datain,
         dataout,
         &dummy,
         aad,
         &dummy, &dummy,
         ctx_out);
}

uint32_t sx_blk_cipher_gcm_update(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      const block_t *key,
      const block_t *datain,
      block_t *dataout,
      const block_t *ctx_in,
      block_t *ctx_out)
{
    uint32_t config = operation_select[operation] | SX_BLK_CIPHER_MODEID_GCM |
          SX_BLK_CIPHER_MODEID_CX_SAVE | SX_BLK_CIPHER_MODEID_CX_LOAD;

   if ((dataout->len != datain->len) || (ctx_in->len != BLK_CIPHER_CTX_xCM_SIZE) ||
         (ctx_out->len != BLK_CIPHER_CTX_xCM_SIZE))
      return CRYPTOLIB_INVALID_PARAM;

   block_t dummy = NULL_blk;
   return sx_blk_cipher_build_descr(
         engine,
         config,
         key,
         &dummy,
         ctx_in,
         datain,
         dataout,
         &dummy, &dummy, &dummy, &dummy,
         ctx_out);
}

uint32_t sx_blk_cipher_gcm_final(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      const block_t *key,
      const block_t *datain,
      block_t *dataout,
      const block_t *ctx_in,
      block_t *mac,
      const block_t *len_a_c)
{
    uint32_t config = operation_select[operation] | SX_BLK_CIPHER_MODEID_GCM |
          SX_BLK_CIPHER_MODEID_CX_LOAD;

   if ((dataout->len != datain->len) || (ctx_in->len != BLK_CIPHER_CTX_xCM_SIZE) ||
         (mac->len != BLK_CIPHER_MAC_SIZE) || (len_a_c->len != 16)) // TODO Use nice names
      return CRYPTOLIB_INVALID_PARAM;

   block_t dummy = NULL_blk;
   return sx_blk_cipher_build_descr(
         engine,
         config,
         key,
         &dummy,
         ctx_in,
         datain,
         dataout,
         &dummy, &dummy,
         len_a_c,
         mac,
         &dummy);
}

uint32_t sx_blk_cipher_gcm_decrypt_verify_final(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,

      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *ctx_in,
      const block_t *mac,
      const block_t *len_a_c)
{
   if (mac->len != BLK_CIPHER_MAC_SIZE)
      return CRYPTOLIB_INVALID_PARAM;

   uint8_t mac_recomputed_buf[BLK_CIPHER_MAC_SIZE] = {0};
   block_t mac_recomputed = block_t_convert(
         mac_recomputed_buf, sizeof(mac_recomputed_buf));

   uint32_t status = sx_blk_cipher_gcm_final(
         engine,
         operation,
         key,
         ciphertext,
         plaintext,
         ctx_in,
         &mac_recomputed,
         len_a_c);
   if (status)
      return status;

   if (memcmp_time_cst(mac->addr, mac_recomputed_buf, mac->len))
      return CRYPTOLIB_INVALID_SIGN_ERR;
   return CRYPTOLIB_SUCCESS;
}

// CCM
// -----------------------------------------------------------------------------
uint32_t sx_blk_cipher_ccm_encrypt(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *nonce,
      block_t *mac,
      const block_t *aad)
{
   if (plaintext->len != ciphertext->len)
      return CRYPTOLIB_INVALID_PARAM;

   uint8_t header_generated[22]; // Maximum header size (excluding aad which will be transferred directly)
   block_t header = BLK_LITARRAY(header_generated);
   uint32_t status = generate_ccm_header(*nonce, aad->len, plaintext->len, mac->len, &header);
   if (status)
      return status;

   uint32_t config = operation_select[operation] | SX_BLK_CIPHER_MODEID_CCM;
   block_t dummy = NULL_blk;
   return sx_blk_cipher_build_descr(
         engine,
         config,
         key,
         &dummy, &dummy,
         plaintext,
         ciphertext,
         &header,
         aad,
         &dummy,
         mac,
         &dummy);
}

uint32_t sx_blk_cipher_ccm_decrypt(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *nonce,
      block_t *mac,
      const block_t *aad)
{
   if (ciphertext->len != plaintext->len)
      return CRYPTOLIB_INVALID_PARAM;

   uint8_t header_generated[22]; // Maximum header size (excluding aad2 which will be transferred directly)
   block_t header = BLK_LITARRAY(header_generated);
   uint32_t status = generate_ccm_header(*nonce, aad->len, ciphertext->len,
         mac->len, &header);
   if (status)
      return status;

   uint32_t config = operation_select[operation] | SX_BLK_CIPHER_MODEID_CCM;
   block_t dummy = NULL_blk;

   /* CCM is implemented in a different way that GCM; it is not possible to
    * directly read back the recomputed MAC from the BA411E.
    * The reference MAC is xor-ed internally with the recomputed one.
    * Sending 0,...,0 in place of the reference will return the recomputed MAC
    * as A xor 0 = A.
    */
   uint8_t zeroes_buf[BLK_CIPHER_MAC_SIZE] = {0};
   block_t zeroes = block_t_convert(zeroes_buf, mac->len);
   return sx_blk_cipher_build_descr(
         engine,
         config,
         key,
         &dummy, &dummy,
         ciphertext,
         plaintext,
         &header,
         aad,
         &zeroes,
         mac,
         &dummy);
}

uint32_t sx_blk_cipher_ccm_decrypt_verify(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,

      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *nonce,
      const block_t *mac,
      const block_t *aad)
{
   uint8_t mac_buf[BLK_CIPHER_MAC_SIZE];
   block_t mac_recomputed = block_t_convert(mac_buf, mac->len);
   uint32_t status = sx_blk_cipher_ccm_decrypt(
         engine,
         operation,
         key,
         ciphertext,
         plaintext,
         nonce,
         &mac_recomputed,
         aad);
   if (status)
      return status;

   if (memcmp_time_cst(mac->addr, mac_buf, mac->len))
      return CRYPTOLIB_INVALID_SIGN_ERR;
   return CRYPTOLIB_SUCCESS;
}

uint32_t sx_blk_cipher_ccm_init(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      const block_t *key,
      const block_t *datain,
      block_t *dataout,
      const block_t *nonce,
      block_t *ctx_out,
      const block_t *aad,
      uint32_t mac_len)
{
    uint32_t config = operation_select[operation] | SX_BLK_CIPHER_MODEID_CCM |
          SX_BLK_CIPHER_MODEID_CX_SAVE;

   if ((datain->len != dataout->len) || (ctx_out->len != BLK_CIPHER_CTX_xCM_SIZE))
      return CRYPTOLIB_INVALID_PARAM;

   uint8_t header_generated[22]; // Maximum header size (excluding aad which will be transferred directly)
   block_t header = BLK_LITARRAY(header_generated);
   uint32_t status = generate_ccm_header(*nonce, aad->len, datain->len, mac_len, &header);
   if (status)
      return status;

   block_t dummy = NULL_blk;
   return sx_blk_cipher_build_descr(
         engine,
         config,
         key,
         &dummy, &dummy,
         datain,
         dataout,
         &header,
         aad,
         &dummy, &dummy,
         ctx_out);
}

uint32_t sx_blk_cipher_ccm_update(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      const block_t *key,
      const block_t *datain,
      block_t *dataout,
      const block_t *ctx_in,
      block_t *ctx_out)
{
    uint32_t config = operation_select[operation] | SX_BLK_CIPHER_MODEID_CCM |
          SX_BLK_CIPHER_MODEID_CX_SAVE | SX_BLK_CIPHER_MODEID_CX_LOAD;

   if ((datain->len != dataout->len) || (ctx_in->len != BLK_CIPHER_CTX_xCM_SIZE) ||
         (ctx_out->len != BLK_CIPHER_CTX_xCM_SIZE))
      return CRYPTOLIB_INVALID_PARAM;

   block_t dummy = NULL_blk;
   return sx_blk_cipher_build_descr(
         engine,
         config,
         key,
         &dummy,
         ctx_in,
         datain,
         dataout,
         &dummy, &dummy, &dummy, &dummy,
         ctx_out);
}

uint32_t sx_blk_cipher_ccm_encrypt_final(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      const block_t *key,
      const block_t *plaintext,
      block_t *ciphertext,
      const block_t *ctx_in,
      block_t *mac)
{
    uint32_t config = operation_select[operation] | SX_BLK_CIPHER_MODEID_CCM |
          SX_BLK_CIPHER_MODEID_CX_LOAD;

   if ((plaintext->len != ciphertext->len) ||
         (ctx_in->len != BLK_CIPHER_CTX_xCM_SIZE))
      return CRYPTOLIB_INVALID_PARAM;

   /* Variant of CCM with disabled authenticity is supported: CCM*
    * CCM* MAC length is one of {0,4,6,8,10,12,14,16}, see
    * https://tools.ietf.org/html/rfc3610 for CCM and
    * B.4 Specification of generic CCM* mode of operation from
    * http://ecee.colorado.edu/~liue/teaching/comm_standards/2015S_zigbee/802.15.4-2011.pdf
    * for CCM*
    */
   if ((mac->len % 2 != 0) || (mac->len == 2) || mac->len > BLK_CIPHER_MAC_SIZE)
      return CRYPTOLIB_INVALID_PARAM;

   block_t dummy = NULL_blk;
   return sx_blk_cipher_build_descr(
         engine,
         config,
         key,
         &dummy,
         ctx_in,
         plaintext,
         ciphertext,
         &dummy, &dummy, &dummy,
         mac,
         &dummy);
}

uint32_t sx_blk_cipher_ccm_decrypt_final(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *ctx_in,
      block_t *mac)
{
    uint32_t config = operation_select[operation] | SX_BLK_CIPHER_MODEID_CCM |
          SX_BLK_CIPHER_MODEID_CX_LOAD;

   if ((plaintext->len != ciphertext->len) ||
         (ctx_in->len != BLK_CIPHER_CTX_xCM_SIZE))
      return CRYPTOLIB_INVALID_PARAM;

   /* Variant of CCM with disabled authenticity is supported: CCM*
    * CCM* MAC length is one of {0,4,6,8,10,12,14,16}, see
    * https://tools.ietf.org/html/rfc3610 for CCM and
    * B.4 Specification of generic CCM* mode of operation from
    * http://ecee.colorado.edu/~liue/teaching/comm_standards/2015S_zigbee/802.15.4-2011.pdf
    * for CCM*
    */
   if ((mac->len % 2 != 0) || (mac->len == 2) || mac->len > BLK_CIPHER_MAC_SIZE)
      return CRYPTOLIB_INVALID_PARAM;

   /* CCM is implemented in a different way that GCM; it is not possible to
    * directly read back the recomputed MAC from the BA411E.
    * The reference MAC is xor-ed internally with the recomputed one.
    * Sending 0,...,0 in place of the reference will return the recomputed MAC
    * as A xor 0 = A.
    */
   uint8_t zeroes_buf[BLK_CIPHER_MAC_SIZE] = {0};
   block_t zeroes = block_t_convert(zeroes_buf, mac->len);

   block_t dummy = NULL_blk;
   return sx_blk_cipher_build_descr(
         engine,
         config,
         key,
         &dummy,
         ctx_in,
         ciphertext,
         plaintext,
         &dummy, &dummy,
         &zeroes,
         mac,
         &dummy);
}

uint32_t sx_blk_cipher_ccm_decrypt_verify_final(
      enum sx_blk_cipher_engine_select engine,
      enum sx_blk_cipher_operation operation,
      const block_t *key,
      const block_t *ciphertext,
      block_t *plaintext,
      const block_t *ctx_in,
      const block_t *mac)
{
   uint8_t mac_buf[BLK_CIPHER_MAC_SIZE];
   block_t mac_recomputed = block_t_convert(mac_buf, mac->len);
   uint32_t status = sx_blk_cipher_ccm_decrypt_final(
         engine,
         operation,
         key,
         ciphertext,
         plaintext,
         ctx_in,
         &mac_recomputed);
   if (status)
      return status;

   if (memcmp_time_cst(mac->addr, mac_buf, mac->len))
      return CRYPTOLIB_INVALID_SIGN_ERR;
   return CRYPTOLIB_SUCCESS;
}

// CMAC
// -----------------------------------------------------------------------------
// Whole
uint32_t sx_blk_cipher_cmac_generate(
      enum sx_blk_cipher_engine_select engine,
      const block_t *key,
      const block_t *message,
      block_t *mac)
{
   if (mac->len != BLK_CIPHER_MAC_SIZE)
      return CRYPTOLIB_INVALID_PARAM;

   uint32_t config = SX_BLK_CIPHER_MODEID_CMA | SX_BLK_CIPHER_MODEID_ENCRYPT;
   block_t dummy = NULL_blk;
   return sx_blk_cipher_build_descr(
         engine,
         config,
         key,
         &dummy, &dummy,
         message,
         &dummy, &dummy, &dummy, &dummy,
         mac,
         &dummy);
}

uint32_t sx_blk_cipher_cmac_verify(
      enum sx_blk_cipher_engine_select engine,
      const block_t *key,
      const block_t *message,
      const block_t *mac)
{
   if (mac->len != BLK_CIPHER_MAC_SIZE)
      return CRYPTOLIB_INVALID_PARAM;

   uint8_t recomputed_buf[BLK_CIPHER_MAC_SIZE];
   block_t recomputed = BLK_LITARRAY(recomputed_buf);

   uint32_t status = sx_blk_cipher_cmac_generate(engine, key, message,
         &recomputed);
   if (status)
      return status;

   if (memcmp_time_cst(recomputed_buf, mac->addr, mac->len))
      return CRYPTOLIB_INVALID_SIGN_ERR;
   return CRYPTOLIB_SUCCESS;
}

// Init
uint32_t sx_blk_cipher_cmac_generate_init(
      enum sx_blk_cipher_engine_select engine,
      const block_t *key,
      const block_t *message,
      block_t *ctx_out)
{
    uint32_t config = SX_BLK_CIPHER_MODEID_CMA | SX_BLK_CIPHER_MODEID_ENCRYPT |
          SX_BLK_CIPHER_MODEID_CX_SAVE;

   if (ctx_out->len != BLK_CIPHER_CTX_SIZE)
      return CRYPTOLIB_INVALID_PARAM;

   block_t dummy = NULL_blk;
   return sx_blk_cipher_build_descr(
         engine,
         config,
         key,
         &dummy, &dummy,
         message,
         &dummy, &dummy, &dummy, &dummy, &dummy,
         ctx_out);
}

// Update
uint32_t sx_blk_cipher_cmac_generate_update(
      enum sx_blk_cipher_engine_select engine,
      const block_t *key,
      const block_t *message,
      const block_t *ctx_in,
      block_t *ctx_out)
{
    uint32_t config = SX_BLK_CIPHER_MODEID_CMA | SX_BLK_CIPHER_MODEID_ENCRYPT |
          SX_BLK_CIPHER_MODEID_CX_SAVE | SX_BLK_CIPHER_MODEID_CX_LOAD;

   if ((ctx_in->len != BLK_CIPHER_CTX_SIZE) ||
         (ctx_out->len != BLK_CIPHER_CTX_SIZE))
      return CRYPTOLIB_INVALID_PARAM;

   block_t dummy = NULL_blk;
   return sx_blk_cipher_build_descr(
         engine,
         config,
         key,
         &dummy,
         ctx_in,
         message,
         &dummy, &dummy, &dummy, &dummy, &dummy,
         ctx_out);
}

// Final
uint32_t sx_blk_cipher_cmac_generate_final(
      enum sx_blk_cipher_engine_select engine,
      const block_t *key,
      const block_t *message,
      const block_t *ctx_in,
      block_t *mac)
{
    uint32_t config = SX_BLK_CIPHER_MODEID_CMA | SX_BLK_CIPHER_MODEID_ENCRYPT |
          SX_BLK_CIPHER_MODEID_CX_LOAD;

   if ((ctx_in->len != BLK_CIPHER_CTX_SIZE) || (mac->len != BLK_CIPHER_MAC_SIZE))
      return CRYPTOLIB_INVALID_PARAM;

   block_t dummy = NULL_blk;
   return sx_blk_cipher_build_descr(
         engine,
         config,
         key,
         &dummy,
         ctx_in,
         message,
         &dummy, &dummy, &dummy, &dummy,
         mac,
         &dummy);
}

uint32_t sx_blk_cipher_cmac_generate_verify_final(
      enum sx_blk_cipher_engine_select engine,
      const block_t *key,
      const block_t *message,
      const block_t *ctx_in,
      const block_t *mac)
{
   if (mac->len != BLK_CIPHER_MAC_SIZE)
      return CRYPTOLIB_INVALID_PARAM;

   uint8_t recomputed_buf[BLK_CIPHER_MAC_SIZE];
   block_t recomputed = BLK_LITARRAY(recomputed_buf);

   uint32_t status = sx_blk_cipher_cmac_generate_final(engine, key, message,
         ctx_in, &recomputed);
   if (status)
      return status;

   if (memcmp_time_cst(recomputed_buf, mac->addr, mac->len))
      return CRYPTOLIB_INVALID_SIGN_ERR;
   return CRYPTOLIB_SUCCESS;
}

/**
 * @brief block cipher generate CCM header
 *    Create the header of CCM data based on lengths, nonce & aad2
 *
 * @param nonce      The Nonce
 * @param aad_len    The length of additional data to authenticate
 * @param data_len   The length of the data (plaintext or cipher) in bytes
 * @param tag_len    The length of the MAC in bytes
 * @param header     Pointer to the output header block
 * @return CRYPTOLIB_SUCCESS if execution was successful
 */
uint32_t generate_ccm_header(block_t nonce, uint32_t aad_len, uint32_t data_len,
      uint32_t tag_len, block_t *header)
{
   uint32_t flags;
   uint32_t m, l;
   uint32_t i;

   /* RFC3610 paragraph 2.2 defines the formatting of the first block.
    * Thee first block contains:
    *  byte  [0]           the flags byte (see below)
    *  bytes [1,1+nonce.len]   nonce
    *  bytes [2+nonce.len, 16] message length
    *
    *  The flags byte has the following bit fields:
    *    [6:7] = 0
    *    [3:5] = authentication tag size, encoded as (tag_len-2)/2
    *              only multiples of 2 between 2 and 16 are allowed.
    *    [0:2] = length field size minus 1. Is the same as (15 - nonce.len - 1)
    *         between 2 and 8.
    **/

   /* Verify input parameters
    *
    * Variant of CCM with disabled authenticity is supported: CCM*
    * CCM* MAC length is one of {0,4,6,8,10,12,14,16}, see
    * https://tools.ietf.org/html/rfc3610 for CCM and
    * B.4 Specification of generic CCM* mode of operation from
    * http://ecee.colorado.edu/~liue/teaching/comm_standards/2015S_zigbee/802.15.4-2011.pdf
    * for CCM*
    */
   if ((tag_len & 1) || (tag_len == 2) || (tag_len > BLK_CIPHER_MAC_SIZE))
      return CRYPTOLIB_INVALID_PARAM;

   /* The conditions below translate into (l > 8) || (l < 2) which match the
    * specification "The parameter L can take on the values from 2 to 8"
    */
   if ((nonce.len < 7) || (nonce.len > 13))
      return CRYPTOLIB_INVALID_PARAM;

   flags = (aad_len > 0) ? (1 << 6) : 0;
   /* Authentication tag size encoding. A tag_len = 0 is accepted by CCM* and
    * that will be encoded as a 0. */
   m = (tag_len > 0) ? (tag_len-2)/2 : 0;
   flags |= (m & 0x7) << 3;
   l = 15 - nonce.len;
   flags |= ((l-1) & 0x7);
   header->addr[0] = flags;

   /* If l >= 4, data_len will always fit as it is an uint32_t, we need to
    * check only in case l < 4: */
   if (l < 4U && data_len >= (1U << (l * 8))) {
      /* message too long to encode the size in the CCM header */
      return CRYPTOLIB_INVALID_PARAM;
   }

   memcpy_blkIn(&header->addr[1], nonce, nonce.len);

   /* append message length in big endian format encoded l octets*/
   memset(&(header->addr[1 + nonce.len]), 0, l);
   for (i = 0; i < sizeof(data_len); i++) {
      if (data_len >= (1U << (i * 8)))
         header->addr[15 - i] = (data_len >> (i * 8)) & 0xff;
      else
         break;
   }

   /* if there's additional authentication data (or aad2),
    * encode the size:
    *
    * 0 < aad_len < 0xFF00     => 2 bytes in big endian format.
    * 0xFF00 < aad_len < 2^32  => 0xff, 0xfe, and four bytes in big endian format.
    * eSecure currently does not support sizes bigger than 2^32.
    */
   if (aad_len > 0) {
      if (aad_len < 0xFF00) {
         header->addr[16] = aad_len >> 8;
         header->addr[17] = aad_len & 0xff;
         header->len = 18;
      } else {
         // TODO: Test following case
         header->addr[16] = 0xFF;
         header->addr[17] = 0xFE;
         header->addr[18] = aad_len >> 24;
         header->addr[19] = (aad_len >> 16) & 0xff;
         header->addr[20] = (aad_len >> 8) & 0xff;
         header->addr[21] = aad_len & 0xff;
         header->len = 22;
      }
   } else {
      header->len = 16;
   }

   return CRYPTOLIB_SUCCESS;
}

/**
 * @brief Get block cipher padding length (realign on block cipher block size)
 * @param input_length input length
 * @return length of padding
 */
uint32_t get_pad_len(uint32_t input_length)
{
   return (16-input_length)&15;
}

/**
 * @brief Build descriptors and call cryptoDMA for block cipher operation
 * @param engine selected engine to be used as block cipher
 * @param config value for cfg mode register
 * @param key block cipher key
 * @param xtskey XTS key.
 * @param iv initialization vector
 * @param datain input data (plaintext or ciphertext)
 * @param dataout output data (ciphertext or plaintext)
 * @param aad1 additional authenticated data part #1
 * @param aad2 additional authenticated data part #2
 * @param extrain additional input data expected by ::GCM or ::CCM
 * @param tag_out authentication tag input for ::CCM, ::GCM & ::CMAC
 * @param ctx_ptr block cipher context output
 * @return CRYPTOLIB_SUCCESS when execution was successful
 */
static uint32_t sx_blk_cipher_build_descr(
      enum sx_blk_cipher_engine_select engine,
      uint32_t hw_config,
      const block_t *key,
      const block_t *xtskey,
      const block_t *iv,
      const block_t *datain,
      block_t *dataout,
      const block_t *aad1,
      const block_t *aad2,
      const block_t *extrain,
      block_t *tag_out,
      block_t *ctx_ptr)
{
   uint32_t status;
   block_t config = block_t_convert(&hw_config, sizeof(hw_config));
   block_t keyb = *key;

   switch (engine) {
   case SX_BLK_CIPHER_AES:
      status = sx_aes_set_hw_config_for_key(&keyb, &hw_config);
      break;
   case SX_BLK_CIPHER_SM4:
      status = sx_sm4_set_hw_config_for_key(&keyb, &hw_config);
      break;
   case SX_BLK_CIPHER_ARIA:
      status = sx_aria_set_hw_config_for_key(&keyb, &hw_config);
      break;
   default:
      return CRYPTOLIB_INVALID_PARAM;
   }
   if (status)
      return status;

   struct dma_sg_descr_s desc_to[DESC_N_TO_ENGINE];  //could be reduces as no use case where all inputs are used, but safer like this
   struct dma_sg_descr_s desc_fr[DESC_N_FROM_ENGINE];
   struct dma_sg_descr_s *d;  // pointer to current descriptor
   block_t datainb = *datain;
   // input padding
   uint32_t aad_zeropad_len      = get_pad_len(aad1->len + aad2->len);
   uint32_t datain_zeropad_len   = get_pad_len(datain->len);
   uint32_t extrain_zeropad_len    = get_pad_len(extrain->len);

   // handle alignment for data out
   if(dataout->flags & DMA_AXI_DESCR_CONST_ADDR)
      dataout->len = roundup_32(dataout->len);

   if(tag_out->flags & DMA_AXI_DESCR_CONST_ADDR)
      tag_out->len = roundup_32(tag_out->len);

   // output discards
   block_t aads_discard      = block_t_convert(NULL, aad1->len + aad2->len + aad_zeropad_len);
   block_t dataout_discard   = block_t_convert(NULL, get_pad_len(dataout->len));
   block_t tagout_discard    = block_t_convert(NULL, get_pad_len(tag_out->len));

   // no input provided, -> empty input = 1 block of zero padding (i.e. for CMAC)
   uint8_t zeroes[16] = {0};
   if (!datain->len && !extrain->len && !aad1->len && !aad2->len ) {
      datain_zeropad_len = 16;
      datainb = block_t_convert(zeroes, datain_zeropad_len);
   }

   // fetcher descriptors
   d = desc_to;
   // Config
   d = write_desc_blk(
         d,
         &config,
         DMA_AXI_DESCR_REALIGN,
         engine_select[engine] | DMA_SG_TAG_ISCONFIG |
         DMA_SG_TAG_SETCFGOFFSET(SX_BLK_CIPHER_OFFSET_CFG));

   // Symmetric key
   d = write_desc_blk(
         d,
         &keyb,
         DMA_AXI_DESCR_REALIGN,
         engine_select[engine] | DMA_SG_TAG_ISCONFIG |
         DMA_SG_TAG_SETCFGOFFSET(SX_BLK_CIPHER_OFFSET_KEY));

   // IV or context (if existing)
   d = write_desc_blk(
         d,
         iv,
         DMA_AXI_DESCR_REALIGN,
         engine_select[engine] | DMA_SG_TAG_ISCONFIG |
         DMA_SG_TAG_SETCFGOFFSET(SX_BLK_CIPHER_OFFSET_IV));

   // XTS key (if existing)
   d = write_desc_blk(
         d,
         xtskey,
         DMA_AXI_DESCR_REALIGN,
         engine_select[engine] | DMA_SG_TAG_ISCONFIG |
         DMA_SG_TAG_SETCFGOFFSET(SX_BLK_CIPHER_OFFSET_KEY2));

   // authentication data (if existing)
   d = write_desc_blk(
         d,
         aad1,
         0,
         engine_select[engine] | DMA_SG_TAG_ISDATA |
         DMA_SG_TAG_DATATYPE_BLK_CIPHER_HEADER);
   d = write_desc_blk(
         d,
         aad2,
         DMA_AXI_DESCR_REALIGN,
       engine_select[engine] | DMA_SG_TAG_ISDATA |
         DMA_SG_TAG_DATATYPE_BLK_CIPHER_HEADER |
         DMA_SG_TAG_SETINVALIDBYTES(aad_zeropad_len));

   // Input data (if existing)
   d = write_desc_blk(
         d,
         &datainb,
         DMA_AXI_DESCR_REALIGN,
         engine_select[engine] | DMA_SG_TAG_ISDATA |
         DMA_SG_TAG_DATATYPE_BLK_CIPHER_PAYLOAD |
         DMA_SG_TAG_SETINVALIDBYTES(datain_zeropad_len));

   // Additional input data (may be tag_in for CCM or len_a_c for GCM)
   d = write_desc_blk(
         d,
         extrain,
         DMA_AXI_DESCR_REALIGN,
         engine_select[engine] | DMA_SG_TAG_ISDATA |
         DMA_SG_TAG_DATATYPE_BLK_CIPHER_PAYLOAD |
         DMA_SG_TAG_SETINVALIDBYTES(extrain_zeropad_len));

   set_last_desc(d - 1);

   // pusher descriptors
   d = desc_fr;

   // discard output aad and associated padding
   d = write_desc_blk(d, &aads_discard, 0, 0);

   // Output data
   d = write_desc_blk(d, dataout, 0, 0);
   d = write_desc_blk(d, &dataout_discard, 0, 0);

   // Output tag (if existing)
   d = write_desc_blk(d, tag_out, 0, 0);
   d = write_desc_blk(d, &tagout_discard, 0, 0);

   // Output context (if existing)
   d = write_desc_blk(d, ctx_ptr, 0, 0);

   set_last_desc(d - 1);

   // launch cryptodma
   cryptodma_run_sg(desc_to, desc_fr);

   return CRYPTOLIB_SUCCESS;
}
