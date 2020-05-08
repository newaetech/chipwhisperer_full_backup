/**
 * @file
 * @brief Defines ECC keygen functions
 * @copyright Copyright (c) 2016-2018 Silex Insight. All Rights reserved
 */


#include "sx_ecc_keygen_alg.h"
#include "cryptolib_def.h"
#include "sx_memcpy.h"
#include "ba414ep_config.h"
#include "sx_rng.h"
#include "sx_primitives.h"
#include "sx_errors.h"

uint32_t ecc_validate_public_key(block_t domain, block_t pub, uint32_t size, uint32_t curve_flags)
{
   uint32_t status;

   if (size > ECC_MAX_KEY_SIZE)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   if (pub.len != 2 * size || (domain.len != 6 * size && domain.len != 5 * size))
      return CRYPTOLIB_INVALID_PARAM;

   ba414ep_set_command(BA414EP_OPTYPE_ECC_CHECK_POINTONCURVE, size, BA414EP_BIGEND, curve_flags);
   ba414ep_load_curve(domain, size, BA414EP_BIGEND, 1);
   point2CryptoRAM_rev(pub, size , BA414EP_MEMLOC_6);

   ba414ep_set_config(BA414EP_MEMLOC_6, BA414EP_MEMLOC_6, BA414EP_MEMLOC_6, 0x0);
   status = ba414ep_start_wait_status();
   if (status)
      return CRYPTOLIB_CRYPTO_ERR;
   return CRYPTOLIB_SUCCESS;
}

uint32_t ecc_generate_keypair(block_t domain, block_t pub, block_t priv, uint32_t size,
      uint32_t curve_flags, struct sx_rng rng)
{
   // Get random number < n -> private key
   block_t n = block_t_convert(domain.addr + size, size);
   uint32_t status = ecc_generate_private_key(n, priv, rng);
   if (status)
      return status;

   //Point mult for Public key
   return ecc_generate_public_key(domain, pub, priv, size, curve_flags);
}

uint32_t ecc_generate_private_key(block_t n, block_t priv, struct sx_rng rng)
{
   if (n.len > ECC_MAX_KEY_SIZE)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   return sx_rng_get_rand_lt_n_blk(priv, n, rng);
}


uint32_t ecc_generate_public_key(block_t curve, block_t pub, block_t priv, uint32_t size, uint32_t curve_flags)
{
   uint32_t status;

   if (size > ECC_MAX_KEY_SIZE)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   // Only domain of 5,6 parameters are supported (Weierstrass p/b and Edwards)
   if (pub.len != 2 * size || priv.len != size ||
         (curve.len != 6 * size && curve.len != 5 * size))
      return CRYPTOLIB_INVALID_PARAM;

   // Set command to enable byte-swap
   ba414ep_set_command(BA414EP_OPTYPE_ECC_POINT_MULT, size, BA414EP_BIGEND, curve_flags);
   // Load parameters
   ba414ep_load_curve(curve, size, BA414EP_BIGEND, 1);

   // Location 14 -> Private key
   mem2CryptoRAM_rev(priv, priv.len, BA414EP_MEMLOC_14);

   /* Set Configuration register */
   ba414ep_set_config(BA414EP_MEMLOC_2, BA414EP_MEMLOC_14, BA414EP_MEMLOC_6, 0x0);

   /* Start ECC Point Mult */
   status = ba414ep_start_wait_status();
   if (status)
      return CRYPTOLIB_CRYPTO_ERR;

   // Fetch the results
   CryptoRAM2point_rev(pub, size, BA414EP_MEMLOC_6);

   return CRYPTOLIB_SUCCESS;
}


uint32_t ecc_montgomery_generate_keypair(block_t curve, block_t pub, block_t priv,
      uint32_t size, uint32_t curve_flags, struct sx_rng rng)
{
   // Get random private key
   ecc_montgomery_generate_private_key(priv, rng);

   // Point mult for Public key
   return ecc_montgomery_mult(curve, curve_flags, size, priv, NULL_blk, pub);
}

uint32_t ecc_montgomery_generate_private_key(block_t priv, struct sx_rng rng)
{
   if (priv.len > ECC_MAX_KEY_SIZE)
      return CRYPTOLIB_INVALID_PARAM;

   rng.get_rand_blk(rng.param, priv);
   return CRYPTOLIB_SUCCESS;

}

uint32_t ecc_montgomery_generate_public_key(block_t curve, block_t pub, block_t priv, uint32_t size, uint32_t curve_flags)
{
   // All the required tests are internally handled by ecc_montgomery_mult
   return ecc_montgomery_mult(curve, curve_flags, size, priv, NULL_blk, pub);
}
