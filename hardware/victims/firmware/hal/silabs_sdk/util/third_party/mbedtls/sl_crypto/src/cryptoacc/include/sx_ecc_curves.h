/**
 * @file
 * @brief This file contains the hard-coded curve parameters
 *          for common ECC
 * @copyright Copyright (c) 2016-2018 Silex Insight. All Rights reserved
 */


#ifndef SX_ECC_CURVES_H
#define SX_ECC_CURVES_H

#include <stdint.h>
#include <stdbool.h>
#include "cryptolib_types.h"


/** @brief The (mathematical) implemented fields used for ECC */
typedef enum sx_ecc_curve_type
{
   sx_ecc_curve_prime,   /**< Prime field curve*/
   sx_ecc_curve_binary,  /**< Binary field curve*/
   sx_ecc_curve_edwards, /**< Edwards curve */
   sx_ecc_curve_unspecified /**< If user does not care the field */
} sx_ecc_curve_type;

/**
 * @brief Build the ::sx_ecc_curve_t.pk_flags for a user-defined curve
 *
 * The ECC PK engine is expecting a few flags to be set by the caller (i.e.
 * the field to use for computation).
 * These flags are correctly set for predefined curves but not for user-defined
 * curves. To allow customers to use their own curves, this function is provided.
 * (see BA414EP_PKE_Datasheet.pdf for the different flag meanings).
 *
 * \param curve_type curve type used to do the ecc computation (see ::sx_ecc_curve_type)
 *
 * \return a \p flags value which may be used for ECC operation
 */
uint32_t sx_ecc_build_pk_flags(enum sx_ecc_curve_type curve_type);


/**
 * @brief Define the default case when no specific BA414EP flags are required
 *        (see ::sx_ecc_build_pk_flags)
 */
#define SX_ECC_DEFAULT_PK_FLAGS (sx_ecc_build_pk_flags(sx_ecc_curve_unspecified))

/** @brief ECC structure for ECC curve definition */
typedef struct sx_ecc_curve_t {
   block_t  params;  /**< points to predefined curve params
      formatted as {q, n, G<SUB>x</SUB>, G<SUB>y</SUB>, a, b} and
      represented in big number array of uint8 (big endian).
      The fileds are:
      - q, the modulus (or the reduction polynomial for ECC Weierstrass binary)
      - n, the order of the curve
      - G<SUB>x</SUB>, G<SUB>y</SUB>, the x and y coordinates of the generator
      - a, b (or d for Edwards curve), the coefficients defining the ECC curve*/
   uint32_t pk_flags; /**< required flags for the PK engine*/
   uint32_t bytesize; /**< size expressed in bytes */
} sx_ecc_curve_t;


/**
 * @brief Get size of \p curve in bytes
 * @param curve is a pointer to a ::sx_ecc_curve_t structure describing the curve
 */
uint32_t sx_ecc_curve_bytesize(const sx_ecc_curve_t *curve);

extern const sx_ecc_curve_t sx_ecc_curve_p192;
extern const sx_ecc_curve_t sx_ecc_curve_p224;
extern const sx_ecc_curve_t sx_ecc_curve_p256;
extern const sx_ecc_curve_t sx_ecc_curve_p384;
extern const sx_ecc_curve_t sx_ecc_curve_p521;
extern const sx_ecc_curve_t sx_ecc_curve_b163;
extern const sx_ecc_curve_t sx_ecc_curve_b233;
extern const sx_ecc_curve_t sx_ecc_curve_b283;
extern const sx_ecc_curve_t sx_ecc_curve_b409;
extern const sx_ecc_curve_t sx_ecc_curve_b571;
extern const sx_ecc_curve_t sx_ecc_curve_k163;
extern const sx_ecc_curve_t sx_ecc_curve_k233;
extern const sx_ecc_curve_t sx_ecc_curve_k283;
extern const sx_ecc_curve_t sx_ecc_curve_k409;
extern const sx_ecc_curve_t sx_ecc_curve_k571;
extern const sx_ecc_curve_t sx_ecc_curve_e521;
extern const sx_ecc_curve_t sx_ecc_curve_curve25519;
extern const sx_ecc_curve_t sx_ecc_curve_curve448;
extern const sx_ecc_curve_t sx_ecc_curve_sm2_p256_v1;
extern const sx_ecc_curve_t sx_ecc_curve_sm2_p256_v2;
extern const sx_ecc_curve_t sx_ecc_curve_sm2_fm257;
extern const sx_ecc_curve_t sx_ecc_bp_curve_p160;
extern const sx_ecc_curve_t sx_ecc_bp_curve_p192;
extern const sx_ecc_curve_t sx_ecc_bp_curve_p224;
extern const sx_ecc_curve_t sx_ecc_bp_curve_p256;
extern const sx_ecc_curve_t sx_ecc_bp_curve_p320;
extern const sx_ecc_curve_t sx_ecc_bp_curve_p384;
extern const sx_ecc_curve_t sx_ecc_bp_curve_p512;


#endif
