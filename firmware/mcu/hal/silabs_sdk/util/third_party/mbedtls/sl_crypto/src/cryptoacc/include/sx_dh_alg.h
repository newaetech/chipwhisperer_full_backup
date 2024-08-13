/**
 * @file
 * @brief Defines the procedures to make DH operations with
 *          the BA414EP pub key
 * @copyright Copyright (c) 2016-2018 Silex Insight. All Rights reserved
 */


#ifndef SX_DH_ALG_H
#define SX_DH_ALG_H

#include <stdint.h>
#include "compiler_extentions.h"
#include "cryptolib_def.h"
#include "cryptolib_types.h"
#if (DH_MODP_ENABLED)

/**
 * @brief Compute a Diffie Hellman shared key in modular arithmetic
 *
 * It computes this shared key based on a public, a private key and
 * the known generator (see RFC 2631)
 * @param  mod     Modulus of the domain
 * @param  priv    Private key
 * @param  pub     Other party public key
 * @param  shared  Secret shared key (big integer)
 * @param  size    Size in bytes of one parameter
 * @return CRYPTOLIB_SUCCESS if no error
 *         CRYPTOLIB_UNSUPPORTED_ERR if size is bigger than DH_MAX_KEY_SIZE
 */
uint32_t dh_shared_key_modp(
      block_t mod,
      block_t priv,
      block_t pub,
      block_t shared,
      uint32_t size) CHECK_RESULT;

#endif


/**
 * @brief Compute a Diffie Hellman shared key using ECC-Edwards and both
 * ECC-Weierstrass prime and binary
 *
 * @param  domain      Domain of the key
 * @param  priv        Private key
 * @param  pub         Other party public key
 * @param  shared      Shared key (coordinates of a point)
 * @param  size        Size in bytes of one parameter
 * @param  curve_flags Curve acceleration parameters
 * @return CRYPTOLIB_SUCCESS if no error
 *         CRYPTOLIB_UNSUPPORTED_ERR if size is bigger than DH_MAX_KEY_SIZE
 */
uint32_t dh_shared_key_ecdh(
      block_t domain,
      block_t priv,
      block_t pub,
      block_t shared,
      uint32_t size,
      uint32_t curve_flags)  CHECK_RESULT;

/**
 * @brief Compute a Diffie Hellman shared key using ECC-Montgomery
 *
 * @param  domain      Domain of the key
 * @param  priv        Private key
 * @param  pub         Other party public key
 * @param  shared      Shared key (only one coordinate)
 * @param  size        Size in bytes of one parameter
 * @param  curve_flags Curve acceleration parameters
 * @return CRYPTOLIB_SUCCESS if no error
 *         CRYPTOLIB_UNSUPPORTED_ERR if size is bigger than DH_MAX_KEY_SIZE
 */
uint32_t dh_shared_montgomery_key_ecdh(
      block_t domain,
      block_t priv,
      block_t pub,
      block_t shared,
      uint32_t size,
      uint32_t curve_flags);


#endif
