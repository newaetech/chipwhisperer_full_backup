/**
 * @file
 * @brief Implements the procedures to make ECDSA operations with
 *          the BA414EP pub key
 * @copyright Copyright (c) 2016-2018 Silex Insight. All Rights reserved
 */


#include "sx_ecdsa_alg.h"
#include "sx_memcpy.h"
#include "ba414ep_config.h"
#include "sx_hash.h"
#include "sx_rng.h"
#include "sx_errors.h"
#include "sx_math.h"

uint32_t ecdsa_validate_domain(const sx_ecc_curve_t *curve)
{
   const uint32_t size = sx_ecc_curve_bytesize(curve);
   if (size > ECC_MAX_KEY_SIZE)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   if (curve->params.len != 5 * size && curve->params.len != 6 * size)
      return CRYPTOLIB_INVALID_PARAM;

   uint32_t status;
   ba414ep_set_command(BA414EP_OPTYPE_ECDSA_PARAM_EVAL, size, BA414EP_BIGEND, curve->pk_flags);
   ba414ep_load_curve(curve->params, size, BA414EP_BIGEND, 1);
   status = ba414ep_start_wait_status();
   if (status)
      return CRYPTOLIB_CRYPTO_ERR;

   return CRYPTOLIB_SUCCESS;
}

uint32_t sx_ecdsa_configure_signature(const sx_ecc_curve_t *curve,
                                      block_t formatted_digest,
                                      block_t key)
{
   const uint32_t size = sx_ecc_curve_bytesize(curve);
   if (size > ECC_MAX_KEY_SIZE)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   if ((curve->params.len != 5 * size && curve->params.len != 6 * size) ||
         key.len != size)
      return CRYPTOLIB_INVALID_PARAM;

   // Set command to enable byte-swap
   ba414ep_set_command(BA414EP_OPTYPE_ECDSA_SIGN_GEN, size, BA414EP_BIGEND, curve->pk_flags);

   // Load parameters
   ba414ep_load_curve(curve->params, size, BA414EP_BIGEND, 1);

   /* Load ECDSA parameters */
   mem2CryptoRAM_rev(key, size, BA414EP_MEMLOC_6);
   mem2CryptoRAM_rev(formatted_digest, size, BA414EP_MEMLOC_12);

   return CRYPTOLIB_SUCCESS;
}

uint32_t sx_ecdsa_attempt_signature(
      const sx_ecc_curve_t *curve,
      block_t signature,
      struct sx_rng rng)
{
   const uint32_t size = sx_ecc_curve_bytesize(curve);
   if (size > ECC_MAX_KEY_SIZE)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   if ((curve->params.len != 5 * size && curve->params.len != 6 * size) ||
         signature.len != 2 * size)
      return CRYPTOLIB_INVALID_PARAM;

   uint8_t  rnd_buff[ECC_MAX_KEY_SIZE];
   block_t rnd = block_t_convert(rnd_buff, size);
   block_t curve_order = block_t_convert(curve->params.addr + size, size);
   uint32_t status = sx_rng_get_rand_lt_n_blk(rnd, curve_order, rng);
   if (status)
      return status;
   mem2CryptoRAM_rev(rnd, rnd.len, BA414EP_MEMLOC_7);

   /* ECDSA signature generation */
   ba414ep_set_command(
         BA414EP_OPTYPE_ECDSA_SIGN_GEN,
         size,
         BA414EP_BIGEND,
         curve->pk_flags);
   status = ba414ep_start_wait_status();

   if (status & BA414EP_STS_BADSIGNATURE_MASK)
      return CRYPTOLIB_INVALID_SIGN_ERR;
   else if (status)
      return CRYPTOLIB_CRYPTO_ERR;

   // Fetch the results
   CryptoRAM2point_rev(signature, size, BA414EP_MEMLOC_10);
   return CRYPTOLIB_SUCCESS;
}

uint32_t ecdsa_generate_signature_digest(const sx_ecc_curve_t *curve,
                                         block_t formatted_digest,
                                         block_t key,
                                         block_t signature,
                                         struct sx_rng rng)
{
   uint32_t ctr = 0;
   uint32_t status = sx_ecdsa_configure_signature(curve, formatted_digest, key);
   if (status != CRYPTOLIB_SUCCESS)
      return status;

   do {
      status = sx_ecdsa_attempt_signature(curve, signature, rng);
   } while((status == CRYPTOLIB_INVALID_SIGN_ERR) && (ctr < 10));
   if (status)
      return status;

   return CRYPTOLIB_SUCCESS;
}




uint32_t ecdsa_verify_signature_digest(const sx_ecc_curve_t *curve,
                                       block_t formatted_digest,
                                       block_t key,
                                       block_t signature)
{
   const uint32_t size = sx_ecc_curve_bytesize(curve);
   if (size > ECC_MAX_KEY_SIZE)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   if ((curve->params.len != 5 * size && curve->params.len != 6 * size) ||
         key.len != 2 * size ||
         signature.len != 2 * size)
      return CRYPTOLIB_INVALID_PARAM;

   ba414ep_set_command(BA414EP_OPTYPE_ECDSA_SIGN_VERIF, size, BA414EP_BIGEND, curve->pk_flags);
   ba414ep_load_curve(curve->params, size, 1, 1); //TODO FIX ENDIAN

   /* Load ECDSA parameters */
   point2CryptoRAM_rev(key, size, BA414EP_MEMLOC_8);
   mem2CryptoRAM_rev(formatted_digest, size, BA414EP_MEMLOC_12);

   // Fetch the signature
   point2CryptoRAM_rev(signature, size, BA414EP_MEMLOC_10);

   /* ECDSA signature verification */
   if(ba414ep_start_wait_status())
      return CRYPTOLIB_INVALID_SIGN_ERR;

   return CRYPTOLIB_SUCCESS;
}


/**
 * @brief perform a bit shift to the right of a large value stored in a byte array
 * @param array value to shift (input and output)
 * @param len length of the \p array
 * @param shift size of the bit shift (between 0 and 7)
 */
//TODO Move this to some "includable" module ?
static void sx_bitshift(uint8_t *array, uint8_t len, uint8_t shift)
{
   if(shift)
   {
      uint8_t prev, val;
      int i;
      prev = 0;
      for(i = 0; i < len; i++)
      {
         val = ((array[i] >> shift)&0xFF) | prev;
         prev = array[i] << (8-shift);
         array[i] = val;
      }
   }
}


/**
 * @brief perform digest computation used for signing or verification
 * @param curve ECDSA curve to use
 * @param message message to sign or verify
 * @param hash_fct algorithm to use for hash
 * @param digest_blk computed digest
 * @return CRYPTOLIB_INVALID_PARAM when there is not enough space in the
 *                                 digest_blk to compute the digest
 *         CRYPTOLIB_SUCCESS otherwise
 */
static uint32_t ecdsa_generate_digest(const sx_ecc_curve_t *curve,
                                      block_t message,
                                      sx_hash_fct_t hash_fct,
                                      block_t *digest_blk)
{
   uint32_t status;
   uint32_t dgst_local_len;
   uint8_t extra_bits;

   uint32_t curve_bytesize = sx_ecc_curve_bytesize(curve);
   uint32_t curve_order_bitsize = sx_math_array_nbits(
                              curve->params.addr + curve_bytesize, curve_bytesize);
   uint32_t curve_order_bytesize = (curve_order_bitsize + 7) / 8;

   if (curve_bytesize > ECC_MAX_KEY_SIZE)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   if (curve->params.len != 5 * curve_bytesize &&
         curve->params.len != 6 * curve_bytesize)
      return CRYPTOLIB_INVALID_PARAM;
   if (digest_blk->len < sx_hash_get_digest_size(hash_fct))
      return CRYPTOLIB_UNSUPPORTED_ERR;

   /* Call hash fct to get digest. */
   status = sx_hash_blk(hash_fct, message, *digest_blk);
   if (status)
      return status;

   /* Define digest size. This only take the most significant bytes when curve
    * is smaller than hash. If it's greater, leading zeroes will be inserted
    * within ecdsa_signature_* functions.
    */
   dgst_local_len = SX_MIN(curve_order_bytesize, sx_hash_get_digest_size(hash_fct));

   /* Shorten the digest to match the expected length */
   digest_blk->len = dgst_local_len;

   /* Bitshift if needed, for curve smaller than digest and with order N not on
    * bytes boundaries.
    */
   extra_bits = (curve_order_bitsize & 0x7);
   if (extra_bits && (sx_hash_get_digest_size(hash_fct) * 8 > curve_order_bitsize))
      sx_bitshift(digest_blk->addr, dgst_local_len,  8-extra_bits);

   return CRYPTOLIB_SUCCESS;
}

uint32_t ecdsa_generate_signature(const sx_ecc_curve_t *curve,
                                  block_t message,
                                  block_t key,
                                  block_t signature,
                                  sx_hash_fct_t hash_fct,
                                  struct sx_rng rng)
{
   uint8_t digest[MAX_DIGESTSIZE];
   block_t digest_blk = BLK_LITARRAY(digest);
   uint32_t status;

   status = ecdsa_generate_digest(curve, message, hash_fct, &digest_blk);
   if (status != CRYPTOLIB_SUCCESS)
      return status;
   return  ecdsa_generate_signature_digest(curve, digest_blk, key, signature,
         rng);
}

uint32_t ecdsa_verify_signature(const sx_ecc_curve_t *curve,
                                block_t message,
                                block_t key,
                                block_t signature,
                                sx_hash_fct_t hash_fct )
{
   uint8_t digest[MAX_DIGESTSIZE];
   block_t digest_blk = BLK_LITARRAY(digest);
   uint32_t status;

   status = ecdsa_generate_digest(curve, message, hash_fct, &digest_blk);
   if (status != CRYPTOLIB_SUCCESS)
      return status;
   return ecdsa_verify_signature_digest(curve, digest_blk, key, signature);
}
