/**
 * @file
 * @brief Defines the shared key generation for DH protocoles
 * @copyright Copyright (c) 2016-2018 Silex Insight. All Rights reserved
 */

#include "sx_dh_alg.h"
#include "cryptolib_def.h"
#include "sx_memcpy.h"
#include "ba414ep_config.h"
#include "sx_primitives.h"
#include "sx_errors.h"


#if (DH_MODP_ENABLED)

uint32_t dh_shared_key_modp(block_t mod, block_t priv, block_t pub, block_t shared, uint32_t size)
{
   if (size > DH_MAX_KEY_SIZE)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   if (mod.len != size ||
         priv.len != size ||
         pub.len != size ||
         shared.len != size)
      return CRYPTOLIB_INVALID_PARAM;
   return modular_exponentiation_blk(pub, priv, mod, shared, size);
}

#endif

uint32_t dh_shared_key_ecdh(block_t curve, block_t priv, block_t pub, block_t shared, uint32_t size, uint32_t curve_flags)
{
   uint32_t error;

   if (size > ECC_MAX_KEY_SIZE)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   if ((curve.len != 5 * size && curve.len != 6 * size) ||
         priv.len != size ||
         pub.len != 2 * size ||
         shared.len != 2 * size)
      return CRYPTOLIB_INVALID_PARAM;

   static BA414EPRegs_t * const BA414EP_REGS = (BA414EPRegs_t *)ADDR_BA414EP_REGS;

   // Set command to enable byte-swap
   ba414ep_set_command(BA414EP_OPTYPE_ECC_POINT_MULT, size, BA414EP_BIGEND, curve_flags);

   /* Set Configuration register */
   ba414ep_set_config(BA414EP_MEMLOC_12, BA414EP_MEMLOC_14, BA414EP_MEMLOC_6, 0x0);
   // Load parameters
   ba414ep_load_curve(curve, size, BA414EP_BIGEND, 1);

   // Location 14 -> Private key
   mem2CryptoRAM_rev(priv, size, BA414EP_MEMLOC_14);
   // Location 12 -> Public key x, Location 13 -> Public key y
   point2CryptoRAM_rev(pub, size, BA414EP_MEMLOC_12);

   BA414EP_REGS->CommandReg = (BA414EP_REGS->CommandReg & ~BA414EP_CMD_OPTYPE_MASK) | BA414EP_OPTYPE_ECC_CHECK_POINTONCURVE;

   // Check point on curve
   error = ba414ep_start_wait_status();
   if (error)
      return CRYPTOLIB_CRYPTO_ERR;

  // The public key is a point on curve. Continue with multiplication.
  BA414EP_REGS->CommandReg = (BA414EP_REGS->CommandReg & ~BA414EP_CMD_OPTYPE_MASK) | BA414EP_OPTYPE_ECC_POINT_MULT;

   // Start BA414EP
   error = ba414ep_start_wait_status();
   if (error)
      return CRYPTOLIB_CRYPTO_ERR;

   // Fetch the results
   CryptoRAM2point_rev(shared, size, BA414EP_MEMLOC_6);

   return CRYPTOLIB_SUCCESS;
}



uint32_t dh_shared_montgomery_key_ecdh(block_t curve, block_t priv, block_t pub, block_t shared, uint32_t size, uint32_t curve_flags)
{
   if (size > ECC_MAX_KEY_SIZE)
      return CRYPTOLIB_UNSUPPORTED_ERR;
   if (curve.len != 3 * size ||
         priv.len != size ||
         pub.len != size ||
         shared.len != size)
      return CRYPTOLIB_INVALID_PARAM;
   return ecc_montgomery_mult(curve, curve_flags, size, priv, pub, shared);
}
