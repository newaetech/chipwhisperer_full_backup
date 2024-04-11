/**
 * @file
 * @brief Defines the procedures to make ECDSA operations with
 *          the BA414EP pub key
 *
 * Default format for ECC is big array of uint8_t (big endian) for both
 * private and public key. Public key is represented as the concatenation
 * of x and y coordinate for all but Montgomery and EdDSA.
 *
 * @copyright Copyright (c) 2016-2018 Silex Insight. All Rights reserved
 */


#ifndef SX_ECDSA_ALG_H
#define SX_ECDSA_ALG_H

#include <stdint.h>
#include "cryptolib_types.h"
#include "sx_ecc_curves.h"
#include "sx_hash.h"
#include "sx_rng.h"


/**
 * @brief Validates curve parameters when given by the host \p curve
 *
 * The checks done over the curve parameters are described in BA414EP_PKE_DataSheet
 * Section Elliptic Curve Digital Signature Algorithm
 * @param curve the ECC curve to validate
 * @return ::CRYPTOLIB_SUCCESS if successful
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the size of one element curve
 *         (\c curve->bytesize) is bigger than ::ECC_MAX_KEY_SIZE
 *         ::CRYPTOLIB_CRYPTO_ERR if there is an issue with the parameters
 */
uint32_t ecdsa_validate_domain (const sx_ecc_curve_t *curve);

/**
 * @brief Verify an ECDSA signature from a \p formatted_digest
 *
 * @param curve            Selects the ECC curve
 * @param formatted_digest Digest to verify against \p signature.
 * @param key              ECC public key
 * @param signature        Signature to verify against the \p formatted_digest
 * @return ::CRYPTOLIB_SUCCESS if successful
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the size of one element curve
 *         (\c curve->bytesize) is bigger than ::ECC_MAX_KEY_SIZE
 *         ::CRYPTOLIB_INVALID_SIGN_ERR if verification failed
 *
 * \note the \p formatted_digest corresponds to the digest truncated if required
 *       to match the ecc curve element size and also right shifted if required
 *       (left padded with zeros)
 */
uint32_t ecdsa_verify_signature_digest(
      const sx_ecc_curve_t *curve,
      block_t formatted_digest,
      block_t key,
      block_t signature);

/**
 * @brief Configure hardware for signature generation.
 *
 * Load the curve parameters, the key and formatted_digest. Those are
 * needed once before attempting to generate signatures.
 * @param curve            Selects the ECC curve
 * @param formatted_digest Digest to generate the signature.
 * @param key              ECC private key
 * @return ::CRYPTOLIB_SUCCESS if successful
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the size of one element curve
 *         (\c curve->bytesize) is bigger than ::ECC_MAX_KEY_SIZE
 *
 * \note the \p formatted_digest corresponds to the digest truncated if required
 *       to match the ecc curve element size and also right shifted if required
 *       (left padded with zeros)
 */
uint32_t sx_ecdsa_configure_signature(
      const sx_ecc_curve_t *curve,
      block_t formatted_digest,
      block_t key);

/**
 * @brief Attempt to compute an ECDSA signature
 *
 * This function tries to generate a signature and gives up immediately in case
 * of error. It is under the caller responsibility to attempt again to generate
 * a signature.
 * @param curve            Selects the ECC curve
 * @param signature        Output signature of the \p formatted_digest
 * @param rng              Used random number generator
 * @return ::CRYPTOLIB_SUCCESS if the signature is generated and valid,
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the size of one element curve
 *         (\c curve->bytesize) is bigger than ::ECC_MAX_KEY_SIZE
 *         ::CRYPTOLIB_INVALID_SIGN_ERR if the generated signature is not valid
 *
 * \warning The digest and the key must be already loaded (i.e: by calling
 *          ::sx_ecdsa_configure_signature first)
 * \warning It is required to call ::sx_ecdsa_configure_signature \e before
 *          attempting to generate a signature as a few parameters have to be
 *          loaded.
 */
uint32_t sx_ecdsa_attempt_signature(
      const sx_ecc_curve_t *curve,
      block_t signature,
      struct sx_rng rng);

/**
 * @brief Verifies a given signature corresponds to a given message
 *
 * @param curve     Selects the ECC curve
 * @param message   Message to hash and verify against \p signature
 * @param key       ECC public key
 * @param signature Signature to check
 * @param hash_fct  Select hash algorithm to use on \p message
 * @return ::CRYPTOLIB_SUCCESS if successful
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the size of one element curve
 *         (\c curve->bytesize) is bigger than ::ECC_MAX_KEY_SIZE
 *         ::CRYPTOLIB_INVALID_SIGN_ERR if signature verification failed
 */
uint32_t ecdsa_verify_signature(
      const sx_ecc_curve_t *curve,
      block_t message,
      block_t key,
      block_t signature,
      sx_hash_fct_t hash_fct);

/**
 * @brief Compute a ECDSA signature for a given digest
 *
 * Unlike ::sx_ecdsa_attempt_signature, this function will retry in case of
 * invalid generated signature and both the formatted_digest and the key are
 * internally loaded.
 *
 * @param curve            Selects the ECC curve
 * @param formatted_digest Digest to use to generate \p signature.
 * @param key              ECC private key
 * @param signature        Output signature of the \p formatted_digest
 * @param rng              Used random number generator
 * @return ::CRYPTOLIB_SUCCESS if successful
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the size of one element curve
 *         (\c curve->bytesize) is bigger than ::ECC_MAX_KEY_SIZE
 *         ::CRYPTOLIB_INVALID_SIGN_ERR if signature verification failed
 *
 * \note the \p formatted_digest corresponds to the digest truncated if required
 *       to match the ecc curve element size and also right shifted if required
 *       (left padded with zeros)
 */

uint32_t ecdsa_generate_signature_digest(
      const sx_ecc_curve_t *curve,
      block_t formatted_digest,
      block_t key,
      block_t signature,
      struct sx_rng rng);

/**
 * @brief Generates an ECDSA signature from a \p message
 *
 * @param curve     Selects the ECC curve
 * @param message   Message to sign with ECDSA
 * @param key ECC   Private key
 * @param signature Location to store the signature
 * @param hash_fct  Select hash algorithm to use on \p message
 * @param rng       Used random number generator
 * @return ::CRYPTOLIB_SUCCESS if successful
 *         ::CRYPTOLIB_UNSUPPORTED_ERR if the size of one element curve
 *         (\c curve->bytesize) is bigger than ::ECC_MAX_KEY_SIZE
 */
uint32_t ecdsa_generate_signature(
      const sx_ecc_curve_t *curve,
      block_t message,
      block_t key,
      block_t signature,
      sx_hash_fct_t hash_fct,
      struct sx_rng rng);

#endif
