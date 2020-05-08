/**
 * @file
 * @brief Defines the procedures to make primitive operations with the BA414EP
 * Public Key Engine
 * @copyright Copyright (c) 2016-2018 Silex Insight. All Rights reserved
 */


#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <stdint.h>
#include "compiler_extentions.h"
#include "cryptolib_types.h"


/** @brief Modular Addition: C = (A + B) mod P
 *  @param a is a pointer to the first operand (this must point to a location
 *           in the System Memory, not in the CryptoRAM).
 *  @param b is a pointer to the second operand (this must point to a location
 *           in the System Memory, not in the CryptoRAM).
 *  @param modulo is a pointer to the modulo (this must point to
 *                a location in the System Memory, not in the CryptoRAM).
 *  @param result is a pointer where the result has to be copied (this must
 *                point to a location in the System Memory, not in the
 *                CryptoRAM).
 *  @param size   represents the size of the operands in bytes.
 *  @return CRYPTOLIB_SUCCESS when no error occurs.
 */
uint32_t modular_addition(
      const uint8_t *a,
      const uint8_t *b,
      const uint8_t *modulo,
      uint8_t *result,
      uint32_t size) CHECK_RESULT;

/** @brief Modular Subtraction: C = (A - B) mod P
 *  @param a is a pointer to the first operand (this must point to a location
 *           in the System Memory, not in the CryptoRAM).
 *  @param b is a pointer to the second operand (this must point to a location
 *           in the System Memory, not in the CryptoRAM).
 *  @param modulo is a pointer to the modulo (this must point to a location in
 *                the System Memory, not in the CryptoRAM).
 *  @param result is a pointer where the result has to be copied (this must
 *                point to a location in the System Memory, not in the
 *                CryptoRAM).
 *  @param size represents the size of the operands in bytes.
 *  @return CRYPTOLIB_SUCCESS when no error occurs.
 */
uint32_t modular_substraction(
      const uint8_t *a,
      const uint8_t *b,
      const uint8_t *modulo,
      uint8_t *result,
      uint32_t size) CHECK_RESULT;

/** @brief Modular Multiplication: C = (A * B) mod P
 *  @param a is a pointer to the first operand (this must point to a location in
 *           the System Memory, not in the CryptoRAM).
 *  @param b is a pointer to the second operand (this must point to a location
 *           in the System Memory, not in the CryptoRAM).
 *  @param modulo is a pointer to the modulo (this must point to a location in
 *                the System Memory, not in the CryptoRAM).
 *  @param result is a pointer where the result has to be copied (this must
 *                point to a location in the System Memory, not in the
 *                CryptoRAM).
 *  @param size represents the size of the operands in bytes.
 *  @return CRYPTOLIB_SUCCESS when no error occurs.
 */
uint32_t modular_multiplication(
      const uint8_t *a,
      const uint8_t *b,
      const uint8_t *modulo,
      uint8_t *result,
      uint32_t size) CHECK_RESULT;

/** @brief Modular Reduction: C = A mod P
 *  @param a is a pointer to the first operand (this must point to a location in
 *           the System Memory, not in the CryptoRAM).
 *  @param modulo is a pointer to the modulo (this must point to a location in
 *                the System Memory, not in the CryptoRAM), must be odd.
 *  @param result is a pointer where the result has to be copied (this must
 *                point to a location in the System Memory, not in the CryptoRAM).
 *  @param size represents the size of the operands in bytes.
 *  @return CRYPTOLIB_SUCCESS when no error occurs.
 */
uint32_t modular_reduction(
      const uint8_t *a,
      const uint8_t *modulo,
      uint8_t *result,
      uint32_t size) CHECK_RESULT;

/** @brief Modular Division: C = (A/B) mod P
 *  @param a is a pointer to the first operand (this must point to a location in
 *           the System Memory, not in the CryptoRAM).
 *  @param b is a pointer to the second operand (this must point to a location
 *         in the System Memory, not in the CryptoRAM).
 *  @param modulo is a pointer to the modulo (this must point to a location in
 *                the System Memory, not in the CryptoRAM).
 *  @param result is a pointer where the result has to be copied (this must
 *                point to a location in the System Memory, not in the CryptoRAM).
 *  @param size represents the size of the operands in bytes.
 *  @return CRYPTOLIB_SUCCESS when no error occurs.
 */
uint32_t modular_division(
      const uint8_t *a,
      const uint8_t *b,
      const uint8_t *modulo,
      uint8_t *result,
      uint32_t size) CHECK_RESULT;

/** @brief Modular Inversion: C = (1/A) mod P
 *  @param a is a pointer to the first operand (this must point to a location
 *           in the System Memory, not in the CryptoRAM).
 *  @param modulo is a pointer to the modulo (this must point to a location in
 *                the System Memory, not in the CryptoRAM).
 *  @param result is a pointer where the result has to be copied (this must
 *                point to a location in the System Memory, not in the
 *                CryptoRAM).
 *  @param size represents the size of the operands in bytes.
 *  @return CRYPTOLIB_SUCCESS when no error occurs.
 */
uint32_t modular_inversion(
      const uint8_t *a,
      const uint8_t *modulo,
      uint8_t *result,
      uint32_t size) CHECK_RESULT;

/** @brief Multiplication: C = A * B
 *  @param a is a pointer to the first operand (this must point to a location in
 *           the System Memory, not in the CryptoRAM).
 *  @param b is a pointer to the second operand of the Multiplication (this must
 *           point to a location in the System Memory, not in the CryptoRAM).
 *  @param result is a pointer where the result has to be copied (this must
 *                point to a location in the System Memory, not in the
 *                CryptoRAM).
 *  @param size represents the size of the operands in bytes.
 *  @return CRYPTOLIB_SUCCESS when no error occurs.
 */
uint32_t multiplication(
      const uint8_t *a,
      const uint8_t *b,
      uint8_t *result,
      uint32_t size) CHECK_RESULT;

/** @brief Modular Exponentiation: result =  (A ^ B) mod P
 *  @param a is a pointer to the first operand (this must point to a location in
 *           the System Memory, not in the CryptoRAM).
 *  @param b is a pointer to the key of the modular exponentiation (this must
 *           point to a location in the System Memory, not in the CryptoRAM).
 *  @param modulo is a pointer to the modulo of the modular exponentiation (this
 *                must point to a location in the System Memory, not in the
 *                CryptoRAM).
 *  @param result is a pointer where the result has to be copied (this must
 *                point to a location in the System Memory, not in the
 *                CryptoRAM).
 *  @param size represents the size of the operands in bytes.
 *  @return CRYPTOLIB_SUCCESS when no error occurs.
 */
 uint32_t modular_exponentiation(
      const uint8_t *a,
      const uint8_t *b,
      const uint8_t *modulo,
      uint8_t *result,
      uint32_t size) CHECK_RESULT;

/** @brief Modular Exponentiation: C = (A ^ B) mod P using block_t operands
*  @param a is a pointer to the first operand (this must point to a location in
*           the System Memory, not in the CryptoRAM).
*  @param b is a pointer to the second operand (this must point to a location in
*           the System Memory, not in the CryptoRAM).
*  @param modulo is a pointer to the modulo (this must point to a location in
*                the System Memory, not in the CryptoRAM).
*  @param result is a pointer where the result has to be copied (this must point
*                to a location in the System Memory, not in the CryptoRAM).
*  @param size represents the size of the operands in bytes.
*  @return CRYPTOLIB_SUCCESS when no error occurs.
*/
uint32_t modular_exponentiation_blk(block_t a, block_t b, block_t modulo,
      block_t result, uint32_t  size) CHECK_RESULT;

/** @brief Montgomery point multiplication
 *  @param curve domain of the curve to use
 *  @param curve_flags flags of the curve to use
 *  @param size curve size in bytes
 *  @param scalar input
 *  @param point input.
 *         If set to ::NULL_blk, generator provided in \p curve will be used
 *  @param result output
 *  @return CRYPTOLIB_SUCCESS if successful, error code otherwise
 */
uint32_t ecc_montgomery_mult(block_t curve, uint32_t curve_flags, uint32_t size,
      block_t scalar, block_t point, block_t result) CHECK_RESULT;

/** @brief ECC point inversion
 *  @param curve domain of the curve to use [input]
 *  @param point point to be inverted [input]
 *  @param result the inverted point [output]
 *  @param size curve size in bytes [input]
 *  @return CRYPTOLIB_SUCCESS if successful, error code otherwise
 */
uint32_t ecc_point_inverse(block_t curve, block_t point, block_t result,
         uint32_t size) CHECK_RESULT;

/** @brief ECC point decompression
 *  @param curve domain of the curve to use [input]
 *  @param point_x X coordinate of the compressed point [input]
 *  @param y_bit least significant bit of Y coordinate of the point [input]
 *  @param point_y Y coordinate after decompression [output]
 *  @param size curve size in bytes [input]
 *  @param curve_flags flags of the curve to use [input]
 *  @return CRYPTOLIB_SUCCESS if successful, error code otherwise
 */
uint32_t ecc_point_decompress(block_t curve, block_t point_x, uint8_t y_bit,
         block_t point_y, uint32_t size, uint32_t curve_flags) CHECK_RESULT;

/** @brief solving Y^2 in accordance to the equation y^2 = x^3 + ax + b mod p
 *  @param curve domain of the curve to use [input]
 *  @param point_x X coordinate of the compressed point [input]
 *  @param point_y2 Y^2 the output of the equation as described (y^2 = x^3 + ax + b mod p) [output]
 *  @param size curve size in bytes [input]
 *  @return CRYPTOLIB_SUCCESS if successful, error code otherwise
 */
uint32_t ecc_point_compute_y2(block_t curve, block_t point_x, block_t point_y2,
      uint32_t size) CHECK_RESULT;

/** @brief ECC point multiplication (result = scalar * point)
 *  @param curve domain of the curve to use [input]
 *  @param scalar integer value to be multiplied by the point [input]
 *  @param point point on the curve to be multiplied with [input]
 *  @param result output of the multiplication operation [output]
 *  @param size curve size in bytes [input]
 *  @param curve_flags flags of the curve to use [input]
 *  @return CRYPTOLIB_SUCCESS if successful, error code otherwise
 */
uint32_t ecc_point_mul(block_t curve, block_t scalar, block_t point,
      block_t result, uint32_t size, uint32_t curve_flags) CHECK_RESULT;

/** @brief ECC point addition (result = point_a + point_b)
 *  @param curve domain of the curve to use [input]
 *  @param point_a the first point summand of the equation [input]
 *  @param point_b the second point summand of the equation [input]
 *  @param result output of the point addition operation [output]
 *  @param size curve size in bytes [input]
 *  @param curve_flags flags of the curve to use [input]
 *  @return CRYPTOLIB_SUCCESS if successful, error code otherwise
 */
uint32_t ecc_point_add(block_t curve, block_t point_a, block_t point_b,
       block_t result, uint32_t size, uint32_t curve_flags) CHECK_RESULT;

/**
 * @brief Validates if an ECC point is on the curve
 * @param curve Curve parameters [input]
 * @param point ECC point to check if it lies on the curve [input]
 * @param size curve size in bytes [input]
 * @param curve_flags flags of the curve to use [input]
 * @return CRYPTOLIB_SUCCESS if point lies on the curve
 * @return CRYPTOLIB_CRYPTO_ERR if thr point does not lie on the curve
 */
uint32_t ecc_is_point_on_curve(block_t curve, block_t point, uint32_t size,
       uint32_t curve_flags) CHECK_RESULT;

/**
 * @brief Multiply and add ECC Weierstrass points
 *        \p result = \p scalar_m * \p point1 + \p scalar_n * \p point2
 *
 * @param curve       ECC curve on which to compute point addition
 * @param scalar_m    will mutliply the first point
 * @param point1      the first point to multiply and add
 * @param scalar_n    will mutliply the second point
 * @param point2      the second point to multiply and add
 * @param result      the result of the mutliply/add operation
 * @param size        size of one element (expressed in bytes)
 * @param curve_flags curve acceleration parameters
 * @return  CRYPTOLIB_SUCCESS if no error
 */
uint32_t ecc_pt_muladd(
      block_t curve,
      block_t scalar_m,
      block_t point1,
      block_t scalar_n,
      block_t point2,
      block_t result,
      uint32_t size,
      uint32_t curve_flags);


#endif
