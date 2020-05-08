/**
 * @file
 * @brief Generate ECC key
 *
 * Default format for ECC is big array of uint8_t (big endian) for both
 * private and public key. Public key is represented as the concatenation
 * of x and y coordinate for all but Montgomery and EdDSA.
 *
 * @copyright Copyright (c) 2016-2018 Silex Insight. All Rights reserved
 */


#ifndef ECC_KEYGEN_ALG
#define ECC_KEYGEN_ALG

#include <stdint.h>
#include "compiler_extentions.h"
#include "cryptolib_types.h"
#include "sx_rng.h"

/**
 * @brief Validates the ECC public key is a point on the curve
 *        (valid for Weierstrass prime, binary or Edwards)
 *
 * @param  domain       Curve parameters
 * @param  pub          Public key to validate
 * @param  size         Size of one element (expressed in bytes)
 * @param  curve_flags  Curve acceleration parameters
 * @return ::CRYPTOLIB_SUCCESS if public key is valid
 * @return ::CRYPTOLIB_CRYPTO_ERR if public key is invalid
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if \p size is bigger than ::ECC_MAX_KEY_SIZE
 */
uint32_t ecc_validate_public_key(
      block_t domain,
      block_t pub,
      uint32_t size,
      uint32_t curve_flags) CHECK_RESULT;

/**
 * @brief Generates an ECC key pair (private and public key)
 *        (valid for Weierstrass prime, binary or Edwards)
 *
 * @param  domain      Curve parameters (domain)
 * @param  pub         Public key to be generated
 * @param  priv        Private key to be generated
 * @param  size        Size of one element (expressed in bytes)
 * @param  curve_flags Curve acceleration parameters
 * @param  rng         Random number generator used
 * @return ::CRYPTOLIB_SUCCESS if no error
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if \p size is bigger than ::ECC_MAX_KEY_SIZE
 */
 uint32_t ecc_generate_keypair(
      block_t domain,
      block_t pub,
      block_t priv,
      uint32_t size,
      uint32_t curve_flags,
      struct sx_rng rng) CHECK_RESULT;

/**
 * @brief Generates an ECC private key
 *        (valid for Weierstrass prime, binary or Edwards)
 *
 * @param  n           Order coming from the curve parameters
 * @param  priv        Private key to be generated
 * @param  rng         Random number generator used
 * @return ::CRYPTOLIB_SUCCESS if no error
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if modulus length is bigger
 *         than ::ECC_MAX_KEY_SIZE
 */
 uint32_t ecc_generate_private_key(
      block_t n,
      block_t priv,
      struct sx_rng rng) CHECK_RESULT;

/**
 * @brief Generates a ECC public key
 *        (Weierstrass prime, binary or Edwards)
 *        based on the curve and a given private key.
 *
 * @param  curve       Curve parameters (domain)
 * @param  pub         Public key to be generated
 * @param  priv        Private key for which public key is desired
 * @param  size        Size of one element (expressed in bytes)
 * @param  curve_flags curve acceleration parameters
 * @return ::CRYPTOLIB_SUCCESS if no error
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if \p size is bigger than ::ECC_MAX_KEY_SIZE
 */
uint32_t ecc_generate_public_key(
      block_t curve,
      block_t pub,
      block_t priv,
      uint32_t size,
      uint32_t curve_flags) CHECK_RESULT;

/**
 * @brief Generates a key pair for ECC Montgomery curve \p curve
 *
 * @param  domain      Curve parameters (domain)
 * @param  pub         Public key to be generated
 * @param  priv        Private key to be generated
 * @param  size        Size of one element (expressed in bytes)
 * @param  curve_flags Curve acceleration parameters
 * @param  rng         Used random number generator
 * @return ::CRYPTOLIB_SUCCESS if no error
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if \p size is bigger than ::ECC_MAX_KEY_SIZE
 */
uint32_t ecc_montgomery_generate_keypair(
      block_t domain,
      block_t pub,
      block_t priv,
      uint32_t size,
      uint32_t curve_flags,
      struct sx_rng rng) CHECK_RESULT;

/**
 * @brief Generates a Montgomery ECC private key
 *
 * @param  priv        Private key to be generated
 * @param  rng         Used random number generator
 * @return ::CRYPTOLIB_SUCCESS if no error
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if private key length is bigger than ::ECC_MAX_KEY_SIZE
 */
uint32_t ecc_montgomery_generate_private_key(
      block_t priv,
      struct sx_rng rng);

/**
 * @brief Generates a Montgomery ECC public key
 *        based on the curve and a given private key.
 *
 * @param  curve       Curve parameters (domain)
 * @param  pub         Public key to be generated
 * @param  priv        Private key for which public key is desired
 * @param  size        Size of one element (expressed in bytes)
 * @param  curve_flags Curve acceleration parameters
 * @return ::CRYPTOLIB_SUCCESS if no error
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if \p size is bigger than ::ECC_MAX_KEY_SIZE
 */
uint32_t ecc_montgomery_generate_public_key(
      block_t curve,
      block_t pub,
      block_t priv,
      uint32_t size,
      uint32_t curve_flags) CHECK_RESULT;

#endif
